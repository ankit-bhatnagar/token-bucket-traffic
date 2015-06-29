// all_function.c
//function definitions present here


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

int B=10;
int current_tokens=0;
pthread_mutex_t M = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv;
double lambda_global=(double)(1000/1);
double mu_global=(double)(1000/0.35);
double r_global=(double)(1000/1.5);
int num = 20;
int P=3;
char file_name[50]={0};
int file_flag=0;
int arrival_thread_finished=0;
int token_thread_finished=0;
My402List queue1;
My402List queue2;
struct timeval start_time;
int end_is_reached=0;
int dropped_tokens=0;
int dropped_packets=0;
int total_tokens=0;
int is_cntrlc_pressed=0;
int err=0;
int processed_packets=0;

sigset_t set;
int packets_serviced=0;
pthread_t arrival_thr;
pthread_t token_thr;
pthread_t server1_thr;
pthread_t server2_thr;
pthread_t thr;
double sum_packets_inter_arrival_time=0.0;
double sum_packets_service_time=0.0;
double sum_Q1_time=0.0;
double sum_Q2_time=0.0;
double sum_S1_time=0.0;
double sum_S2_time=0.0;
double sum_system_time=0.0;
double square_sum_system_time=0.0;
double processed_packets_inter_arrival_time=0.0;
//time difference calculation function
double subtract_time(struct timeval t1,struct timeval t2)
{
	long sec = t1.tv_sec - t2.tv_sec;
    long microsec = t1.tv_usec - t2.tv_usec;
   //check for microseconds being less
   if(microsec < 0){
        sec = sec - 1;
        microsec = microsec + 1000000;
    }
    double ans_millisec = ((sec * 1000000) + microsec) / 1000.00;
    return ans_millisec;
}


void display_stats(double temp_time)
{
	double token_drop_prob=0.0;
	double packet_drop_prob=0.0;
	double average_packet_inter_arrival_time=0.0;
	double average_packet_service_time=0.0;
	double temp1=0.0, temp2=0.0, standard_deviation=0.0;
	
	pthread_mutex_lock(&M);
	
	printf("\n\nStatistics :\n");
	//all this should be in seconds
	if(packets_serviced==0)
	{
		average_packet_inter_arrival_time= ((sum_packets_inter_arrival_time + processed_packets_inter_arrival_time)/processed_packets);
		printf("\n    average packet inter-arrival time = %.6g",(double)average_packet_inter_arrival_time/1000.00);
		printf("\n    average packet service time = N/A as no packets serviced\n");
		
		if((sum_Q1_time/temp_time)==0)
			printf("\n    average number of packets in Q1 = N/A as no packets at this facility");
		else
			printf("\n    average number of packets in Q1 = %.6g", (double)(sum_Q1_time/temp_time));
		if((sum_Q2_time/temp_time)==0)
			printf("\n    average number of packets in Q2 = N/A as no packets at this facility");
		else
			printf("\n    average number of packets in Q2 = %.6g", (double)(sum_Q2_time/temp_time));
		if((sum_S1_time/temp_time)==0)
			printf("\n    average number of packets at S1 = N/A as no packets at this facility");
		else
			printf("\n    average number of packets at S1 = %.6g", (double)(sum_S1_time/temp_time));
		if((sum_S2_time/temp_time)==0)
			printf("\n    average number of packets at S2 = N/A as no packets at this facility");
		else
			printf("\n    average number of packets at S2 = %.6g\n", (double)(sum_S2_time/temp_time));
		
		printf("\n    average time a packet spent in system = N/A as no packets serviced");
		printf("\n    standard deviation for time spent in system = N/A as no packets serviced\n");
		
		token_drop_prob=(double)dropped_tokens/total_tokens;
		printf("\n    token drop probability = %.6g", token_drop_prob);
		
		packet_drop_prob=(double)dropped_packets/processed_packets;
		printf("\n    packet drop probability = %.6g\n", packet_drop_prob);
		
		pthread_mutex_unlock(&M);
		return;
	}
	average_packet_inter_arrival_time= ((sum_packets_inter_arrival_time + processed_packets_inter_arrival_time)/processed_packets);
	printf("\n    average packet inter-arrival time = %.6g",(double)average_packet_inter_arrival_time/1000.00);
	average_packet_service_time= (sum_packets_service_time/packets_serviced);
	printf("\n    average packet service time = %.6g\n",(double)average_packet_service_time/1000.00);
	
	
	if((sum_Q1_time/temp_time)==0)
			printf("\n    average number of packets in Q1 = N/A as no packets at this facility");
	else
		printf("\n    average number of packets in Q1 = %.6g", (double)(sum_Q1_time/temp_time));
	if((sum_Q2_time/temp_time)==0)
		printf("\n    average number of packets in Q2 = N/A as no packets at this facility");
	else
		printf("\n    average number of packets in Q2 = %.6g", (double)(sum_Q2_time/temp_time));
	if((sum_S1_time/temp_time)==0)
		printf("\n    average number of packets at S1 = N/A as no packets at this facility");
	else
		printf("\n    average number of packets at S1 = %.6g", (double)(sum_S1_time/temp_time));
	if((sum_S2_time/temp_time)==0)
		printf("\n    average number of packets at S2 = N/A as no packets at this facility");
	else
		printf("\n    average number of packets at S2 = %.6g\n", (double)(sum_S2_time/temp_time));
	
	printf("\n    average time a packet spent in system = %.6g", (double)(sum_system_time/packets_serviced)/1000.00);
	temp1=square_sum_system_time/packets_serviced;
	temp2=(sum_system_time/packets_serviced) * (sum_system_time/packets_serviced);
	standard_deviation= sqrt(temp1 - temp2);
	printf("\n    standard deviation for time spent in system = %.6g\n",(double)standard_deviation/1000.00);
	
	token_drop_prob=(double)dropped_tokens/total_tokens;
	printf("\n    token drop probability = %.6g", token_drop_prob);
	
	packet_drop_prob=(double)dropped_packets/processed_packets;
	printf("\n    packet drop probability = %.6g\n", packet_drop_prob);
	
	pthread_mutex_unlock(&M);
	return;
}

