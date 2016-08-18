#include <robustIO.h>


void Rio_writen(int fd, void *usrbuf, size_t n) {

  ssize_t sizecheck;
  
  sizecheck = rio_writen(fd, usrbuf, n);

}


ssize_t rio_writen(int fd, void *usrbuf, size_t n)
{
  // size_t is an unsigned data type defined by several C/C++ standards
  // ssize_t is defined as an int.
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


ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) {

  ssize_t retsize;

  retsize = rio_readlineb(rp, usrbuf, maxlen);

  if (retsize < 0) {
    
    unix_error("readlineb Error");

  }

  return retsize;

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


void Rio_readinitb(rio_t *rp, int fd) {

  rio_readinitb(rp, fd);

}


void rio_readinitb(rio_t *rp, int fd)
{
  rp->rio_fd = fd;
  rp->rio_cnt = 0;
  rp->rio_bufptr = rp->rio_buf;
}

