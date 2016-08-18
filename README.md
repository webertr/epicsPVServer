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

An input function cannot 