void monitor()
{
	int sig=0;
	My402ListElem* elem=(My402ListElem*) malloc(sizeof(My402ListElem));
	struct timeval print_time;
	while (1)
	{
		sigwait(&set,&sig);
		pthread_mutex_lock(&M);
		//cancel
		
		arrival_thread_finished=1;
		token_thread_finished=1;
		is_cntrlc_pressed=1;
		pthread_cancel(arrival_thr);
		pthread_cancel(token_thr);
		
		while(!(My402ListEmpty(&queue1)))
		{
			packet* pac=(packet*) malloc(sizeof(packet));
			elem=My402ListFirst(&queue1);
			pac=((packet*)(elem->obj));
			processed_packets_inter_arrival_time= processed_packets_inter_arrival_time + pac->lam;
			gettimeofday(&print_time,NULL);

			printf("\n%012.3lfms : p%d removed from Q1",subtract_time(print_time,start_time), pac->packet_name);
			My402ListUnlink(&queue1,elem);
			//unlink q1
		}
		
		while(!(My402ListEmpty(&queue2)))
		{
			packet* pac=(packet*) malloc(sizeof(packet));
			elem=My402ListFirst(&queue2);
			pac=((packet*)(elem->obj));
			processed_packets_inter_arrival_time= processed_packets_inter_arrival_time + pac->lam;
			gettimeofday(&print_time,NULL);
			printf("\n%012.3lfms : p%d removed from Q2",subtract_time(print_time,start_time), pac->packet_name);
			My402ListUnlink(&queue2,elem);
			//unlink q2
		}
		
		pthread_cond_broadcast(&cv);
		pthread_mutex_unlock(&M);
	}
	return;
}


