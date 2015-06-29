#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <math.h>
#include <signal.h>

#include "my402list.h"
#include "warmup2.h"

//to display the queues
/* void display_list(My402List* dum_list)
{
	int chk=0,len=0,c=0;
	My402ListElem *elem=NULL;
	packet* pac=(packet*) malloc(sizeof(packet));
	chk=My402ListEmpty(dum_list);
	
	
	if(chk==TRUE)
		printf("\nList is empty, nothing to print");
	else
	{
		len=My402ListLength(dum_list);
		for(elem=My402ListFirst(dum_list),c=0;c<len ; elem=elem->next,c++)
		{
			
			pac=((packet*)(elem->obj));
			printf("\nPacket %d",pac->packet_name);
			
		}
		
		
	}
	
	return;
}  */
 
int main(int argc, char *argv[])
{
  int i=1,l=0,chk2=0,chk3=0,num_main=0,f=0,entry=0;
  char file_buf[1026]; FILE *fp;
  memset(file_buf, 0, 1026);
  double lambda_convert=(double)0;
  double mu_convert=(double)0;
  double r_convert=(double)0;
  int B_convert=0;
  int P_convert=0;
  int num_convert=0;
  struct timeval print_time;
  struct stat x;
  
  
  /* &queue1=(My402List*) malloc(sizeof(My402List));
  queue2=(My402List*) malloc(sizeof(My402List)); */
  chk2=My402ListInit(&queue1);
  chk3=My402ListInit(&queue2);
  
  
  //monitor thread
  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  pthread_sigmask(SIG_BLOCK,&set,0);
  //sigprocmask(SIG_BLOCK, &set, 0);
  
  
  
  if(argc==2)
  {
	  printf("\nWrong format....usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	...exiting...");
		exit(1);
  }
  if(argc == 1 )
  {
	if(!(strcmp(argv[0],"./warmup2")))
	{
		entry=1;
	}
  } 
  
  while(i<argc)
 {
  
	 
  if(!(strcmp(argv[i],"-lambda")))
  {
	  entry=1;
	
		if(argv[i+1]!=NULL)
		{
			if(argv[i+1][0]=='-')
			{
				printf("\nWrong format....usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	...exiting...");
				exit(1);
			}
			lambda_convert=atof(argv[i+1]);
			if(lambda_convert>0)
			{
				if(1/lambda_convert>10)
				{
					lambda_convert=0.1;
				}
				
				lambda_global=(double)(1000/lambda_convert);
				
			}
			else
			{
				printf("\nValue should be greater than 0....exiting...");
				exit(1);
			}
		}
		else
		{
			printf("\nWrong format....usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	...exiting...");
			exit(1);
		}
	
  }  
  
  else if(!(strcmp(argv[i],"-mu")))
  {
	  entry=1;
	
	
	if(argv[i+1]!=NULL )
	{
		if(argv[i+1][0]=='-')
		{
		printf("\nWrong format....usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	...exiting...");
		exit(1);
		}
		mu_convert=atof(argv[i+1]);
		if(mu_convert>0)
		{
			if(1/mu_convert>10)
				{
					mu_convert=0.1;
				}
				
				mu_global=(double)(1000/mu_convert);
		}
		else
		{
			printf("\nValue should be greater than 0....exiting...");
			exit(1);
		}
		
	}
	else
	{
		printf("\nWrong format..usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	..exiting...");
		exit(1);
	}
   	
  }
  
  else if(!(strcmp(argv[i],"-r")))
  {
	  entry=1;
	
	
	if(argv[i+1]!=NULL)
	{
			if(argv[i+1][0]=='-')
			{
				printf("\nWrong format....usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	...exiting...");
				exit(1);
			}
		r_convert=atof(argv[i+1]);
		if(r_convert>0)
		{
			if(1/r_convert>10)
				{
					r_convert=0.1;
				}
				
				r_global=(double)(1000/r_convert);
		}	
		else
		{
			printf("\nValue should be greater than 0....exiting...");
			exit(1);
		}
	}
	else
	{
		printf("\nWrong format...usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	..exiting...");
		exit(1);
	}
  
 }
  
  else if(!(strcmp(argv[i],"-B")))
  {
	  entry=1;
	
	
	if(argv[i+1]!=NULL)
	{
		if(argv[i+1][0]=='-')
		{
			printf("\nWrong format....usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	...exiting...");
			exit(1);
		}
		B_convert=atoi(argv[i+1]);
		if(B_convert>0)
		{
			B=B_convert;
		}
		else
		{
			printf("\nValue should be greater than 0....exiting...");
			exit(1);
		}
	}
	else
	{
		printf("\nWrong format..usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	..exiting...");
		exit(1);
	}
  
 }
  
  else if(!(strcmp(argv[i],"-P")))
  {
	  entry=1;
	
	
	if(argv[i+1]!=NULL)
	{
		if(argv[i+1][0]=='-')
		{
			printf("\nWrong format....usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	...exiting...");
			exit(1);
		}
		P_convert=atoi(argv[i+1]);
		if(P_convert>0)
		{
			P=P_convert;
		}
		else
		{
			printf("\nValue should be greater than 0....exiting...");
			exit(1);
		}
	}
	else
	{
		printf("\nWrong format..usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	..exiting...");
		exit(1);
	}
	
  }
  
  else if(!(strcmp(argv[i],"-n")))
  {
	  entry=1;
	
	
	if(argv[i+1]!=NULL)
	{
		if(argv[i+1][0]=='-')
		{
			printf("\nWrong format....usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	...exiting...");
			exit(1);
		}
		num_convert=atoi(argv[i+1]);
		if(num_convert>0)
			num=num_convert;
		else
		{
			printf("\nWrong value out of bounds....exiting...");
			exit(1);
		}
	}
	else
	{
		printf("\nWrong format..usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	..exiting...");
		exit(1);
	}
  
  }
  
  else if(!(strcmp(argv[i],"-t")))
  {
	  entry=1;
	
	if(argv[i+1]!=NULL)
	{
		if(argv[i+1][0]=='-')
		{
			printf("\nWrong format....usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	...exiting...");
			exit(1);
		}
		f=stat(argv[i+1],&x);
		if(S_ISDIR(x.st_mode))
		{
		 printf("\nDirectory....no file present....exiting");
		 exit(1);
		}  
		  
		file_flag=1;
		strcpy(file_name,argv[i+1]);
		l=strlen(file_name);
		file_name[l]='\0';
		//now packet thread may call read function
		//chk1=file_read(file_name); 
		fp = fopen(file_name,"r"); // read mode
		if( fp == NULL )
		{
		  perror("\nError while opening the file\n");
		  exit(1);
		}
		
		if(fgets(file_buf,sizeof(file_buf),fp)!=NULL)
		{
			num_main=atoi(file_buf);
			if(num_main<=0)
			{
				printf("\nError in file....exiting");
				exit(0);
			}
			//printf("\n Num is %d",num);
		}
		fclose(fp);
	}
	else
	{
		printf("\nWrong format..usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	..exiting...");
		exit(1);
	}
	
   }
  i++;
 }
  
  if(entry==0)
  {
	printf("\nWrong ffformat..usage is ./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]	..exiting...\n");  
	exit(1); 
  }
  //printf("Thread creating");
  
  //Packet arrival thread
  pthread_create(
	&arrival_thr,
	0,
	(void *)arrival_func,
	0);
  
  //token bucket thread  
  pthread_create(
	&token_thr,
	0,
	(void *)token_func,
	0);
	
	
   //server 1 thread
   pthread_create(
	&server1_thr,
	0,
	(void *)server1_func,
	0);
  
  //server 2 thread
   pthread_create(
	&server2_thr,
	0,
	(void *)server2_func,
	0);  

  
  //pthread_sigmask(SIG_BLOCK,&set,0);
  pthread_create(&thr, 0, (void *)monitor, 0);
  //long_running_proc();
	
	//printf("\nThread created");
	
	if(err)
	{
	  exit(1);
	}
	
	gettimeofday(&start_time,NULL);
	printf("\nEmulation Parameters :");
	
	if(file_flag)
	{
		printf("\nnumber to arrive = %d",num_main);
		printf("\nr = %lf",1000.00/r_global);
		printf("\nB = %d",B);
		printf("\ntsfile = %s",file_name);
	}
	
	else
	{
		printf("\nnumber to arrive = %d",num);
		printf("\nlambda = %lf",1000.00/lambda_global);
		printf("\nmu = %lf",1000.00/mu_global);
		printf("\nr = %lf",1000.00/r_global);
		printf("\nB = %d",B);
		printf("\nP = %d",P);
	}
	
	
	
	printf("\n\n%012.3lfms : emulation begins", subtract_time(start_time,start_time));
		

		//printf("\nabout to join");
  //all joined
  pthread_join(arrival_thr, 0);	
  
  pthread_join(token_thr, 0);
  
  pthread_join(server1_thr, 0);
  
  pthread_join(server2_thr, 0); 
 
  //pthread_join(thr, 0); 
	
  /* printf("\nQueue1 has:");
  display_list(&queue1);
  //print queue2
  printf("\nQueue2 has:");
  display_list(&queue2); */
  
  gettimeofday(&print_time,NULL);
  printf("\n%012.3lfms : emulation Ends\n", subtract_time(print_time,start_time));
  
  //statistics
  display_stats(subtract_time(print_time,start_time));
  
  
  My402ListUnlinkAll(&queue1);
  My402ListUnlinkAll(&queue2);
  return 0;
}
