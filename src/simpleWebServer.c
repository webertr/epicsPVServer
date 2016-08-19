/****************************************************************************
 * simpleWebServer.c - A simple, iterative HTTP/1.0 Web server that uses the
 * GET method to serve static and dynamic content.
 ***************************************************************************/


//#include <stdio.h>
//#include <myfunctions.h>

#include <simpleWebServer.h> // Header file that contains everything that will be needed
#include <robustIO.h>



/******************************************************************************
 * Function: main
 * Inputs: int, char**
 * Returns: None
 * Description: This is a simple web server, that will serve up some PV 
 * (Process Variable, EPICS)  information
 ******************************************************************************/

int main(int argc, char **argv)
{
  int listenfd,                      // listenfd is a file descriptor
    connfd,                          // connfd is a file descriptor
    port,                            // The port that will be used for the simple web server (3490?).
    clientlen;                       // listenfd is a file descriptor

  struct sockaddr_in clientaddr;     // This is defined in <netinet/in.h>. This is a basic structure
                                     // for system calls that deal with internet addresses
  /*
  struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
  };

  struct in_addr {
    unsigned long s_addr;  // load with inet_aton()
  };
  */

  // Checks to see if only 1 command line argument was sent. argc is the "argument count"
  // Always at least 1, because the name of the program counts as an argument
  // argv is the argument vector. argv[0] has the file name.
  if (argc != 2) {

    // fprintf sends information to a stream (like a file, a data structure). In this case, it is
    // the standard error stream, stderr. stderr is a FILE *, or a file data structure pointer.
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  
  port = atoi(argv[1]); // Converts a character pointer to an integer. In <stdlib.h>

  listenfd = Open_listenfd(port); // Wrapper to open_listenfd. listenfd is a helper function for:
                                  // socket (create a socket), bind (bind it to a socket address
                                  // structure), listen (make it a listening socket), and return
                                  // the socket descriptor.

  while (1) {                     // Infinite while loop to handle connections.
    
    clientlen = sizeof(clientaddr);     // Gets length of clientaddr structure

    
    connfd = Accept(listenfd,           // Accept is a helper function for accept.
		    (SA *)&clientaddr,
		    &clientlen);          

    
    doit(connfd); // Function called to perform the actions requested by the client on connection
                  // Descriptor connfd

    Close(connfd); // Close connection after action is performed 
    
  }
  
}


/************************************************************************************** 
 * Function: Open_listend
 * Inputs: int port
 * Returns: int listenfd (a listen file descriptor)
 * Description: A wrapper to the open_listenfd function. Provides error handling
 **************************************************************************************/

int Open_listenfd(int port)
{

  int listenfd;

  listenfd = open_listenfd(port);

  if (listenfd < 0) {
    
    unix_error("listen Error");

  }

  return listenfd;

}


/************************************************************************************** 
 * Function: open_listend
 * Inputs: int port
 * Returns: int listenfd (a listen file descriptor)
 * Description: A helper function that combines socket, bind and listen. This will create 
 * a listen descriptor that is ready to receive connections on the port.
 **************************************************************************************/

int open_listenfd(int port) {

  int listenfd,                    // the listening socket descriptor
    optval=1;
  struct sockaddr_in serveraddr;   // This is defined in <netinet/in.h>. This is a basic structure
                                   // for system calls that deal with internet addresses

  // socket creates a socket descriptor. AF_INET indiciates we are using the internet.
  // SOCK_STREAM indicates that the socket will be the end point of an internet connection.
  listenfd = socket(AF_INET, SOCK_STREAM, 0); //

  // From the perspective of the Unix kernel, a socket is an end point for communication.
  // From the perspective of a Unix program, a socket is an open file with a corresponding descriptor.

  // This verifies that the socket descriptor is not < 0, indicating an error.
  if (listenfd  < 0) {
    
    return -1;

  }

  // Eliminates "Address already in use" error from bind 
  // The setsockopt allows us to immediatley restart a server after terminating. Usually it takes
  // 30 seconds
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
		 (const void *)&optval , sizeof(int)) < 0) {
    
    return -1;

  }

  // Listenfd will be an end point for all requests to port on any IP address for this host
  // The bzero() function sets the first n bytes of the area starting at s
  // to zero (bytes containing '\0') It is in <string.h>
  // Maybe this zeros the structure???
  bzero((char *) &serveraddr, sizeof(serveraddr));

  // Setting the structure to different values
  serveraddr.sin_family = AF_INET;
  // htonl goees from host byte order to network byte order
  // INADDR_ANY tells the kernel that this server will accept requests to any of the IP
  // addresses for this host.
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)port); // assigns the port number to the socket address
                                                     // structure

  // The bind function tells the kernel to associate the server’s socket address
  // in serveraddr with the socket descriptor listenfd. The addrlen argument is sizeof(sockaddr).
  if (bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr)) < 0) {

    return -1;

  }
  
  // Make it a listening socket ready to accept connection requests
  // By default, the kernel assumes that a descriptor created by the socket function corresponds
  // to an active socket that will live on the client end of a connection. A server calls the listen
  // function to tell the kernel that the descriptor will be used by a server instead of a client.
  if (listen(listenfd, LISTENQ) < 0) {
    
    return -1;

  }
  
  return listenfd;  // Return the listening socket descriptor

}


