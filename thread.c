#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

int i=5;
pthread_mutex_t M = PTHREAD_MUTEX_INITIALIZER;

void *disp1()
{
  usleep(5000000);
  printf("\nT1 value was %d\n",i);
  pthread_mutex_lock(&M);
  i=i+2;
  pthread_mutex_unlock(&M);
  printf("\nInside thread 1 value is %d\n",i);
  return 0;
}

void *disp2()
{
  printf("\nT2 value was %d\n",i);
  pthread_mutex_lock(&M);
  i=i+3;
  pthread_mutex_unlock(&M);
  printf("\nInside thread 2 value is %d\n",i);
  return 0;
}

int main()
{
 
  pthread_t thr1;
  pthread_t thr2;
  
  printf("\nYo creating");
  /* for (i=0; i<4; i++)
  { */
    pthread_create(
	&thr1,
	0,
	disp1,
	0);
	
   printf("\nIn middle");
	
	pthread_create(
	&thr2,
	0,
	disp2,
	0);
  //}
  
  printf("\nYo created");
  pthread_join(thr1, 0);	
  pthread_join(thr2, 0);	
  
  return 0;
}
