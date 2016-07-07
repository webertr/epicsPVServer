#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <myfunctions.h>
#include <stdlib.h>

#define MYPORT "3490" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


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
  char s[INET6_ADDRSTRLEN];
  
  getHTMLFile(htmlMessage);

  char *pv_Read = "fred1";
  char pvValue[10];
  cagetFuZE(pv_Read, pvValue);

  printf(pvValue);
  swapCharArray(htmlMessage, "[1##]", pvValue);
  //swapCharArray(htmlMessage, "[2##]", pv_Read[1]);
  //swapCharArray(htmlMessage, "[3##]", pv_Read[2]);


  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  struct addrinfo hints, *res;
  int sockfd, new_fd;
  int len, bytes_sent;

 
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

  addr_size = sizeof their_addr;

  pid_t pid, retpid;
  while (1) {


    listen(sockfd, BACKLOG);
    
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

    inet_ntop(their_addr.ss_family,
	      get_in_addr((struct sockaddr *)&their_addr),
	      s, sizeof s);
    printf("server: got connection from %s\n", s);

    pid = fork();
    if ( pid == 0 ) { // this is the child process
      
      len = strlen(htmlMessage);
      bytes_sent = send(new_fd, htmlMessage, len, 0);
      printf("\n %d Bytes Sent\n", bytes_sent);
      sleep(1);
      close(new_fd);
      exit(0);
    }
    close(new_fd);  // parent doesn't need this
    // now accept an incoming connection:
    retpid = waitpid(pid, NULL, 0);
    //while(waitpid(-1, NULL, WNOHANG) > 0);
    
    
    //listen(sockfd, BACKLOG);
    //new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);


    // ready to communicate on socket descriptor new_fd!
  } 
}
