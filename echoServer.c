/*
Systems Project 4: server-client
4/29/20
Anna Krolokowski
Nicholai Benko
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

void error(char *msg){
  perror(msg);
  exit(1);
}
int main(int argc, char *argv[]){
  int sockfd, newsockfd, portno, clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  if(argc<2){
    fprintf(stderr,"ERROR, no port provided\n");
    exit(1);
  }
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0){
    error("ERROR, opening socket");
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portno);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
          error("ERROR on binding");
  }
  while(strcmp(buffer, "kill\n") != 0 && strcmp(buffer, "killserver\n") != 0 ){ //loops until told to quit
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); //listens for client
    if(newsockfd < 0){
      error("ERROR on accept");
    }

    bzero(buffer, 256); //clear buffer
    n = read(newsockfd,buffer,255); //reads from buffer
    if(n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n", buffer); //prints the message from the client
    n = write(newsockfd, "I got your message: ", 20); //responds to client
    n = write(newsockfd, buffer, 255);
    if(n < 0) error("ERROR writing to socket");

  }
  printf("exiting server\n");
  return 0;
}
