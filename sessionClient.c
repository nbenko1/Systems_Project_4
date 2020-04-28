#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>

void error(char *msg){
  perror(msg);
  exit(0);
}
int main(int argc, char *argv[]){
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char buffer[256];
  if(argc < 3){
    fprintf(stderr,"usage %s hostname port \n", argv[0]);
    exit(0);
  }
  portno = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
    error("ERROR opening socket");
  server = gethostbyname(argv[1]);
  if(server == NULL){
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
       (char *) &serv_addr.sin_addr.s_addr,
       server->h_length);
  serv_addr.sin_port = htons(portno);
  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR connecting");

  char id[12];
  printf("Server session started\n");

  bzero(buffer,256); // clearing buffer
  read(sockfd, buffer, 62); //gets server instructions
  printf("%s\n", buffer); //prints instructions

  bzero(buffer,256); // clearing buffer
  read(sockfd, buffer, sizeof(buffer)); //gets pid of server child
  strcpy(id, buffer); //saves copy
  bzero(buffer,256); //clear buffer


  while(strcmp(buffer, "kill\n") != 0 && strcmp(buffer, "killserver\n") != 0) {
    printf("%s $  ", id); //prompt user for message
    bzero(buffer,256); //clears
    fgets(buffer,255,stdin); //reads input
    n = write(sockfd, buffer, strlen(buffer)); //adds input to buffer
    if(strcmp(buffer, "kill\n") == 0) return 0;
    if(strcmp(buffer, "kill\n") != 0 && strcmp(buffer, "killserver\n") != 0) {
      if(n < 0) error("ERROR writing to socket");
      bzero(buffer,256); //clears buffer
      n = read(sockfd, buffer, 255); //reads servers response
      if (n < 0) error ("ERROR reading from socket");
    }

    printf("%s\n", buffer); //prints response
  }
  return 0;

}
