//warmup2.h header file
//contains all functions and variable declarations

#ifndef warmup2_h
#define warmup2_h

#include "my402list.h"

extern int B;
extern int current_tokens;
extern pthread_mutex_t M;
extern pthread_cond_t cv;
extern double lambda_global;
extern double mu_global;
extern double r_global;
extern int num ;
extern int P;
extern char file_name[50];
extern int file_flag;
extern int arrival_thread_finished;
extern int token_thread_finished;
extern int end_is_reached;
extern int dropped_tokens;
extern int dropped_packets;
extern int total_tokens;
extern int packets_serviced;
extern int is_cntrlc_pressed;
extern int processed_packets;
extern double sum_packets_inter_arrival_time;
extern double processed_packets_inter_arrival_time;
extern double sum_packets_service_time;
extern double sum_Q1_time;
extern double sum_Q2_time;
extern double sum_S1_time;
extern double sum_S2_time;
extern double sum_system_time;
extern double square_sum_system_time;
extern sigset_t set;
extern int err;

extern My402List queue1;
extern My402List queue2;

typedef struct packet{
	int packet_name;
	double lam;
	double muu;
	int ptoken;
	
	struct timeval q1_start_time;
	struct timeval q1_end_time;
	struct timeval q2_start_time;
	struct timeval q2_end_time;
}packet;
extern pthread_t arrival_thr;
extern pthread_t token_thr;
extern pthread_t server1_thr;
extern pthread_t server2_thr;
extern pthread_t thr;
extern double subtract_time(struct timeval, struct timeval);
extern int file_read(char *);
extern void arrival_func();
extern void token_func();
extern void server1_func();
extern void server2_func();
extern void monitor();
extern void display_stats(double );
extern struct timeval start_time;

#endif /*_warmup2_h_*/