//file read function
int file_read(char file_name[50])
{
	
	FILE *fp; int chk1=0,qlen=0,qlen2=0,len=0,name=1,packet_num=0;
	char file_buf[100]={0};
	fp = fopen(file_name,"r"); // read mode
	double p_lambda=(double)0;
	double p_mu=(double)0,sleep,c; 
	int p_tokens=0;
	My402ListElem* elem=(My402ListElem*) malloc(sizeof(My402ListElem));
	struct timeval previous_time, temp_packet_arrival, print_time,a,b;

	if(is_cntrlc_pressed)
	{
		//printf("\ncntrl c pressed, in file");
		return 1;
	}
	
	if( fp == NULL )
	{
	  perror("\nError while opening the file\n");
	  exit(0);
	}
	
	if(fgets(file_buf,sizeof(file_buf),fp)!=NULL)
	{
		num=atoi(file_buf);
		//printf("\n Num is %d",num);
		if(num<=0)
		{
			printf("\nError in file....exiting");
			err=1;
			exit(0);
		}
	}
	
	//previous_time
	//previous_time = (struct timeval){0};
	gettimeofday(&previous_time,NULL);
	while(packet_num<num)
	{	 
	  
	  packet* pac=(packet*) malloc(sizeof(packet));
	  fscanf(fp,"%lf %d %lf", &p_lambda, &p_tokens, &p_mu);
	  
	  if(p_lambda > 2147483647 || p_lambda < 0 || p_tokens < 0 || p_tokens > 2147483647 || p_mu <0 || p_mu > 2147483647 )
	  {
		printf("\nError in file .. wrong format.. exiting....");
		exit(1);
		  
	  }
	  
	  
	  //usleep((p_lambda)*1000.0);
	  
		gettimeofday(&b,NULL);
		//b
		sleep = (double)(p_lambda)*1000.000000;
		
		if(packet_num==0)
		{
			usleep(sleep);
			start_time=previous_time;
		}
		else
		{
			c=subtract_time(a,b);
			usleep(sleep-c);
		}
		//a
		gettimeofday(&a,NULL);
		
	  pthread_mutex_lock(&M);
	  pac->packet_name=name;
	  //pac->lam=p_lambda;
	  pac->ptoken=p_tokens;
	  pac->muu=p_mu;
	  name++;
	  
	  fgets(file_buf,sizeof(file_buf),fp);
	  len=strlen(file_buf);
	  if(file_buf[len-1] == '\n')
		file_buf[len-1] = '\0';
	  //check conditions here
	  //then append
	        if((pac->ptoken)>B)
			  {
				gettimeofday(&temp_packet_arrival,NULL);
				//gettimeofday(&print_time,NULL);
				pac->lam=subtract_time(temp_packet_arrival,previous_time);
				printf("\n%012.3lfms : p%d arrives, needs %d tokens, inter-arrival time = %lfms, dropped", subtract_time(temp_packet_arrival,start_time), pac->packet_name, pac->ptoken, pac->lam);
				previous_time=temp_packet_arrival;
				dropped_packets++;
				packet_num++;
				processed_packets++;
				processed_packets_inter_arrival_time= processed_packets_inter_arrival_time + pac->lam;
				pthread_mutex_unlock(&M);  
				continue;
			  }
				  
				//gettimeofday(&current_time,NULL);
				gettimeofday(&temp_packet_arrival,NULL);
				//net_packet_arrival_time= p->lam + subtract_time(current_time,previous_time);
				//gettimeofday(&print_time,NULL);
				pac->lam=subtract_time(temp_packet_arrival,previous_time);
				printf("\n%012.3lfms : p%d arrives, needs %d tokens, inter-arrival time = %lfms", subtract_time(temp_packet_arrival,start_time), pac->packet_name, pac->ptoken, subtract_time(temp_packet_arrival,previous_time));
				
				//processed_packets_inter_arrival_time= processed_packets_inter_arrival_time + pac->lam;
				previous_time=temp_packet_arrival;
				chk1=My402ListAppend(&queue1,pac);
				qlen=My402ListLength(&queue1);
				gettimeofday(&print_time,NULL);
				printf("\n%012.3lfms : p%d enters Q1", subtract_time(print_time,start_time), pac->packet_name);
				gettimeofday(&(pac->q1_start_time),NULL);
				
			  if(qlen==1)
			  {
				//printf("\nInside qlen=1 ");
				if((pac->ptoken)<=current_tokens)
				{
					
					current_tokens=current_tokens-(pac->ptoken);
					elem=My402ListFirst(&queue1);
					My402ListUnlink(&queue1,elem);
					gettimeofday(&(pac->q1_end_time),NULL);
					gettimeofday(&print_time,NULL);
					printf("\n%012.3lfms : p%d leaves Q1, time in Q1 = %lfms, token bucket now has %d token", subtract_time(print_time,start_time), pac->packet_name, subtract_time(pac->q1_end_time,pac->q1_start_time), current_tokens);
					
					
					qlen2=My402ListLength(&queue2);
					chk1=My402ListAppend(&queue2,pac);
					gettimeofday(&(pac->q2_start_time),NULL);
					gettimeofday(&print_time,NULL);
					printf("\n%012.3lfms : p%d enters Q2", subtract_time(print_time,start_time), pac->packet_name);
					if(qlen2==0)
						pthread_cond_broadcast(&cv);
					
				}
			  }		
		 packet_num++;
		 processed_packets++;
		 pthread_mutex_unlock(&M);  
	}
	
	fclose(fp);
	return 0;
}


