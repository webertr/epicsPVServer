To access this site, just run this in a command line:
./pv_server 3490
Then, type into your web browser the following:
http://10.10.10.248:3490/var/index.html
Assuming 3490 is the port you want, and the IP of your machine is 10.10.10.248.





I had to go into /etc/ld.so.conf.d/
and add a file called "epics.conf" with the line,
root@fuze2:/etc/ld.so.conf.d# more epics.conf
/usr/src/epics/base-3.15.3/lib/linux-x86_64

otherwise, the OS couldn't find the shared library. When I ran the binary, I would get this message:
./pv_server: error while loading shared libraries: libdbCore.so.3.15.3: cannot open shared object file: No such file or directory

I installed the epics distribution in /usr/src/epics/base-3.15.3



Here are my notes from Bryant's Computer Systems Chapter 10, System-level I/O:
You ask the kernel to open a file. The kernel returns a small, non-negative integer called a descriptor to identify the file for all subsequent operations.
The kernel maintains the file position k.
When you close a file, the kernel frees the data structure it created, and returns the file descriptor to the pool for free descriptors.
#include <unistd.h>
ssize_t read(int fd, void *buf, size_t n);
Returns: number of bytes read if OK, 0 on EOF, −1 on error
ssize_t write(int fd, const void *buf, size_t n);
Returns: number of bytes written if OK, −1 on error


The read function copies from the current file position of the file descriptor to the buffer. Returning -1 indicates an error.
A return value of 0 indicates an EOF (End-of-file)
Otherwise, return value indicates the number of bytes that were transfered.
Write copies n bytes from buf to the current file position of the file descriptor, fd.
write returns the number of bytes written.

Short counts occur when read or write operations transfer less bytes then the application requires. They do not indicate an error. They can occur because:
-a read operation encountered an EOF
-Reading from a terminal, each read operation transfers one line at a time.
-If the open file corresponds to a network socket, internal buffering constaints, and long network delays can cause read and write to return short counts.

"Thread safe" means a data structure the access to which can be done safely by multiple threads.

RIO = Robust I/O. Should handle network read/writes that are subject to short counts.

RIO will provide:
-Unbuffered input/output functions to transfer data directly to and from files without any application level buffering. Especially useful for networks.
-Buffered input functions:


ssize_t rio_readn(int fd, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);
Returns: number of bytes transferred if OK, 0 on EOF ( rio_readn only), −1 on error

rio_readn can only return a short if an EOF is encountered.
Apparently calls to rio_readn and rio_writen can be interleaved on the same descriptor. Who knows.

Oh, these things allow for signal call interupts! They check for "errno".
The <errno.h> header file defines the integer variable errno, which is set by system calls and some library functions in the event of an error to indicate what went wrong.  Its value is significant only when the return value of the call indicated an error (i.e., -1 from most system calls; -1 or NULL from most library functions); a function that succeeds is allowed to change errno.

Basically, rio check for errno, and keeps reading/writting until an EOF is encoutered, or everything has been read.

Rio Buffered Input Functions.

The problem is we want to read a text line until we encounter a '\n' new line character.

void rio_readinitb(rio_t *rp, int fd);
Returns: nothing

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);
Returns: number of bytes read if OK, 0 on EOF, −1 on error

The rio_readinitb function is called once per open descriptor. It associates the descriptor fd with a read buffer of type rio_t at address rp.

rio_t is a structure:
#define RIO_BUFSIZE 8192
typedef struct {
  int rio_fd;                /* Descriptor for this internal buf */
  int rio_cnt;               /* Unread bytes in internal buf */
  char *rio_bufptr;          /* Next unread byte in internal buf */
  char rio_buf[RIO_BUFSIZE]; /* Internal buffer */
} rio_t;


When you call readinitb, you have to pass it a rio_t structure.

rio_readinitb just sets the structure values for the rio_t it is passed.

So basically, these fill a buffer by doing a series of reads by the size of the buffer. Okay, I think I get that.


Here is an example:

int main(int argc, char **argv)
{
int n;
rio_t rio;
char buf[MAXLINE];

Rio_readinitb(&rio, STDIN_FILENO);
while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0)
   Rio_writen(STDOUT_FILENO, buf, n);
}


So it has to pass a fd, or descriptor. In this case, it passes,
STDIN_FILENO
which is the default standard input file descriptor, most like zero. You use it with functions like read().

stdin is a FILE *, or a file pointer. You use it with functions like fscanf().

fopen, fclose, fread, fwrite, scanf, printf are high level functions provided by the C langauge, and in the libc package. They are an alternative to the unix stuff.

The standard I/O modules provided with C module open files as streams. A stream is a pointer to a type called "FILE". Stream = FILE *

Every ANSI C program begins with 3 streams:

#include <stdio.h>
extern FILE *stdin; /* Standard input (descriptor 0) */
extern FILE *stdout; /* Standard output (descriptor 1) */
extern FILE *stderr; /* Standard error (descriptor 2) */

A FILE type is an abstraction for a file descriptor and stream buffer.


At the end of the day, the purpose of all these things is to minimize the number of Unix I/O system calls. rio_readline works by calling a read 1 character, a bunch of times. But it is reading from a buffer, that has been filled by just one call with the maximum size of it's buffer, and will re-fill when it is empty.

The ANSI C things in stdio, or libc, I don't know, do the same thing as rio.

open, close, read and write are Unix I/O functions, they are system calls, essentially.

The ANSI standard I/O is the best thing to use for disks and terminal devices.

For network applications, the standard I/O is no good.

A network is a type of a "file" called a socket.

