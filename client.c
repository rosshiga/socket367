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

#define PORT "3521" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
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

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);
	
    char filename[MAXDATASIZE];
    freeaddrinfo(servinfo); // all done with this structure

    //  while(cmd != 'q'){ //////////////////////////////////while loop//////////////////
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
    if (cmd == 'l')
    {

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
    if (cmd == 'c')
    {
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
      char fileName[50];


      send(sockfd, "disp", 50, 0); //send server cmd input
      printf("enter file name: \n"); //client side file name
      scanf("%s", &fileName);
      while (1)
      {
        send(sockfd, fileName, 100, 0); //receieve server file string
        numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0);
        buf[numbytes] = '\0';
        printf("%s\n", buf);
      }

      printf("Displaying File: %s\n", fileName); //client side filename
      //printf("%s\n",buf); //print server sent file contents

	//download file from server
	FILE *fp;
	fp = fopen("file1.txt","w");
	if(NULL==fp){
	printf("error opening file");
	return 1;
	}

	//recieve data in 100 btyes
	while(numbytes = recv(sockfd, buf, MAXDATASIZE-1,0) > 0)
	{
	printf("got %d bytes\n",numbytes);
	fwrite(MAXDATASIZE-1, 1, numbytes, fp);
	}
	


      close(sockfd);
    }
  }////////while loop///////////////////////////
  close(sockfd);

  return 0;
}



