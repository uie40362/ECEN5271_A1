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


#define BUFSIZE 1024

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}
/*function prototypes*/
int send_command(char * instr, char* argu, int fd, char buf[], struct sockaddr_in sa);
int load_to_buffer(FILE * fp, char * buf, int size);

extern int errno;

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
            int sent = send_command(instr, filename, sockfd, buf, serveraddr);
            printf("SENT COMMAND: %s %s\n", instr, filename);
            if (sent == 1) {
                printf("Command was sent unsuccessfully. Press ENTER to continue\n");
                getchar();
                continue;
            }

            /*open file and determine its size*/
            FILE * fp = fopen(filename, "r");
            fseek(fp, 0, SEEK_END);
            int size = ftell(fp);   //get size of file
            fseek(fp, 0, SEEK_SET);

            /*send file size to server*/
            bzero(buf, BUFSIZE);
            char * str_size;
            sprintf(str_size, "%d", size);
            strcpy(buf, str_size);
            n = sendto(sockfd, buf, strlen(buf), 0, (const struct sockaddr *)&serveraddr, sizeof(serveraddr));
            if (n < 0) {
                printf("Data was sent unsuccessfully. Press ENTER to continue\n");
                getchar();
                continue;
            }

            /*create data buffer and load file into buffer*/
            char* databuf = (char *) calloc(size, sizeof(char));
            load_to_buffer(fp, databuf, size);

            /*send buffer to server*/
            n = sendto(sockfd, databuf, size, 0, (const struct sockaddr *)&serveraddr, sizeof(serveraddr));
            if (n < 0) {
                printf("Data was sent unsuccessfully. Press ENTER to continue\n");
                getchar();
                continue;
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

/*
function to send command and any filenames to server

args:
 instr - instruction chosen by user i.e. put, get, etc.
 argu - the argument, i.e. the filename
 fd - socket file descriptor
 buf - pointer to buffer array
 sa - server address struct

returns:
 0 - successfully sent command
 1 - unsuccessful attempt to send command
*/
int send_command(char * instr, char* argu, int fd, char buf[], struct sockaddr_in sa){
    /*sending instruction to server*/
    bzero(buf, BUFSIZE);
    strcpy(buf, instr);
    ssize_t n = sendto(fd, buf, strlen(buf), 0, (const struct sockaddr *)&sa, sizeof(sa));
    if (n < 0)
        return 1;
    /*sending filename to server*/
    if (argu != NULL) {
        /*reset buffer and copy filename into it*/
        bzero(buf, BUFSIZE);
        strcpy(buf, argu);
        n = sendto(fd, buf, strlen(buf), 0, (const struct sockaddr *) &sa, sizeof(sa));
    }
    if (n < 0)
        return 1;
    return 0;
}

/*function to load data to buffer from file
 *
 * args:
 * fp - file pointer
 * buf - buffer array
 * size - size of buffer
 *
 * returns:
 * 1 - successful load
 * 0 - unsuccessful load
 * */

int load_to_buffer(FILE * fp, char * buf, int size){
    char ch;
    bzero(buf, BUFSIZE);
    for (int i = 0; i<size; i++){
        ch = fgetc(fp);
        buf[i] = ch;
    }
    return 0;
}