/************************************************************************************** 
 * Function: unix_error
 * Inputs: char *
 * Returns: None
 * Description: You pass this function a character pointer, and it outputs it to stderr.
 **************************************************************************************/

void unix_error(char *msg)
{
  // When main function gets evoked, you have 3 streams already: Input (stdin), Output (stdout)
  // and Error (stderr). A stream is a data structure. Files are represented as streams.
  // FILE is defined in stdio.h. A FILE object holds information about the file. possition, buffering.
  // fprintf sends formatted output to a stream.The w %s's are filled in with the two following values
  // strerror returns a pointer to a string that corresponds to the error code.
  // errno is set by system calls.
  fprintf(stderr, "%s: %s\n", msg, strerror(errno)); //
  exit(0);
}



/************************************************************************************** 
 * Function: Accept
 * Inputs: int, struct sockaddr, socklen_t
 * Returns: int (connected descriptor (NOT listening descriptor, which never changes))
 * Description: A wrapper to the accept function. The accept function waits for a connection request 
 * from a client to arrive on the listening descriptor listenfd, then fills in the client’s socket 
 * address in addr, and returns a connected descriptor that can be used to communicate with the client
 * using Unix I/O functions.
 **************************************************************************************/

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen) {

  int retint;  // Connected descriptor used for communication with client

  // #include <sys/socket.h>
  // int accept(int listenfd, struct sockaddr *addr, int *addrlen);
  // Returns: nonnegative connected descriptor if OK, −1 on error
  retint = accept(s, addr, addrlen);

  if (retint < 0) {
    
    unix_error("Accept Error");

  }

}


/************************************************************************************** 
 * Function: doit
 * Inputs: int (file descriptor
 * Returns: void
 * Description: This is a function called after Accept returns with a connection descriptor.
 * It executes the request by reading from the connection descriptor, getting the method,
 * an executing the method.
 **************************************************************************************/

void doit(int fd)
{

  int is_static; // holds the value indicating if it is a static of dynamic request
  struct stat sbuf; // A structure defined in sys/stat.h or something that returns file info
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio; // A structure that contains the descriptor, buffer, unread bytes...Used for connection

  /* Read request line and headers */
  Rio_readinitb(&rio, fd); // Initialize the robust io structure, used for connection
  Rio_readlineb(&rio, buf, MAXLINE); // Fills buffer, then reads line from connection
  sscanf(buf, "%s %s %s", method, uri, version); // output read line into method, uri, version
                                                 // the format for http requests.

  if (strcasecmp(method, "GET")) { // If the method wasn't "GET". Only method supported
    
    clienterror(fd, method, "501", "Not Implemented",
		"Tiny does not implement this method");
    return;

  }

  read_requesthdrs(&rio); // reads and ignored the request header

  /* Parse URI from GET request */
  is_static = parse_uri(uri, filename, cgiargs); // Takes the char* ur, and extracts the filename, and
                                                 // cgi arguments if dynamic content 
 
  fprintf(stderr, filename);                     // Outputs the filename to the standard error stream
    
  if (stat(filename, &sbuf) < 0) { // Checks to see if file is there. Retrieves file information.
    
    clienterror(fd, filename, "404", "Not found", // client error returns an http formated message 
		"Tiny couldn’t find this file");  // to the client, letting them know what happened
    return;

  }

  if (is_static) { // Serve static content (just get a file and output it

    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) { // sbuf struct was filled by stat()
      clienterror(fd, filename, "403", "Forbidden",              // function earlier 
		  "Tiny couldn’t read the file");
      return;
    }
    serve_static(fd, filename, sbuf.st_size); 
  }
  else { /* Serve dynamic content */
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
      clienterror(fd, filename, "403", "Forbidden",
		  "Tiny couldn’t run the CGI program");
      return;
    }
    serve_dynamic(fd, filename, cgiargs);
  }
}


/************************************************************************************** 
 * Function: read_requesthdrs
 * Inputs: rio_t (a structure to handle file I/O for the robust I/O)
 * Returns: void
 * Description: This function will read and ignore the request header. Has the format,
 * <header name>: <header data>. Provides additional information
 **************************************************************************************/

void read_requesthdrs(rio_t *rp)
{
  char buf[MAXLINE];
  rio_readlineb(rp, buf, MAXLINE); // reads line and fills buffer is necessary

  while(strcmp(buf, "\r\n")) {    // are the strings equal, less then, or greater then
                                  // The request header is terminated by an empty line

    rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);

  }

  return;
}


/************************************************************************************** 
 * Function: parse_uri
 * Inputs: char *, char *, char *
 * Returns: int
 * Description: This function will break the uri down into cgi arguments, and a filename.
 * It was also figure out if it is static or dynamic content.
 **************************************************************************************/

