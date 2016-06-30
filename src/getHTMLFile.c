#include <stdio.h>
#include <string.h>


void getHTMLFile(char *buffer) {

  FILE *f1;
  f1= fopen("/home/webertr/Github/epicsPVServer/var/html_file.html", "r" );

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


void swapCharArray(char* charArray, char* pattern, char* swapChar) {

  typedef enum {false, true} bool;
  int size = strlen(charArray);
  int sizeBuffer = strlen(pattern);
  int sizeSwap = strlen(swapChar);
  int endIter = size - sizeBuffer;
  bool test = false;
  int j;
  int i;
  int ii;
  
  for (i =0; i < endIter; i++) {

    test = true;
    j = 0;
    while ((test == true) && (j < sizeBuffer)) {

      if (charArray[i+j] != pattern[j]) {
	test = false;
      }

      j++;

    }

    if (test) {
      for (ii = 0; ii < sizeSwap; ii++) {
	charArray[i+ii] = swapChar[ii];
      }
    }
  }

  return;
      

}


  
