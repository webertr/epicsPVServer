#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void getHTMLFile(char *buffer) {

  FILE *f1;
  f1= fopen("/home/webertr/CPP_Stuff/simpleWebServer/var/html_file.html", "r" );

  //char *fgets( char *buf, int n, FILE *fp );
  //char *temp = fgets(buffer, 100, f1);

  int ret;
  int i = 0;
  do {

    ret = fgetc(f1);
    buffer[i] = ret;
    i++;

  }
  while (ret !=EOF);
  
  //int fclose( FILE *fp );
  ret = fclose(f1);

  return;

}
