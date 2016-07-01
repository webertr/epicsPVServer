#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <myfunctions.h>

#define MYPORT "3490" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold


void listenTest(void) {

  /*
  struct addrinfo hints, *res;
  int sockfd;

  // first, load up address structs with getaddrinfo():
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // fill in my IP for me
  getaddrinfo(NULL, "3490", &hints, &res);

  // make a socket:
  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  // bind it to the port we passed in to getaddrinfo():
  bind(sockfd, res->ai_addr, res->ai_addrlen);


  // int listen(int sockfd, int backlog);

  int backlog = 5;

  int ret = listen(sockfd, backlog);
  */

  char htmlMessage[1000];
  getHTMLFile(htmlMessage);

  char *pv_Read[] = {"fred1", "fred2", "fred3"};
  char *read_Buffer[3];

  cagetFuZE(pv_Read, 3, read_Buffer);  

  swapCharArray(htmlMessage, "[1##]", pv_Read[0]);
  swapCharArray(htmlMessage, "[2##]", pv_Read[1]);
  swapCharArray(htmlMessage, "[3##]", pv_Read[2]);


  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  struct addrinfo hints, *res;
  int sockfd, new_fd;
 
  // !! don't forget your error checking for these calls !!
  // first, load up address structs with getaddrinfo():
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // fill in my IP for me
  getaddrinfo(NULL, MYPORT, &hints, &res);

  // make a socket, bind it, and listen on it:
  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  bind(sockfd, res->ai_addr, res->ai_addrlen);
  listen(sockfd, BACKLOG);

  int len, bytes_sent;
  
  while (1) {
    // now accept an incoming connection:
    addr_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

    len = strlen(htmlMessage);
    bytes_sent = send(new_fd, htmlMessage, len, 0);

    printf("\n %d Bytes Sent\n", bytes_sent); 
  
    // ready to communicate on socket descriptor new_fd!
  } 
}
