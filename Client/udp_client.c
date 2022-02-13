/* 
 * udpclient.c - A simple UDP client
 * usage: udpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>


#define BUFSIZE 1024
#define ACK 0
#define DATA 1

//structs
struct frame{
    int frame_type; //ACK: 0, DATA:1
    int seq_no; //0 or 1
    char buf[BUFSIZE]; //buffer to store payload
};

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}
/*function prototypes*/
int send_wait_ack(struct frame * sendframe, struct frame * recvframe, int fd, struct sockaddr_in * sa);
int recv_send_ack(struct frame * sendframe, struct frame * recvframe, int fd, struct sockaddr_in * sa);

//globals
extern int errno;
int last_recv_seq = 1;
int last_sent_seq = 0;

int main(int argc, char **argv) {
    int sockfd, portno, ip_valid;
    ssize_t n;
    socklen_t serverlen;
    struct sockaddr_in serveraddr;
    struct hostent *server;
//    char *hostname;
    char buf[BUFSIZE];
    struct in_addr ipaddr;
    char *ip;

    //set stdout buffer size to 0, for debugging purpose...CLion not flushing stdout buffer...
    setbuf(stdout, 0);

    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }
//    hostname = argv[1];
    ip = argv[1];
    ip_valid = inet_aton(ip, &ipaddr);
    if (!ip_valid){
        error("IP not valid");
    }

    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* gethostbyaddr: get the server based on IP Address*/
    server = gethostbyaddr((const void *)&ipaddr, sizeof(ipaddr), AF_INET);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", ip);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* get a message from the user */
    /*bzero(buf, BUFSIZE);
    printf("Please enter msg: ");
    fgets(buf, BUFSIZE, stdin);*/

    /*Main while loop*/
    while (1){
        char command[50];
        char * instr;
        char * filename;
        struct frame send_frame;
        struct frame recv_frame;
        bzero(send_frame.buf, BUFSIZE);

        printf("List of commands:\nput [filename]\nget [filename]\ndelete [filename]\nls\nexit\n\nInput command:");
        fgets(command, sizeof(command), stdin); //get input from user
        command[strcspn(command, "\n")] = 0;    //remove trailing newline

        instr = strtok(command, " ");   //determine command based on first word

        /*put command*/
        if (strcmp(instr, "put") == 0){
            filename = strtok(NULL, " ");  //extract filename
            //handle if no filename provided
            if (filename == NULL) {
                printf("No filename provided. Press ENTER to continue\n");
                getchar();
                continue;
            }
            /*process if file is in system*/
            if( access( filename, F_OK ) == 0 ) {
                //file present
                //do nothing
            }
            else {
                // file doesn't exist
                printf("errno %d\n", errno);
                perror("");
                printf("Press ENTER to continue\n");
                getchar();
                continue;
            }

            /*send  put command*/
            strcpy(send_frame.buf, instr);
            int ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
            if (ackd == 1) {
                printf("Command was sent unsuccessfully. Press ENTER to continue\n");
                getchar();
                continue;
            }

            //send filename
            bzero(send_frame.buf, BUFSIZE);
            strcpy(send_frame.buf, filename);
            ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
            printf("SENT COMMAND: %s %s\n", instr, filename);
            if (ackd == 1) {
                printf("Command was sent unsuccessfully. Press ENTER to continue\n");
                getchar();
                continue;
            }

            /*open file and determine its size*/
            FILE * fp = fopen(filename, "r");
            fseek(fp, 0, SEEK_END);
            int size = ftell(fp);   //get size of file
            fseek(fp, 0, SEEK_SET);

            /*send filesize to server*/
            bzero(send_frame.buf, BUFSIZE);
            char str_size[BUFSIZE];
            sprintf(str_size, "%d", size);
            strcpy(send_frame.buf, str_size);
            ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &serveraddr);

            if (ackd ==1) {
                printf("File size was sent unsuccessfully. Press ENTER to continue\n");
                getchar();
                continue;
            }

            /*sending file*/
            while (size) {
                bzero(send_frame.buf, BUFSIZE);
                int bytes_read = fread(send_frame.buf, sizeof(char), BUFSIZE, fp);

                /*send buffer to server*/
                ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &serveraddr);

                if (ackd == 1) {
                    printf("Data was sent unsuccessfully. Press ENTER to continue\n");
                    getchar();
                    continue;
                }
                size -= bytes_read;
            }
            printf("Finished sending file\n");
            fclose(fp);
        }

        /*get command*/
        else if (strcmp(instr,"get") == 0){
            filename = strtok(NULL, " ");  //extract filename

            //send instruction to server
            strcpy(send_frame.buf, instr);
            int ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
            if (ackd == 1) {
                printf("Command was sent unsuccessfully. Press ENTER to continue\n");
                getchar();
                continue;
            }

            //send filename to server
            bzero(send_frame.buf, BUFSIZE);
            strcpy(send_frame.buf, filename);
            ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
            if (ackd == 1) {
                printf("Command was sent unsuccessfully. Press ENTER to continue\n");
                getchar();
                continue;
            }

            //receive if file exists from server
            bzero(recv_frame.buf, BUFSIZE);
            int recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
            if (recvd == 1) {
                printf("Packet not received. Press ENTER to continue\n");
                getchar();
                continue;
            }

            if (strcmp(recv_frame.buf, "EXISTS")==0){
                //send OK
                char * ok = "OK";
                bzero(send_frame.buf, BUFSIZE);
                strcpy(send_frame.buf, ok);
                ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
                if (ackd == 1) {
                    printf("Command was sent unsuccessfully. Press ENTER to continue\n");
                    getchar();
                    continue;
                }
            }

            else{
                //file doesn't exist
                printf("%s was not found on server. Press ENTER to continue\n", filename);
                getchar();
                continue;
            }

            //receive file size
            bzero(recv_frame.buf, BUFSIZE);
            recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
            if (recvd == 1) {
                printf("Packet not received. Press ENTER to continue\n");
                getchar();
                continue;
            }
            //process filesize
            int filesize = atoi(recv_frame.buf);
            //create file in write mode
            FILE * writefile = fopen(filename, "w");

            //receive file and write to it
            int bytes_written;
            while (filesize > 0) {
                bzero(recv_frame.buf, BUFSIZE);
                recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
                if (recvd == 1) {
                    printf("Packet not received. Press ENTER to continue\n");
                    getchar();
                    continue;
                }
                bytes_written = fwrite(recv_frame.buf, sizeof(char), BUFSIZE, writefile);
                filesize -= bytes_written;
            }
            fclose(writefile);
            printf("RECEIVED FILE: %s\n", filename);
        }

        /*delete command*/
        else if (strcmp(instr, "delete") == 0){
            //send instruction to server
            strcpy(send_frame.buf, instr);
            int ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
            if (ackd == 1) {
                printf("Command was sent unsuccessfully. Press ENTER to continue\n");
                getchar();
                continue;
            }

            //send filename to server
            filename = strtok(NULL, " ");  //extract filename
            bzero(send_frame.buf, BUFSIZE);
            strcpy(send_frame.buf, filename);
            ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
            if (ackd == 1) {
                printf("Command was sent unsuccessfully. Press ENTER to continue\n");
                getchar();
                continue;
            }

            //receive if file exists from server
            bzero(recv_frame.buf, BUFSIZE);
            int recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
            if (recvd == 1) {
                printf("Packet not received. Press ENTER to continue\n");
                getchar();
                continue;
            }

            if (strcmp(recv_frame.buf, "EXISTS")==0){
                //send OK
                char * ok = "OK";
                bzero(send_frame.buf, BUFSIZE);
                strcpy(send_frame.buf, ok);
                ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
                if (ackd == 1) {
                    printf("Command was sent unsuccessfully. Press ENTER to continue\n");
                    getchar();
                    continue;
                }
            }

            else{
                //file doesn't exist
                printf("%s was not found on server. Press ENTER to continue\n", filename);
                getchar();
                continue;
            }

            //receive if file successfully deleted
            recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
            if (recvd == 1) {
                printf("Packet not received. Press ENTER to continue\n");
                getchar();
                continue;
            }
            if (strcmp(recv_frame.buf, "DELETED") == 0){
                printf("FILE SUCCESSFULLY DELETED\n");
            }
            else
                printf("FILE COULD NOT BE DELETED\n");

        }

        /*ls command*/
        else if (strcmp(instr, "ls") ==0 ){
            //send instruction to server
            strcpy(send_frame.buf, instr);
            int ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
            if (ackd == 1) {
                printf("Command was sent unsuccessfully. Press ENTER to continue\n");
                getchar();
                continue;
            }

            //receive file name from server
            while(1) {
                bzero(recv_frame.buf, BUFSIZE);
                int recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &serveraddr);
                if (recvd == 1) {
                    printf("Packet not received. Press ENTER to continue\n");
                    getchar();
                    continue;
                }
                if (strcmp(recv_frame.buf, "LAST") == 0)
                    break;

                printf("%s\n", recv_frame.buf);
            }

        }

        /*case for invalid command*/
        else{
            printf("Invalid command. Press ENTER to continue\n");
            getchar();
        }
    }

    /* send the message to the server */
    serverlen = sizeof(serveraddr);
    n = sendto(sockfd, buf, strlen(buf), 0, (const struct sockaddr *)&serveraddr, serverlen);
    if (n < 0) 
      error("ERROR in sendto");
    
    /* print the server's reply */
    n = recvfrom(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&serveraddr, &serverlen);
    if (n < 0) 
      error("ERROR in recvfrom");
    printf("Echo from server: %s", buf);
    return 0;
}