//packet arrival function
void arrival_func()
{
	My402ListElem* elem=(My402ListElem*) malloc(sizeof(My402ListElem));
	int chk1=0,packet_num=0,qlen=0,qlen2=0,name=1;
	double sleep=0.0,c=0.0;
	struct timeval previous_time, temp_packet_arrival, print_time,a,b;
	
	if(is_cntrlc_pressed)
	{
		//printf("\ncntrl c pressed, in pack");
		return;
	}
	
	if(file_flag)
	{
		chk1=file_read(file_name);
		
		
		//mutex
		//append in critical area
		
	}
	
	else
	{
		
		//deterministic mode
		
		//previous_time = (struct timeval){0};
		gettimeofday(&previous_time,NULL);
		//printf("\nprevious time has %lus and %lums",previous_time.tv_sec,previous_time.tv_usec);
		while(packet_num<num)
		{
			
			packet* pac=(packet*) malloc(sizeof(packet));
			//x+y
			
			gettimeofday(&b,NULL);
			//b
			sleep = (double)(lambda_global)*1000.000000;
			
			if(packet_num==0)
			{
				usleep(sleep);
				start_time=previous_time;
			}
			else
			{
				c=subtract_time(a,b);
				usleep(sleep-c);
			}
			//a
			gettimeofday(&a,NULL);
			pthread_mutex_lock(&M);  
			pac->packet_name=name;
			//pac->lam=lambda_global;
			pac->ptoken=P;
			pac->muu=mu_global;
			
			name++;
			
			
			  if((pac->ptoken)>B)
			  {
				gettimeofday(&temp_packet_arrival,NULL);
				//gettimeofday(&print_time,NULL);
				pac->lam=subtract_time(temp_packet_arrival,previous_time);
				printf("\n%012.3lfms : p%d arrives, needs %d tokens, inter-arrival time = %lfms, dropped", subtract_time(temp_packet_arrival,start_time), pac->packet_name, pac->ptoken, pac->lam);
				previous_time=temp_packet_arrival;
				dropped_packets++;
				processed_packets++;
				packet_num++;
				processed_packets_inter_arrival_time= processed_packets_inter_arrival_time + pac->lam;
				pthread_mutex_unlock(&M);  
				continue;
			  }
				gettimeofday(&temp_packet_arrival,NULL);
				//printf("\narrival time has %lus and %lums",temp_packet_arrival.tv_sec,temp_packet_arrival.tv_usec);
				gettimeofday(&print_time,NULL);
				pac->lam=subtract_time(temp_packet_arrival,previous_time);
				printf("\n%012.3lfms : p%d arrives, needs %d tokens, inter-arrival time = %lfms", subtract_time(temp_packet_arrival,start_time),pac->packet_name, pac->ptoken, pac->lam);
				
				//processed_packets_inter_arrival_time= processed_packets_inter_arrival_time + pac->lam;
				previous_time=temp_packet_arrival;
				chk1=My402ListAppend(&queue1,pac);
				qlen=My402ListLength(&queue1);
				gettimeofday(&print_time,NULL);
				printf("\n%012.3lfms : p%d enters Q1", subtract_time(print_time,start_time), pac->packet_name);
				gettimeofday(&(pac->q1_start_time),NULL);
				  
			  if(qlen==1)
			  {
				//printf("\nInside qlen=1 ");
				if((pac->ptoken)<=current_tokens)
				{
					
					current_tokens=current_tokens-(pac->ptoken);
					elem=My402ListFirst(&queue1);
					My402ListUnlink(&queue1,elem);
					gettimeofday(&(pac->q1_end_time),NULL);
					gettimeofday(&print_time,NULL);
					printf("\n%012.3lfms : p%d leaves Q1, time in Q1 = %lfms, token bucket now has %d token", subtract_time(print_time,start_time), pac->packet_name, subtract_time(pac->q1_end_time,pac->q1_start_time), current_tokens);
					
					qlen2=My402ListLength(&queue2);
					chk1=My402ListAppend(&queue2,pac);
					gettimeofday(&(pac->q2_start_time),NULL);
					gettimeofday(&print_time,NULL);
					printf("\n%012.3lfms : p%d enters Q2", subtract_time(print_time,start_time), pac->packet_name);
					if(qlen2==0)
						pthread_cond_broadcast(&cv);
				}
			  }		
		 packet_num++;
		 processed_packets++;
		 pthread_mutex_unlock(&M);
		}
	}
	
	arrival_thread_finished=1;
	return ;
}

