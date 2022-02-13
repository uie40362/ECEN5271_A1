/*
* udpserver.c - A simple UDP echo server
* usage: udpserver <port>
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <dirent.h>

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
    exit(1);
}

/*function prototypes*/
int recv_send_ack(struct frame * sendframe, struct frame * recvframe, int fd, struct sockaddr_in * ca);
int send_wait_ack(struct frame * sendframe, struct frame * recvframe, int fd, struct sockaddr_in * ca);

/*globals*/
int last_recv_seq = 1;
int last_sent_seq = 0;

int main(int argc, char **argv) {
    int sockfd; /* socket */
    int portno; /* port to listen on */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char buf[BUFSIZE]; /* message buf */
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    ssize_t n; /* message byte size */


    //set stdout buffer size to 0, for debugging purpose...CLion not flushing stdout buffer...
    setbuf(stdout, 0);

    /*
    * check command line arguments
    */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);

    /*
    * socket: create the parent socket
    */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    error("ERROR opening socket");

    /* setsockopt: Handy debugging trick that lets
    * us rerun the server immediately after we kill it;
    * otherwise we have to wait about 20 secs.
    * Eliminates "ERROR on binding: Address already in use" error.
    */
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
         (const void *)&optval , sizeof(int));

    /*
    * build the server's Internet address
    */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);

    /*
    * bind: associate the parent socket with a port
    */
    if (bind(sockfd, (struct sockaddr *) &serveraddr,
       sizeof(serveraddr)) < 0)
    error("ERROR on binding");

    /*
    * main loop
    */

    while (1) {
        char instr[10];
        char * filename;
        bzero(instr, 10);
        struct frame send_frame;
        struct frame recv_frame;

        /*we first receive an instruction, i.e. get, put, etc.*/
        bzero(recv_frame.buf, BUFSIZE);
        int recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
        if (recvd == 1){
            printf("Packet not received. Press ENTER to continue\n");
            getchar();
            continue;
        }

        /*process instruction from buffer*/
        strncpy(instr, recv_frame.buf, strlen(recv_frame.buf));

        /*put instruction*/
        if (strcmp(instr, "put")==0){

            //receive filename from put instruction
            bzero(recv_frame.buf, BUFSIZE);
            recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
            if (recvd == 1){
                printf("Packet not received. Press ENTER to continue\n");
                getchar();
                continue;
            }

            //process filename
            filename = (char *) calloc(strlen(recv_frame.buf), sizeof(char));
            strncpy(filename, recv_frame.buf, strlen(recv_frame.buf));
            printf("RECEIVED COMMAND: %s %s\n", instr, filename);

            //create file in write mode
            FILE * filetosave = fopen(filename, "w");
            //receive file size
            bzero(recv_frame.buf, BUFSIZE);
            recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
            if (recvd == 1){
                printf("Packet not received. Press ENTER to continue\n");
                getchar();
                continue;
            }

            //process filesize
            int filesize = atoi(recv_frame.buf);

            //receive file data and write to file
            while (filesize>0) {
                bzero(recv_frame.buf, BUFSIZE);
                recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
                if (recvd == 1) {
                    printf("Packet not received. Press ENTER to continue\n");
                    getchar();
                    continue;
                }

                int bytes_written = fwrite(recv_frame.buf, sizeof(char), BUFSIZE, filetosave);
                filesize -= bytes_written;
            }
            fclose(filetosave);
            printf("RECEIVED FILE: %s\n", filename);
         }

        /*get instruction*/
        else if (strcmp(instr, "get") == 0){
            //receive filename
            bzero(recv_frame.buf, BUFSIZE);
            recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
            if (recvd == 1) {
                printf("Packet not received. Press ENTER to continue\n");
                getchar();
                continue;
            }
            //process filename
            filename = (char *) calloc(strlen(recv_frame.buf), sizeof(char));
            strncpy(filename, recv_frame.buf, strlen(recv_frame.buf));
            printf("RECEIVED COMMAND: %s %s\n", instr, filename);

            //determine if file exists
            if( access( filename, F_OK ) == 0 ) {
                //let client know file exists
                char * exist = "EXISTS";
                bzero(send_frame.buf, BUFSIZE);
                strcpy(send_frame.buf, exist);
                int ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
                if (ackd == 1) {
                    printf("Packet not sent. Press ENTER to continue\n");
                    getchar();
                    continue;
                }
            }
            else {
                // let client know file doesn't exist
                char * noexist = "NOEXIST";
                bzero(send_frame.buf, BUFSIZE);
                strcpy(send_frame.buf, noexist);
                int ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
                if (ackd == 1) {
                    printf("Packet not sent. Press ENTER to continue\n");
                    getchar();
                    continue;
                }
                printf("FILE: %s is not present in system\n", filename);
                continue;
            }

            //receive OK from client
            bzero(recv_frame.buf, BUFSIZE);
            recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
            if (recvd == 1) {
                printf("Packet not received. Press ENTER to continue\n");
                getchar();
                continue;
            }
            //process OK
            if (strcmp(recv_frame.buf, "OK")){
                printf("Client not ready to receive file\n");
                getchar();
                continue;
            }

            //send filesize
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
            int ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &clientaddr);

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
                ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &clientaddr);

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

        /*delete command*/
        else if(strcmp(instr, "delete") == 0){
            //receive filename
            bzero(recv_frame.buf, BUFSIZE);
            recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
            if (recvd == 1) {
                printf("Packet not received. Press ENTER to continue\n");
                getchar();
                continue;
            }

            //process filename
            filename = (char *) calloc(strlen(recv_frame.buf), sizeof(char));
            strncpy(filename, recv_frame.buf, strlen(recv_frame.buf));
            printf("RECEIVED COMMAND: %s %s\n", instr, filename);

            //determine if file exists
            if( access( filename, F_OK ) == 0 ) {
                //let client know file exists
                char * exist = "EXISTS";
                bzero(send_frame.buf, BUFSIZE);
                strcpy(send_frame.buf, exist);
                int ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
                if (ackd == 1) {
                    printf("Packet not sent. Press ENTER to continue\n");
                    getchar();
                    continue;
                }
            }
            else {
                // let client know file doesn't exist
                char * exist = "NOEXIST";
                bzero(send_frame.buf, BUFSIZE);
                strcpy(send_frame.buf, exist);
                int ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
                if (ackd == 1) {
                    printf("Packet not sent. Press ENTER to continue\n");
                    getchar();
                    continue;
                }
                printf("FILE: %s is not present in system\n", filename);
                continue;
            }

            //receive OK from client
            bzero(recv_frame.buf, BUFSIZE);
            recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
            if (recvd == 1) {
                printf("Packet not received. Press ENTER to continue\n");
                getchar();
                continue;
            }
            //process OK
            if (strcmp(recv_frame.buf, "OK")){
                printf("No OK received from client. Press ENTER to continue.\n");
                getchar();
                continue;
            }

            //delete file
            if (remove(filename) == 0){
                printf("FILE DELETED SUCCESSFULLY\n");
                //let client know file was deleted
                bzero(send_frame.buf, BUFSIZE);
                char * deleted = "DELETED";
                strcpy(send_frame.buf, deleted);
                int ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
                if (ackd == 1) {
                    printf("Packet not sent. Press ENTER to continue\n");
                    getchar();
                    continue;
                }
            }

            else{
                printf("FILE DELETED UNSUCCESSFULLY\n");
                //let client know file was not deleted
                char * deleted = "NODELETED";
                strcpy(send_frame.buf, deleted);
                int ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
                if (ackd == 1) {
                    printf("Packet not sent. Press ENTER to continue\n");
                    getchar();
                    continue;
                }
            }
        }

        /*ls command*/
        else if (strcmp(instr, "ls") == 0){
            printf("RECEIVED COMMAND: %s\n", instr);
            //send file names to client
            DIR *d;
            struct dirent *dir;
            d = opendir(".");
            if (d)
            {
                while ((dir = readdir(d)) != NULL)
                {
                    printf("%s\n", dir->d_name);
                    bzero(send_frame.buf, BUFSIZE);
                    strcpy(send_frame.buf, dir->d_name);
                    int ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
                    if (ackd == 1) {
                        printf("Packet not sent. Press ENTER to continue\n");
                        getchar();
                        continue;
                    }
                }
                //let client know once done going through files
                bzero(send_frame.buf, BUFSIZE);
                char * last = "LAST";
                strcpy(send_frame.buf, last);
                int ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
                if (ackd == 1) {
                    printf("Packet not sent. Press ENTER to continue\n");
                    getchar();
                    continue;
                }
                closedir(d);
            }
        }
        /*exit*/
        if (strcmp(instr, "exit") == 0){
            //send SHUTDOWN message
            bzero(send_frame.buf, BUFSIZE);
            char * shtdwn = "SHUTDOWN";
            strcpy(send_frame.buf, shtdwn);
            int ackd = send_wait_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
            if (ackd == 1) {
                printf("Packet not sent. Press ENTER to continue\n");
                getchar();
                continue;
            }
            //shutdown the server
            int shut = close(sockfd);
            if (shut == 0){
                printf("SERVER SHUTDOWN. EXITING PROGRAM\n");
                exit(0);
            }
            else{
                error("SOMETHING WENT WRONG WHILE EXITING\n");
                printf("errno %d\n", errno);
                perror("");
            }
        }
    }
}

