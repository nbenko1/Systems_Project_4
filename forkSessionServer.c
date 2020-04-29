#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <signal.h>

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
  int loop = 1;
  while(loop == 1){
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(newsockfd < 0){
      error("ERROR on accept");
    }

    int pid = fork(); //creates fork

    if(pid == 0) { //if the process is a child
        n = write(newsockfd, "Use \"kill\" to exit session, \"killserver\" to kill server", 62);
        char strpid[sizeof(getpid())];
        sprintf(strpid, "%d", getpid()); // casts pid to an int
        n = write(newsockfd, strpid, sizeof(getpid())); //add pid to buffer

        //main loop that reads input
        while(strcmp(buffer, "kill\n") != 0 && strcmp(buffer, "killserver\n") != 0 ){
            bzero(buffer, 256);
            n = read(newsockfd,buffer,255);
            if(strcmp(buffer, "kill\n") != 0 && strcmp(buffer, "killserver\n") != 0) {
              if(n < 0) error("ERROR reading from socket");
              // printf("new child with ID: %s\n", strpid); // debugging
              printf("Here is the message: %s\n", buffer);
              n = write(newsockfd, buffer, strlen(buffer));
              if(n < 0) error("ERROR writing to socket");
            }
        }

        if(strcmp(buffer, "kill\n") == 0) { //kills child
            printf("killing current fork\n");
            kill(getpid(), SIGTERM);
            return 0;
        }
        if(strcmp(buffer, "killserver\n") == 0) { //kills server
            kill(getppid(), SIGTERM); //exits parents
            loop = 0; //
        }
    } else {
      signal(SIGCHLD,SIG_IGN);
    }
  }
  printf("exiting server\n");
  return 0;
}