/*function to send and wait for ack*/
int send_wait_ack(struct frame * sendframe, struct frame * recvframe, int fd, struct sockaddr_in * sa){
    sendframe->frame_type = DATA;
    sendframe->seq_no = last_sent_seq;
    while (1) {
        //send packet
        ssize_t n = sendto(fd, sendframe, sizeof(struct frame), 0, (const struct sockaddr *) sa, sizeof(*sa));
        if (n < 0)
            return 1;
        /*setup select function*/
        fd_set select_fds;
        struct timeval timeout;

        FD_ZERO(&select_fds);
        FD_SET(fd, &select_fds);
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        //wait for ack otherwise timeout
        if (select(32, &select_fds, 0, 0, &timeout) == 0){
            printf("Timeout on receiving ACK\n");
            continue;
        }
        //get ack
        socklen_t length = sizeof(*sa);
        ssize_t recv_size = recvfrom(fd, recvframe, sizeof(struct frame), 0, (struct sockaddr *) sa,&length);
        if (recv_size > 0 && recvframe->seq_no == sendframe->seq_no && recvframe->frame_type == ACK)
        {
            last_sent_seq ^= 1;
            break;
        }
    }

    return 0;
}

/*function to receive packet and send ack*/
int recv_send_ack(struct frame * sendframe, struct frame * recvframe, int fd, struct sockaddr_in * sa){
    while(1) {
        //receive packet
        bzero(recvframe->buf, BUFSIZE);
        socklen_t length = sizeof(*sa);
        ssize_t recv_size = recvfrom(fd, recvframe, sizeof(struct frame), 0, (struct sockaddr *) sa, &length);

        if (recv_size < 0)
            return 1;

        //send ACK
        sendframe->frame_type = ACK;
        sendframe->seq_no = recvframe->seq_no;
        ssize_t n = sendto(fd, sendframe, sizeof(struct frame), 0, (const struct sockaddr *) sa, sizeof(*sa));
//        printf("SENT ACK %d\n", sendframe->seq_no);
        if (n < 0)
            return 1;

        if (recvframe->seq_no != last_recv_seq){
            last_recv_seq = recvframe->seq_no;
            break;
        }
    }


    return 0;
}