//token bucket function
void token_func()
{
	//token
	int chk1=0,i=0,qlen2=0;
	double k=(double)0.0;
	My402ListElem* elem=(My402ListElem*) malloc(sizeof(My402ListElem));
	packet* pac=(packet*) malloc(sizeof(packet));
	struct timeval print_time, previous_time;
	
	if(is_cntrlc_pressed)
	{
		//printf("\ncntrl c pressed, in tok");
		return;
	}
	
	
	while(1)
	{
		
		//sleep
		gettimeofday(&previous_time,NULL);
		k=(double)(r_global*1000.000000);
		usleep(k);
		
		pthread_mutex_lock(&M);
		//update current_tokens with if
		if(current_tokens<B)
		{
			current_tokens=current_tokens+1;
			total_tokens++;
			if(total_tokens==1)
			{
				start_time=previous_time;
			}
			gettimeofday(&print_time,NULL);
			printf("\n%012.3lfms : token t%d arrives, token bucket now has %d token", subtract_time(print_time,start_time), total_tokens, current_tokens);
		}
		
		else
		{
			total_tokens++;
			
			gettimeofday(&print_time,NULL);
			printf("\n%012.3lfms : token t%d arrives, dropped", subtract_time(print_time,start_time), total_tokens);
			dropped_tokens++;
		}
		
		
		
		//check q1 tokens then q1->q2
		while(!(My402ListEmpty(&queue1)))
		{
			elem=My402ListFirst(&queue1);
			pac=((packet*)(elem->obj));
			if(pac->ptoken<=current_tokens)
			{
				//pthread_mutex_lock(&M);
				current_tokens=current_tokens-(pac->ptoken);
				elem=My402ListFirst(&queue1);
				My402ListUnlink(&queue1,elem);
				gettimeofday(&(pac->q1_end_time),NULL);
				gettimeofday(&print_time,NULL);
				printf("\n%012.3lfms : p%d leaves Q1 token bucket now has %d token", subtract_time(print_time,start_time),pac->packet_name, current_tokens);
				
				qlen2=My402ListLength(&queue2);
				chk1=My402ListAppend(&queue2,pac);
				gettimeofday(&(pac->q2_start_time),NULL);
				gettimeofday(&print_time,NULL);
				printf("\n%012.3lfms : p%d enters Q2", subtract_time(print_time,start_time), pac->packet_name);
				if(qlen2==0)
						pthread_cond_broadcast(&cv);
				
				i++;
				//pthread_mutex_unlock(&M);  
			}
			else
			{
				break;
			}
		}
		pthread_mutex_unlock(&M);  
		if(arrival_thread_finished==1 && My402ListEmpty(&queue1))
		{
			//printf("\nFinished packets ");
			//printf("\nTotal tokens =%d", total_tokens);
			//printf("\ndropped tokens =%d",dropped_tokens);
			token_thread_finished=1;
			pthread_cond_broadcast(&cv);
			break;
		}
		
	}
	return;
}


