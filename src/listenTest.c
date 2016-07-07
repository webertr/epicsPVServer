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


  char htmlMessage[1000];
  char s[INET6_ADDRSTRLEN];
  char *pv_Read = "fred1";
  char pvValue[10];
  struct sockaddr_storage their_addr;
  socklen_t addr_size;
  struct addrinfo hints, *res;
  int sockfd, new_fd;
  int len, bytes_sent;
  pid_t pid, retpid; // process id for the child and return for waitpid

  
  // get HTML string in htmlMessage to send to client
  getHTMLFile(htmlMessage);

  // calling caget to get a pv value returned as a string in pvValue
  cagetFuZE(pv_Read, pvValue);

  // inserting PV value into the html message in place
  swapCharArray(htmlMessage, "[1##]", pvValue);

 
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

  while (1) {


    pid = fork();
    if ( pid == 0 ) { // this is the child process

      // wait for a request
      listen(sockfd, BACKLOG);

      // ???
      new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

      // get IP Address code
      inet_ntop(their_addr.ss_family,
	      get_in_addr((struct sockaddr *)&their_addr),
	      s, sizeof s);
      printf("server: got connection from %s\n", s);

      // send HTML Message
      len = strlen(htmlMessage);
      bytes_sent = send(new_fd, htmlMessage, len, 0);
      printf("\n %d Bytes Sent\n", bytes_sent);

      // Sleep. if I don't sleep before close(new_fd), message doesn't always make it
      sleep(1);
      close(new_fd);
      exit(0);
    }

    // Reaping the child process with process ID, pid.
    retpid = waitpid(pid, NULL, 0);
    
  } 
}