int parse_uri(char *uri, char *filename, char *cgiargs)
{
  char *ptr;

  if (!strstr(uri, "cgi-bin")) { /* Static content */

    strcpy(cgiargs, "");
    strcpy(filename, ".");
    strcat(filename, uri);

    if (uri[strlen(uri)-1] == '/') {
      strcat(filename, "home.html");
    }
    
    return 1;
  }
  else { /* Dynamic content */

    ptr = index(uri, '?');

    if (ptr) {
      strcpy(cgiargs, ptr+1);
      *ptr = '\0';
    }
    else {
      strcpy(cgiargs, "");
    }
    strcpy(filename, ".");
    strcat(filename, uri);
    return 0;
  }
}


/************************************************************************************** 
 * Function: clienterror
 * Inputs: int, char *, char *, char *, char *
 * Returns: void
 * Description: This function will output some string messages, in html format, back to
 * the client signfying something went wrong.
 **************************************************************************************/

void clienterror(int fd, char *cause, char *errnum,
		 char *shortmsg, char *longmsg)
{
  
  char buf[MAXLINE], body[MAXBUF];
  /* Build the HTTP response body */
  sprintf(body, "<html><title>Tiny Error</title>");
  sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

  /* Print the HTTP response */
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  Rio_writen(fd, body, strlen(body));

}


/************************************************************************************** 
 * Function: serve_static
 * Inputs: int, char *, int
 * Returns: void
 * Description: This function basically sends an header back to the client, then reads
 * the text or image from the file, and sends that to the client as well.
 **************************************************************************************/

void serve_static(int fd, char *filename, int filesize)
{
  int srcfd;
  char *srcp, filetype[MAXLINE], buf[MAXBUF];

  // Send response headers to client
  get_filetype(filename, filetype);
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
  sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
  sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
  Rio_writen(fd, buf, strlen(buf));

  // Send response body to client
  srcfd = Open(filename, O_RDONLY, 0);
  srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); // Mmap function maps the contents of
  Close(srcfd);                                               // the file to the char array, srcp
  Rio_writen(fd, srcp, filesize);                             // Write char * to connection descriptor
  Munmap(srcp, filesize);                                     // Remote mappings

  // Mmap function helps I guess if the OS needs to switch out the memory, and doesn't have to do
  // as much work??
}


/************************************************************************************** 
 * Function: get_filetype
 * Inputs: char *, char *
 * Returns: void
 * Description: It takes a filename, and figures out if it is text/html, or a image/gif
 **************************************************************************************/

void get_filetype(char *filename, char *filetype)
{
  
  if (strstr(filename, ".html"))      // strstr returns pointer to first instance of char *
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif"))
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".jpg"))
    strcpy(filetype, "image/jpeg");
  else
    strcpy(filetype, "text/plain");

}


void serve_dynamic(int fd, char *filename, char *cgiargs)
{
  char buf[MAXLINE], *emptylist[] = { NULL };

  /* Return first part of HTTP response */
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));

  if (Fork() == 0) { // This is the child
    setenv("QUERY_STRING", cgiargs, 1);   // Real server would set all CGI vars here
    Dup2(fd, STDOUT_FILENO);              // Redirect stdout to client
    Execve(filename, emptylist, environ); // Run CGI program
  }

  // Parent waits for and reaps child

}


int Open(const char *pathname, int flags, mode_t mode) {

  int retint;

  retint = open(pathname, flags, mode);

  if (retint < 0) {
    unix_error("Open Error");
  }

  return retint;

}


void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset) {

  mmap(addr, len, prot, flags, fd, offset);

}


void Munmap(void *start, size_t length) {

  munmap(start, length);

}


int Dup2(int fd1, int fd2) {

  dup2(fd1, fd2);

}


pid_t Wait(int *status) {

  pid_t retpid;
  
  retpid = wait(status);
  if (retpid  < 0) {
    unix_error("wait Error");
  }
  
  return retpid;

}


void Execve(const char *filename, char *const argv[], char *const envp[]) {

  execve(filename, argv, envp);

}



pid_t Fork()
{

  pid_t retpid;

  retpid = fork();
  
  if (retpid < 0) {    
    unix_error("fork error");
  }
  
  return retpid;

}
  

void Close(int fd) {

  close(fd);

}


/************************************************************************************** 
 * Function: open_clientfd
 * Inputs: char *, int
 * Returns: int 
 * Description: This is a helper function that combines socket and connect functions
 * to basically create a socket, and connect it to a host at some port.
 **************************************************************************************/

int open_clientfd(char *hostname, int port) {

  int clientfd;
  struct hostent *hp;
  struct sockaddr_in serveraddr;

  if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return -1; /* Check errno for cause of error */

  /* Fill in the server’s IP address and port */
  if ((hp = gethostbyname(hostname)) == NULL)
    return -2; /* Check h_errno for cause of error */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  bcopy((char *)hp->h_addr_list[0],
	(char *)&serveraddr.sin_addr.s_addr, hp->h_length);
  serveraddr.sin_port = htons(port);

  /* Establish a connection with the server */
  if (connect(clientfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0)
    return -1;
  return clientfd;

}


