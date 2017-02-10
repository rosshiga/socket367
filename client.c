/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3510" // the port client will be connecting to

#define MAXDATASIZE 100 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    char commandLetter = 't';          //user command input storage

    //  while(commandLetter != 'q'){/////////////////////////while loop//////////////////
    //    printf("whats your input?\n");
    //    scanf("%c",commandLetter);
    //  printf("%c\n",commandLetter);

    if (argc != 2) {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    while (commandLetter != 'q') {
        if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }

        // loop through all the results and connect to the first we can
        for (p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype,
                                 p->ai_protocol)) == -1) {
                perror("client: socket");
                continue;
            }

            if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(sockfd);
                perror("client: connect");
                continue;
            }

            break;
        }

        if (p == NULL) {
            fprintf(stderr, "client: failed to connect\n");
            return 2;
        }

        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr),
                  s, sizeof s);
        printf("client: connecting to %s\n", s);

        char filename[MAXDATASIZE];
        freeaddrinfo(servinfo); // all done with this structure

        // while(commandLetter != 'q'){ //////////////////////////////////while loop/////                   /////////////
        printf("............Welcome to the MC Command Hub.............\n");
        printf("Please input your command (h for help): ");
        char commandString[1000]; //Parsing as char can result in unexpected behavior, read whole lines
        scanf("%999s", commandString);
        commandLetter = tolower(commandString[0]);
        while (getchar() != '\n') continue;
        if (commandLetter == 'h') {
            printf("Please make your selection from the following commands\n");
            printf("'l' = List contents of server.....ex(l)...............\n");
            printf("'c' = Check for a file name.......ex(c <filename>)....\n");
            printf("'d' = Download file name..........ex(d <filename>)....\n");
            printf("'p' = Display file name...........ex(p <filename>)....\n");
            printf("'q' = Quit............................................\n");
        }
        if (commandLetter == 'l') {
            char bigBuffer[10000]; // Allocate buffer
            int bigCount = 0; // Counter for bigbuffer
            send(sockfd, "list", 4, 0);
            do {
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
                    perror("recv");
                    exit(1);
                }
                bigCount += numbytes; // Add bytes recieved to count
                strcat(bigBuffer, buf); // Dump buf into bigBuffer
            } while (numbytes == MAXDATASIZE - 1); // If we didnt get max packet size, assume there is more data
            bigBuffer[bigCount] = '\0'; // Null terminate string
            printf("%s\n", bigBuffer);
        }
        if (commandLetter == 'c') {
            char ans[3];
            int numb;
            send(sockfd, "chek", 4, 0);
            printf("Which file do you want to check for?:\n");
            scanf("%s", &filename);
            while (getchar() != '\n') continue;

            //Sending filename to server
            send(sockfd, filename, MAXDATASIZE, 0);
            numb = recv(sockfd, buf, MAXDATASIZE - 1, 0);
            buf[numb] = '\0';

            if (!strcmp(buf, "yes"))
                printf("File was found\n");
            else
                printf("File was not found\n");


        }

        if (commandLetter == 'd') // download function
        {

            send(sockfd, "disp", 50, 0); //send server commandLetter input
            printf("enter file name: \n"); //client side file name
            scanf("%s", &filename);
            int filesize;
            char sizeofFile[20] = {0};
            send(sockfd, filename, 100, 0); //receive server file string
            numbytes = recv(sockfd, sizeofFile, 20, 0); // Receive back the file size from server
            filesize = (int) strtol(sizeofFile, (char **) NULL, 10); //Change the  chars the server sent us back to int
            printf("Size file: %d \n", filesize);
            if (filesize == 0) {
                printf("File not found"); // 0 indicates file not found
                continue;
            } else {
                char *filebuff = calloc(filesize + 1, sizeof(char)); //Allocate char rray of file size
                sleep(1);
                numbytes = recv(sockfd, filebuff, filesize, 0); // Receive file  to allocated array
                // printf("contents : \n%s", filebuff); // debug
                printf("Save as: "); //client side filename
                scanf("%s", &filename);
                //download file from server
                FILE *fp;
                fp = fopen(filename, "r");
                if (fp != NULL) {
                    char choice[5];
                    printf("File %s exist. Do you want to override? (y/n): ", filename); //client side filename
                    scanf("%4s", &choice);
                    if (tolower(choice[0]) == 'n') {
                        fclose(fp); // Close file
                        free(filebuff); // Free allocated buffer
                        continue;
                    }
                }

                // fp is null, write file
                fp = fopen(filename, "w");
                if (NULL == fp) {
                    printf("error opening file");
                    return 1;
                }
                fwrite(filebuff, 1, filesize, fp); // Write filesize # of bytes to fp
                fclose(fp); // Close file
                free(filebuff); // Free allocated buffer


            }
        }
        if (commandLetter == 'p') // display function
        {

            send(sockfd, "disp", 50, 0); //send server commandLetter input
            printf("enter file name: \n"); //client side file name
            scanf("%s", &filename);
            int filesize;
            char sizeofFile[20] = {0};
            send(sockfd, filename, 100, 0); //receive server file string
            numbytes = recv(sockfd, sizeofFile, 20, 0); // Receive back the file size from server
            filesize = (int) strtol(sizeofFile, (char **) NULL, 10); //Change the  chars the server sent us back to int
            printf("Size file: %d \n", filesize);
            if (filesize == 0) {
                printf("File not found"); // 0 indicates file not found
                continue;
            } else {
                char *filebuff = calloc(filesize + 1, sizeof(char)); //Allocate char rray of file size
                sleep(1);
                numbytes = recv(sockfd, filebuff, filesize, 0); // Receive file  to allocated array
                printf("contents : \n%s", filebuff); // debug

                free(filebuff); // Free allocated buffer


            }
        }
        if (commandLetter == 'q') {
            break;
        }
        close(sockfd);
        sleep(1); // Let server reset to listen before reconnection
    }////////while loop///////////////////////////


    return 0;
}
