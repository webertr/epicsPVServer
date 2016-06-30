#include <stdio.h>
#include <myfunctions.h>
#include <pthread.h>
#include <semaphore.h>


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

  	
  while(i < 5)
    {
      usleep(1);
      printf("main is running...\n");
      ++i;
    }

  //caExample("fred");
  printf("main waiting for thread to terminate...\n");

  pthread_join(pth,NULL);

  // to stop a thread:
  // int pthread_cancel(pthread_t thread);
  
  //listenTest();
  

  return 0;
}
