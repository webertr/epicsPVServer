#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>


void socketTest(void) {

  int s;
  struct addrinfo hints, *res;

  // do the lookup
  // [pretend we already filled out the "hints" struct]
  getaddrinfo("www.example.com", "http", &hints, &res);

  // [again, you should do error-checking on getaddrinfo(), and walk
  // the "res" linked list looking for valid entries instead of just
  // assuming the first one is good (like many of these examples do.)
  // See the section on client/server for real examples.]
  s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  /*
  //struct addrinfo hints, *res;
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
  */
}
