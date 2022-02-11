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

/*globals*/
int last_recv_seq = 1;

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
        last_recv_seq = recv_frame.seq_no;

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
            last_recv_seq = recv_frame.seq_no;

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
            last_recv_seq = recv_frame.seq_no;

            //process filesize
            int filesize = atoi(recv_frame.buf);

            //receive file data and write to file
            while (filesize) {
                bzero(recv_frame.buf, BUFSIZE);
                recvd = recv_send_ack(&send_frame, &recv_frame, sockfd, &clientaddr);
                if (recvd == 1) {
                    printf("Packet not received. Press ENTER to continue\n");
                    getchar();
                    continue;
                }
                last_recv_seq = recv_frame.seq_no;

                int bytes_written = fwrite(recv_frame.buf, sizeof(char), BUFSIZE, filetosave);
                filesize -= bytes_written;
            }
            fclose(filetosave);
            printf("RECEIVED FILE: %s\n", filename);
    }

        /*
         * gethostbyaddr: determine who sent the datagram
         */
//        hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
//                  sizeof(clientaddr.sin_addr.s_addr), AF_INET);
//        if (hostp == NULL)
//          error("ERROR on gethostbyaddr");
//        hostaddrp = inet_ntoa(clientaddr.sin_addr);
//        if (hostaddrp == NULL)
//          error("ERROR on inet_ntoa\n");
//        printf("server received datagram from %s (%s)\n",
//           hostp->h_name, hostaddrp);
//        printf("server received %lu/%d bytes: %s\n", strlen(buf), n, buf);

        /*
         * sendto: echo the input back to the client
         */
        //    n = sendto(sockfd, buf, strlen(buf), 0,
        //	       (struct sockaddr *) &clientaddr, clientlen);
        //    if (n < 0)
        //      error("ERROR in sendto");
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

        if (recvframe->seq_no != last_recv_seq)
            break;
    }


    return 0;
}