The problems with the standard I/O and sockets happen b/c sockets are full-duplex, meaning you can both read and write on the same socket.

An input function cannot following an output function without resetting current file position or flushing buffer for standard I/O.



Chapter 11 - Networking Programming from Bryant Computer Systems

These are my notes on this chapter.

Network application use client-server model.

Fundamental thing is a transaction, between the client and server.
Transaction:
Client sends request
Server receives request, and manipulates it resources in the approriate way.
Server sends a response to the client, then waits for the next request.
The Client receives the response and manipulates it.

An IP address is stored as a structure. This is an unforunate historical fact.


A socket is the end point of a connection.
Each socket has a socket address, which consists of an IP address, and a 16-bit port number. It is denoted address:port.

/etc/services contains a list of well-known ports.

A connection is uniquely defined by it's pair of socket address. You can think of it as a tuple.

A "socket interface" is a set of functions that is used in conjunction with the Unix I/O functions to build network applications.

Overview of the socket interface:

Client	    	Server
socket	    	socket
	    	bind
	    	listen
connect->   	accept
rio_written->	rio_readlineb
rio_readlineb	<-rio_written
close -> EOF	rio_readlineb
      	 	close -> goes back to accept


In the code, a "socket" is just an open file, with a descriptor.
From the kernel's prespective, a socket is an end point for communication.

connect, bind and accept require a pointer to a protocol specific socket address structure. In order for these to work for anytime of sockets (no void * at the time), they defined a generic
sockaddr structure:
typedef struct sockaddr SA;
So, anytime you give your socket, you have to cast it to this generic socket structure SA,
(SA *) your_socket

#include <sys/types.h>
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
Returns: nonnegative descriptor if OK, −1 on error

This creates and returns a socket descriptor, based on the domain, type and protocol.

This is what we will be using:
clientfd = Socket(AF_INET, SOCK_STREAM, 0);

AF_INET = the internet
SOCK_STREAM = this socket is an endpoint for an internet connection
0 = ???
Remember, the capitali Socket, just means this is an error-handling wrapper function.

We can't use the socket yet until we finalize it. How we finalize it depends on if we are a server or client.

#include <sys/socket.h>
int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
Returns: 0 if OK, −1 on error

connect attempts to establish a connection with a server at socket address, serv_addr.
If it is successful, then the socket characterized by the descriptor, sockfd, is now ready to read and write to/from.


The socket and connect function gets wrapped in a helper,
int open_clientfd(char *hostname, int port);
Returns: descriptor if OK, −1 on Unix error, −2 on DNS error

int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
Returns: 0 if OK, −1 on error

The bind function tells the kernel to associate the server’s socket address in my_addr with the socket descriptor sockfd. The addrlen argument is sizeof(sockaddr_in).
So you make a socket, with socket(), then take that socket descriptor and combine it with a struc sockaddr *my_addr.

#include <sys/socket.h>
int listen(int sockfd, int backlog);
Returns: 0 if OK, −1 on error

By default, the kernel assumes that the socket will be a client. By calling liste() on teh socket descriptor, you let the kernel know that this will be a server socket.
So the kernel assumes a "Active Socket". By calling listen, it chagnes it to a "Listening Socket".
backlog says the number of connections it should que before refusing.

int open_listenfd(int port);
Returns: descriptor if OK, −1 on Unix error
This combines socket, bind, and listen.

#include <sys/socket.h>
int accept(int listenfd, struct sockaddr *addr, int *addrlen);
Returns: nonnegative connected descriptor if OK, −1 on error

The accept function waits for a connection request from a client to arrive on the listening descriptor listenfd, then fills in the client’s socket address in addr, and returns a connected descriptor that can be used to communicate with the client using Unix I/O functions.

Important point. THe accept function returns a connected descriptor, which describes the connection.
The listening descriptor describes the socket for the server that is listening. It descrbies the end point for the client connection requests.

A web server can provide content in 1 of 2 ways:

1) Fetch a file from a disk, and send it to teh client (static content)
2) Run an executable and send the contents to the client (dynamic content)

An http request has the form:
<method> <uri> <version>

method = GET, POST, OPTIONS, etc.
uri = URL, includes filename and optional arguments

An http response:
<version> <status code> <status message>

status code = 3 digit number indiciates disposition of the request (OK, Bad request, etc..)
status message = English equivalent of the error code

Common Gateway Interface (CGI) standard for serving dynamic content.

'?' separates filename from arguments.
'&' symbol separates each argument
spaces not allowed.

example:
GET /cgi-bin/adder?15000&213 HTTP/1.1

After the server receives the above, it calls fork to create a child process and calls execve to run the /cgi-bin/adder program in the context of the child. Programs like the adder program are often referred to as CGI programs because they obey the rules of the CGI standard. And since many CGI programs are written as Perl scripts, CGI programs are often called CGI scripts. Before the call to execve, the child process sets the CGI environment variable QUERY_STRING to “15000&213”, which the adder program can reference at run time using the Unix getenv function.


The cgi program looks for environment variables, like
QUERY_STRING	      	  Program arguments
SERVER_PORT		  Port that the parent is listening on
REQUEST_METHOD		  GET or POST
REMOTE_HOST		  Domain name of client
REMOTE_ADDR		  Dotted-decimal IP address of client
CONTENT_TYPE		  POST only: MIME type of the request body
CONTENT_LENGTH 		  POST only: Size in bytes of the request body

I guess you can define this in the server program before calling execve from the forked child.

Write them with,
setenv("QUERY_STRING", cgiargs, 1);

Read them with,
buf = getenv("QUERY_STRING")
