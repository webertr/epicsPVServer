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
#include <errno.h>

#define MYPORT "3490" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold


static void unix_error(char *msg) {

  fprintf(stderr, "%s: %s\n", msg, strerror(errno));
  exit(0);

}


// A wrapper for the fork function
static pid_t Fork()
{

  pid_t retpid;
  if (retpid = fork() < 0)
    unix_error("fork error");

  return retpid;

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

 
  // !! don't forget your error checking for these calls !!
  // first, load up address structs with getaddrinfo():
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // fill in my IP for me
  getaddrinfo(NULL, MYPORT, &hints, &res);

  // make a socket, bind it, and listen on it (later):
  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  bind(sockfd, res->ai_addr, res->ai_addrlen);

  // ???
  addr_size = sizeof their_addr;

  while (1) {


    pid = Fork();
    if ( pid == 0 ) { // this is the child process

        // calling caget to get a pv value returned as a string in pvValue
      cagetFuZE(pv_Read, pvValue);

      // inserting PV value into the html message in place
      swapCharArray(htmlMessage, "[1##]", pvValue);

      // wait for a request
      listen(sockfd, BACKLOG);

      // ???
      new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

      // send HTML Message
      len = strlen(htmlMessage);
      bytes_sent = send(new_fd, htmlMessage, len, 0);

      // Sleep. if I don't sleep before close(new_fd), message doesn't always make it
      sleep(1);
      close(new_fd);
      exit(0);
    }

    // Reaping the child process with process ID, pid.
    retpid = waitpid(pid, NULL, 0);
    
  } 
}
