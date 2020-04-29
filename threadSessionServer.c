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
#include <sys/unistd.h>
#include <signal.h>
#include <pthread.h>

int loop = 0; //if main process should keep looking for clients
int numThreads = 0; //number of active threads

void error(char *msg){
  perror(msg);
  exit(1);
}

//this method handles each client
void * newSession(void * sid) {
  int newsockfd = (intptr_t) sid;
  int n;
  char buffer[256];
  long tid = pthread_self(); //current process id
  numThreads++; //new thread is active

  printf("\nnew thread with ID: %d\n", tid);
  n = write(newsockfd, "Use \"kill\" to exit session, \"killserver\" to kill server", 62);
  if(n < 0) error("ERROR writing to socket");

  bzero(buffer,256);
  char strpid[sizeof(tid)];
  sprintf(strpid, "%d", tid); //change tip to string
  n = write(newsockfd, strpid, sizeof(tid)); //add thread id to buffer
  if(n < 0) error("ERROR writing to socket");

  //main loop that reads input
  while(strcmp(buffer, "kill\n") != 0 && strcmp(buffer, "killserver\n") != 0 ){
      bzero(buffer, 256);
      n = read(newsockfd,buffer,255); //reads buffer
      if(strcmp(buffer, "kill\n") != 0 && strcmp(buffer, "killserver\n") != 0) {
        if(n < 0) error("ERROR reading from socket");
        printf("Here is the message: %s\n", buffer);
        n = write(newsockfd, buffer, strlen(buffer)); //write to buffer
        if(n < 0) error("ERROR writing to socket");
      }
  }
  printf("exiting thread ID: %s\n", strpid);
  numThreads--; //kills one thread

  if(strcmp(buffer, "killserver\n") == 0) {
      printf("killing server\n");
      loop = 1; //ends parent loop
  }

  if(numThreads == 0 && loop == 1) {
      kill(getpid(), SIGTERM); // ends process`
  }
  return 0;
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
  pthread_t newThread; //declare thread
  while(loop == 0){ //main process loops until a thread tells it to exit
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); //listen for client connection
    if(newsockfd < 0){
      error("ERROR on accept");
    }
    pthread_create(&newThread, NULL, &newSession, (void*) (intptr_t) newsockfd); // create thread
  }
  return 0;
}