/*funtion to receive packet and reply with ACK*/

int recv_send_ack(struct frame * sendframe, struct frame * recvframe, int fd, struct sockaddr_in * ca){
    while(1) {
        //receive packet
        bzero(recvframe->buf, BUFSIZE);
        socklen_t length = sizeof(*ca);
        ssize_t recv_size = recvfrom(fd, recvframe, sizeof(struct frame), 0, (struct sockaddr *) ca, &length);

        if (recv_size < 0)
            return 1;

        //send ACK
        sendframe->frame_type = ACK;
        sendframe->seq_no = recvframe->seq_no;
        ssize_t n = sendto(fd, sendframe, sizeof(struct frame), 0, (const struct sockaddr *) ca, sizeof(*ca));
//        printf("SENT ACK %d\n", sendframe->seq_no);
        if (n < 0)
            return 1;

        if (recvframe->seq_no != last_recv_seq) {
            last_recv_seq = recvframe->seq_no;
            break;
        }
    }
    return 0;
}

/*function to send packet and wait for ack*/
int send_wait_ack(struct frame * sendframe, struct frame * recvframe, int fd, struct sockaddr_in * ca){
    sendframe->frame_type = DATA;
    sendframe->seq_no = last_sent_seq;
    while (1) {
        //send packet
        ssize_t n = sendto(fd, sendframe, sizeof(struct frame), 0, (const struct sockaddr *) ca, sizeof(*ca));
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
        socklen_t length = sizeof(*ca);
        ssize_t recv_size = recvfrom(fd, recvframe, sizeof(struct frame), 0, (struct sockaddr *) ca,&length);
        if (recv_size > 0 && recvframe->seq_no == sendframe->seq_no && recvframe->frame_type == ACK)
        {
            last_sent_seq ^= 1;
            break;
        }
    }

    return 0;
}