#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include "../include/myfunctions.h"
#include <pthread.h>
#include <semaphore.h>
//#include "cadef.h"

/* This is our thread function.  It is like main(), but for a thread */
void *threadFunc(void *arg)
{

  listenTest();
  
  return NULL;
}

int main(void) {


  pthread_t pth; // this is our thread identifier
  int i = 0;
 
  void *(*func) (void *arg1);

  func = &threadFunc;


  //listenTest();
  pthread_create(&pth,NULL,func,"foo");

  	
  while(i < 100)
    {
      usleep(1);
      printf("main is running...\n");
      ++i;
    }

  printf("main waiting for thread to terminate...\n");
  pthread_join(pth,NULL);

  // int pthread_create(pthread_t * pth, pthread_attr_t *att, void * (*function), void * arg);
  // to stop a thread:
  // int pthread_cancel(pthread_t thread);
  
  //listenTest();
  

  return 0;
}
