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
  if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){ //maybe
          error("ERROR on binding");
  }
  listen(sockfd,5);
  clilen = sizeof(cli_addr);

  while(true){
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(newsockfd < 0){
      error("ERROR on accept");
    }

    int pid = fork();

    if(pid == 0) {
        n = write(newsockfd, "Use \"kill\" to exit session, \"killserver\" to kill server", 56);
        n = write(newsockfd, pid, sizeof(pid));

        while(strcmp(buffer, "kill\n") != 0 && strcmp(buffer, "killserver\n") != 0 ){
            bzero(buffer, 256);
            n = read(newsockfd,buffer,255);
            if(n < 0) error("ERROR reading from socket");
            printf("Here is the message: %s\n", buffer);
            n = write(newsockfd, buffer, sizeof(buffer));
            if(n < 0) error("ERROR writing to socket");
        }
    }

    if(strcmp(buffer, "kill\n") == 0) {
        // look for using getpid

    }
    if(strcmp(buffer, "killserver\n") == 0) {
        // look at least for getppid
    }

    

  }
  printf("exiting server\n");
  return 0;
}
