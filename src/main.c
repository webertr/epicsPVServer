/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
 *GET method to serve static and dynamic content.
*/


//#include <stdio.h>
//#include <myfunctions.h>

#include <csapp.h>

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum,
		 char *shortmsg, char *longmsg);


//3490 Port to use, I guess.

int main(int argc, char **argv)
{
  int listenfd, connfd, port, clientlen;
  struct sockaddr_in clientaddr;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  
  port = atoi(argv[1]);

  listenfd = Open_listenfd(port);

  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    //doit(connfd);
    //Close(connfd);
  }
  
}



/************************** 
 * Error-handling functions
 **************************/
/* $begin errorfuns */
/* $begin unixerror */
void unix_error(char *msg) /* Unix-style error */
{
  fprintf(stderr, "%s: %s\n", msg, strerror(errno));
  exit(0);
}
/* $end unixerror */


void doit(int fd)
{

  int is_static;
  struct stat sbuf;
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio;

  
  /* Read request line and headers */
  Rio_readinitb(&rio, fd);
  Rio_readlineb(&rio, buf, MAXLINE);
  sscanf(buf, "%s %s %s", method, uri, version);

  if (strcasecmp(method, "GET")) {
    clienterror(fd, method, "501", "Not Implemented",
		"Tiny does not implement this method");
    return;
  }

  read_requesthdrs(&rio);

  /* Parse URI from GET request */
  is_static = parse_uri(uri, filename, cgiargs);

  if (stat(filename, &sbuf) < 0) {
    clienterror(fd, filename, "404", "Not found",
		"Tiny couldn’t find this file");
    return;
  }

  if (is_static) { /* Serve static content */
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
      clienterror(fd, filename, "403", "Forbidden",
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


void read_requesthdrs(rio_t *rp)
{
  char buf[MAXLINE];
  Rio_readlineb(rp, buf, MAXLINE);

  while(strcmp(buf, "\r\n")) {
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
  }

  return;
}




int parse_uri(char *uri, char *filename, char *cgiargs)
{
  char *ptr;

  if (!strstr(uri, "cgi-bin")) { /* Static content */
    strcpy(cgiargs, "");
    strcpy(filename, ".");
    strcat(filename, uri);
    if (uri[strlen(uri)-1] == '/')
      strcat(filename, "home.html");
    return 1;
  }
  else { /* Dynamic content */
    ptr = index(uri, '?');
    if (ptr) {
      strcpy(cgiargs, ptr+1);
      *ptr = '\0';
    }
    else
      strcpy(cgiargs, "");
    strcpy(filename, ".");
    strcat(filename, uri);
    return 0;
  }

}


void serve_static(int fd, char *filename, int filesize)
{
  int srcfd;
  char *srcp, filetype[MAXLINE], buf[MAXBUF];

  /* Send response headers to client */
  get_filetype(filename, filetype);
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
  sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
  sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
  Rio_writen(fd, buf, strlen(buf));

  /* Send response body to client */
  srcfd = Open(filename, O_RDONLY, 0);
  srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
  Close(srcfd);
  Rio_writen(fd, srcp, filesize);
  Munmap(srcp, filesize);

}



/*
 * get_filetype - derive file type from file name
 */
void get_filetype(char *filename, char *filetype)
{
  if (strstr(filename, ".html"))
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

  if (Fork() == 0) { /* child */
    /* Real server would set all CGI vars here */
    setenv("QUERY_STRING", cgiargs, 1);
    Dup2(fd, STDOUT_FILENO);
    /* Redirect stdout to client */
    Execve(filename, emptylist, environ); /* Run CGI program */
  }

  Wait(NULL); /* Parent waits for and reaps child */

}


int Open(const char *pathname, int flags, mode_t mode) {

  int retint;
  if (retint = open(pathname, flags, mode) < 0)
    unix_error("Open Error");

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

  if (retpid = wait(status) < 0)
    unix_error("wait Error");

  return retpid;

}


void Execve(const char *filename, char *const argv[], char *const envp[]) {

  execve(filename, argv, envp);

}



void Rio_writen(int fd, void *usrbuf, size_t n) {

  ssize_t sizecheck;
  
  sizecheck = rio_writen(fd, usrbuf, n);

}

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen) {

  int retint;

  if (retint = accept(s, addr, addrlen) < 0)
    unix_error("Accept Error and Corndogs");

}

void Rio_readinitb(rio_t *rp, int fd) {

  rio_readinitb(rp, fd);

}


ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) {

  ssize_t retsize;

  if (retsize = rio_readlineb(rp, usrbuf, maxlen))
    unix_error("readlineb Error");

  return retsize;

}



pid_t Fork()
{

  pid_t retpid;
  if (retpid = fork() < 0)
    unix_error("fork error");

  return retpid;

}
  

void Close(int fd) {

  close(fd);

}


// A wrapper for the open function
int Open_listenfd(int port)
{

  int retint;
  if (retint = open_listenfd(port) < 0)
    unix_error("listen Error");

  return retint;

}



int open_listenfd(int port)
{
  int listenfd, optval=1;
  struct sockaddr_in serveraddr;

  /* Create a socket descriptor */
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    return -1;
  /* Eliminates "Address already in use" error from bind */
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
		 (const void *)&optval , sizeof(int)) < 0)
    return -1;

  /* Listenfd will be an end point for all requests to port
     on any IP address for this host */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)port);
  if (bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr)) < 0)
    return -1;

  /* Make it a listening socket ready to accept connection requests */
  if (listen(listenfd, LISTENQ) < 0)
    return -1;

  return listenfd;

}


ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
  size_t nleft = n;
  ssize_t nwritten;
  char *bufp = usrbuf;

  while (nleft > 0) {
    if ((nwritten = write(fd, bufp, nleft)) <= 0) {
      if (errno == EINTR) /* Interrupted by sig handler return */
	nwritten = 0;
      /* and call write() again */
      else
	return -1;
      /* errno set by write() */
    }
    nleft -= nwritten;
    bufp += nwritten;
  }
  return n;

}


ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
  int cnt;

  while (rp->rio_cnt <= 0) { /* Refill if buf is empty */
    rp->rio_cnt = read(rp->rio_fd, rp->rio_buf,
		       sizeof(rp->rio_buf));
    if (rp->rio_cnt < 0) {
      if (errno != EINTR) /* Interrupted by sig handler return */
	return -1;
    }
    else if (rp->rio_cnt == 0) /* EOF */
      return 0;
    else
      rp->rio_bufptr = rp->rio_buf; /* Reset buffer ptr */
  }

  /* Copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
  cnt = n;
  
  if (rp->rio_cnt < n)
    cnt = rp->rio_cnt;
  memcpy(usrbuf, rp->rio_bufptr, cnt);
  rp->rio_bufptr += cnt;
  rp->rio_cnt -= cnt;

  return cnt;

}



ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
  int n, rc;
  char c, *bufp = usrbuf;

    for (n = 1; n < maxlen; n++) {
      if ((rc = rio_read(rp, &c, 1)) == 1) {
	*bufp++ = c;
	if (c == '\n')
	  break;
      } else if (rc == 0) {
	if (n == 1)
	  return 0; /* EOF, no data read */
	else
	  break;
	/* EOF, some data was read */
      } else
	return -1;
      /* Error */
    }
  *bufp = 0;
  return n;

}

void rio_readinitb(rio_t *rp, int fd)
{
  rp->rio_fd = fd;
  rp->rio_cnt = 0;
  rp->rio_bufptr = rp->rio_buf;
}



/* int main(void) { */
/*   listenTest(); */
  
/*   return 0; */
/* } */