//server 1 function
void server1_func()
{
	//pthread_sigmask(SIG_BLOCK, &set, 0);
	packet* pac=(packet*) malloc(sizeof(packet));
	int i=0;
	//while loop for cond_wait
	double final_packet_time, total_service, server_sleep;
	struct timeval serv_start, serv_end, print_time;
	My402ListElem* elem=(My402ListElem*) malloc(sizeof(My402ListElem));
	
	//printf("\nEntered S1");
	while(1)
	{
		 if((My402ListEmpty(&queue1) &&  My402ListEmpty(&queue2) && token_thread_finished==1) || is_cntrlc_pressed)
			{
				end_is_reached=1;
				//printf("\nReached end of S1");
				//pthread_mutex_unlock(&M);
				break;
			}
		
		pthread_mutex_lock(&M);  
		
			
		while(My402ListEmpty(&queue2) && !is_cntrlc_pressed)
		{
			if(token_thread_finished==1)
			{
				//printf("\ninside s1 while");
				break;
			}
			pthread_cond_wait(&cv, &M);	
		}
		
			
		pthread_mutex_unlock(&M);  
		
		if(!(My402ListEmpty(&queue2)))
		{
			pthread_mutex_lock(&M);  
			elem=My402ListFirst(&queue2);
			pac=((packet*)(elem->obj));
			
			//time calculate
			gettimeofday(&(pac->q2_end_time),NULL);
			gettimeofday(&print_time,NULL);
			printf("\n%012.3lfms : p%d leaves Q2, time in Q2 = %lfms", subtract_time(print_time,start_time), pac->packet_name,subtract_time(pac->q2_end_time,pac->q2_start_time));
			My402ListUnlink(&queue2,elem);
			gettimeofday(&serv_start,NULL);
			gettimeofday(&print_time,NULL);
			printf("\n%012.3lfms : p%d begins service at S1, requesting %lfms of service", subtract_time(print_time,start_time), pac->packet_name,pac->muu);
			
			if(My402ListEmpty(&queue2))
			{
				//all_q2_finished=1;
				pthread_cond_broadcast(&cv);
			}
			
			pthread_mutex_unlock(&M);  
			server_sleep=(double)((pac->muu)*1000.000000);
			usleep(server_sleep);
			
			pthread_mutex_lock(&M);  
			gettimeofday(&serv_end,NULL);
			total_service=subtract_time(serv_end,serv_start);
			final_packet_time= subtract_time(pac->q2_end_time,pac->q1_start_time)+ total_service;
			gettimeofday(&print_time,NULL);
			printf("\n%012.3lfms : p%d departs from S1, service time = %lfms, time in system = %lfms", subtract_time(print_time,start_time), pac->packet_name, total_service,final_packet_time);
			//check_variable=1;
			
			packets_serviced++;
			
				sum_packets_inter_arrival_time=sum_packets_inter_arrival_time+pac->lam;
				sum_packets_service_time= sum_packets_service_time+total_service;
				
				sum_Q1_time= sum_Q1_time + subtract_time(pac->q1_end_time,pac->q1_start_time);
				sum_Q2_time= sum_Q2_time + subtract_time(pac->q2_end_time,pac->q2_start_time);
				sum_S1_time= sum_S1_time + total_service;
				
				sum_system_time= sum_system_time + final_packet_time;
				square_sum_system_time= square_sum_system_time + (final_packet_time*final_packet_time);
			
			pthread_mutex_unlock(&M);  
			i++;
		}
		if((My402ListEmpty(&queue1) &&  My402ListEmpty(&queue2) && token_thread_finished==1) || is_cntrlc_pressed)
			{
				end_is_reached=1;
				//printf("\nReached end of S1");
				//pthread_mutex_unlock(&M);
				break;
			}
		
	}
	
	//printf("\nOut of the server s1 for good!!");
	
	return ;
}

