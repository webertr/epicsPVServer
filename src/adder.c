/****************************************************************************
 * adder.c - Compiled a binary that will be called as dynamic web content
 * by a server
 ***************************************************************************/

#include <simpleWebServer.h> // Header file that contains everything that will be needed


/******************************************************************************
 * Function: main
 * Inputs: None
 * Returns: int
 * Description: Makes a binary to be called is dynamic web content by a server
 * To call this: 10.10.10.245:3490/cgi-bin/adder?15000&213 into webbrowser
 ******************************************************************************/

int main(void) {
  char *buf, *p;
  char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
  int n1=0, n2=0;

  if ((buf = getenv("QUERY_STRING")) != NULL) {  // Get arguments (2) set to environment variable,
                                                 // QUERY_STRING
    p = strchr(buf, '&');                        // Returns pointer to first occurence of & in string
    *p = '\0';                                   // Puts an '\0' in place of '&'
    strcpy(arg1, buf);                           // Copies string up to the the '\0'
    strcpy(arg2, p+1);                           // Copies string after the '\0'
    n1 = atoi(arg1);
    n2 = atoi(arg2);
  }

  // Make the response body
  // Put it all in char* content
  sprintf(content, "Welcome to add.com: ");
  sprintf(content, "%sTHE Internet addition portal.\r\n<p>", content);
  sprintf(content, "%sThe answer is: %d + %d = %d\r\n<p>",
	  content, n1, n2, n1 + n2);
  sprintf(content, "%sThanks for visiting!\r\n", content);

  
  // Generate the HTTP response
  // Output content to the standard output (stdout), which has been re-directed to the connection
  printf("Content-length: %d\r\n", (int)strlen(content));
  printf("Content-type: text/html\r\n\r\n");
  printf("%s", content);

  fflush(stdout); // Flushes the output buffer of the stream. Ensure that everything got written

  exit(0);  // Call exit so that the child that called this kills

}
