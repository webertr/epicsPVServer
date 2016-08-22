/****************************************************************************
 * pvDisplay.c - Compiled a binary that will be called as dynamic web content
 * by a server
 ***************************************************************************/

#include <simpleWebServer.h> // Header file that contains everything that will be needed


void cagetFuZE(char *pvName, char *pvValue);

/******************************************************************************
 * Function: main
 * Inputs: None
 * Returns: int
 * Description: Makes a binary to be called is dynamic web content by a server
 * To call this: 10.10.10.245:3490/cgi-bin/adder?15000&213 into webbrowser
 ******************************************************************************/

int main(void) {

  char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE], pvn_1[36], pvn_2[36], val1[10], val2[20];
  char *p, *buf;

  if ((buf = getenv("QUERY_STRING")) != NULL) {  // Get arguments (2) set to environment variable,
                                                 // QUERY_STRING
    p = strchr(buf, '&');                        // Returns pointer to first occurence of & in string
    *p = '\0';                                   // Puts an '\0' in place of '&'
    strcpy(pvn_1, buf);                           // Copies string up to the the '\0'
    strcpy(pvn_2, p+1);                           // Copies string after the '\0'
  }

  cagetFuZE(pvn_1, val1);
  cagetFuZE(pvn_2, val2);
    
  // Make the response body
  // Put it all in char* content
  sprintf(content, "Welcome to add.com: ");
  sprintf(content, "%sTHE Internet addition portal.\r\n<p>", content);
  sprintf(content, "%sPV fred1 is: %s\r\n<p>",
	  content, val1);
  sprintf(content, "%sPV fred2 is: %s\r\n<p>",
	  content, val2);
  sprintf(content, "%sThanks for visiting!\r\n", content);

  
  // Generate the HTTP response
  // Output content to the standard output (stdout), which has been re-directed to the connection
  printf("Content-length: %d\r\n", (int)strlen(content));
  printf("Content-type: text/html\r\n\r\n");
  printf("%s", content);

  fflush(stdout); // Flushes the output buffer of the stream. Ensure that everything got written

  exit(0);  // Call exit so that the child that called this kills

}