//server 2 function
void server2_func()
{
	//pthread_sigmask(SIG_BLOCK, &set, 0);
	packet* pac=(packet*) malloc(sizeof(packet));
	int i=0;
	//while loop for cond_wait
	double final_packet_time, total_service, server_sleep;
	struct timeval serv_start, serv_end, print_time;
	My402ListElem* elem=(My402ListElem*) malloc(sizeof(My402ListElem));
	
	//printf("\nEntered S2");
	while(1)
	{
		 if((My402ListEmpty(&queue1) &&  My402ListEmpty(&queue2) && token_thread_finished==1) || is_cntrlc_pressed)
			{
				end_is_reached=1;
				//printf("\nReached end of S2");
				//pthread_mutex_unlock(&M);
				break;
			}
		 
		
		/* if(is_cntrlc_pressed==1)
		{
			break;
		} */
		
		pthread_mutex_lock(&M); 

		while(My402ListEmpty(&queue2) && !is_cntrlc_pressed)
		{
			if(token_thread_finished==1)
			{
				//printf("\ninside s2 while");
				break;
			}
			pthread_cond_wait(&cv, &M);	
		}
		
		pthread_mutex_unlock(&M);  
		
		if(!(My402ListEmpty(&queue2)))
		{
			pthread_mutex_lock(&M);  
			elem=My402ListFirst(&queue2);
			pac=((packet*)(elem->obj));
			
			//time calculate
			gettimeofday(&(pac->q2_end_time),NULL);
			gettimeofday(&print_time,NULL);
			printf("\n%012.3lfms : p%d leaves Q2, time in Q2 = %lfms", subtract_time(print_time,start_time), pac->packet_name,subtract_time(pac->q2_end_time,pac->q2_start_time));
			My402ListUnlink(&queue2,elem);
			gettimeofday(&serv_start,NULL);
			gettimeofday(&print_time,NULL);
			printf("\n%012.3lfms : p%d begins service at S2, requesting %lfms of service", subtract_time(print_time,start_time), pac->packet_name,pac->muu);
			
			if(My402ListEmpty(&queue2))
			{
				//all_q2_finished=1;
				pthread_cond_broadcast(&cv);
			}
			pthread_mutex_unlock(&M);  
			server_sleep=(double)((pac->muu)*1000.000000);
			usleep(server_sleep);
			
			pthread_mutex_lock(&M);  
			gettimeofday(&serv_end,NULL);
			total_service=subtract_time(serv_end,serv_start);
			final_packet_time= subtract_time(pac->q2_end_time,pac->q1_start_time)+ total_service;
			gettimeofday(&print_time,NULL);
			printf("\n%012.3lfms : p%d departs from S2, service time = %lfms, time in system = %lfms", subtract_time(print_time,start_time), pac->packet_name, total_service,final_packet_time);
			
			
			packets_serviced++;
			
				sum_packets_inter_arrival_time=sum_packets_inter_arrival_time+pac->lam;
				sum_packets_service_time= sum_packets_service_time+total_service;
				
				sum_Q1_time= sum_Q1_time + subtract_time(pac->q1_end_time,pac->q1_start_time);
				sum_Q2_time= sum_Q2_time + subtract_time(pac->q2_end_time,pac->q2_start_time);
				sum_S2_time= sum_S2_time + total_service;
				
				sum_system_time= sum_system_time + final_packet_time;
				square_sum_system_time= square_sum_system_time + (final_packet_time*final_packet_time);
			
			pthread_mutex_unlock(&M);  
			i++;
			
		}
		
		if((My402ListEmpty(&queue1) &&  My402ListEmpty(&queue2) && token_thread_finished==1) || is_cntrlc_pressed)
			{
				end_is_reached=1;
				//printf("\nReached end of S2");
				//pthread_mutex_unlock(&M);
				break;
			}
		
	}
	
	//printf("\nOut of the server s2 for good!!");
	
	return ;
}


