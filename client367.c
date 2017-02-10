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
#include <ctype.h>

#include <arpa/inet.h>

#define PORT "3527" // the port client will be connecting to

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
    char cmd = 't';          //user command input storage

    //  while(cmd != 'q'){/////////////////////////while loop//////////////////
    //    printf("whats your input?\n");
    //    scanf("%c",cmd);
    //  printf("%c\n",cmd);

    if (argc != 2) {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    while (cmd != 'q') {
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

        //  while(cmd != 'q'){ //////////////////////////////////while loop/////                   /////////////
        printf("............Welcome to the MC Command Hub.............\n");
        printf("Please input your command........'input: h for help'..\n");
        scanf("%c", &cmd);
        while (getchar() != '\n') continue;
        if (cmd == 'h') {
            printf("Please make your selection from the following commands\n");
            printf("'l' = List contents of server.....ex(l)...............\n");
            printf("'c' = Check for a file name.......ex(c <filename>)....\n");
            printf("'d' = Download file name..........ex(d <filename>)....\n");
            printf("'p' = Display file name...........ex(p <filename>)....\n");
            printf("'q' = Quit............................................\n");
        }
        ///////////////check for commands//////////////////////////
        if (cmd == 'l') {

            send(sockfd, "list", 4, 0);
            //{//
            //perror("send");
            //close(sockfd);
            //exit(0);
            //}
            numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0);
            buf[numbytes] = '\0';
            printf("%s\n", buf);
        }
        if (cmd == 'c') {
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

        if (cmd == 'p') // display function
        {


            send(sockfd, "disp", 50, 0); //send server cmd input
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
              /*  printf("Save as: "); //client side filename
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

*/
            }
           // close(sockfd);
        }

	if(cmd == 'd'){
		send(sockfd, "disp", 50, 0); //send server cmd input
		printf("Enter File Name: \n"); //client side file name
		scanf("%s", &filename);
		int filesize;
		char sizeofFile[20] = {0};
		send(sockfd, filename, 100, 0); //receive server file string
		numbytes = recv(sockfd, sizeofFile, 20, 0); //Receive back the file size from server
		filesize = (int) strtol(sizeofFile, (char **) NULL, 10); //change chars the sever sent to int
		printf("File Size: %d \n", filesize);
		if(filesize == 0){
			printf("File Not Found");
			continue;
		}
		else {
			char *filebuff = calloc(filesize+1, sizeof(char)); //Allocate char array of file size
			sleep(1);
			numbytes = recv(sockfd, filebuff, filesize, 0); //Receive file to allocated array
			printf("File Downloaded from Server\n");
			printf("Save as: "); //client side filename
			scanf("%s", &filename);

			//download file from server
			FILE *fp;
			fp = fopen(filename, "r");
			if(fp != NULL){
				char choice[5];
				printf("File %s exists. Do you want to override? (y/n): ", filename);
				scanf("%4s", &choice);
				if(tolower(choice[0]) == 'n'){
					fclose(fp);
					free(filebuff);
					continue;
				}
			}

			//Write file
			fp = fopen(filename, "w");
			if(NULL == fp){
				printf("Error Opening File");
				return 1;
			}
			fwrite(filebuff, 1, filesize, fp);
			fclose(fp);
			free(filebuff);
		}
		//close(sockfd);
	}

        if (cmd == 'q') {
            break;
        }
    }////////while loop///////////////////////////
    close(sockfd);

    return 0;
}
