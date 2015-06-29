#include "defs.h"
/* compile with -D_POSIX_PTHREAD_SEMANTICS on nunki */
typedef struct foo {
  int x, y;
} my_state;

my_state state;
sigset_t set;
pthread_mutex_t m=PTHREAD_MUTEX_INITIALIZER;

void display(my_state *ptr) {
  printf("\nx = %1d\n", ptr->x);
  usleep(1000);
  printf("y = %1d\n", ptr->y);
}

void *monitor(void *arg) {
  int sig=0;
  for (;;) {
    sigwait(&set, &sig);
    pthread_mutex_lock(&m);
    display(&state);
	//
	//pthread cancel
    pthread_mutex_unlock(&m);
  }
  return 0;
}

void update_state(my_state *ptr) {
  ptr->x++;
  usleep(100000);
  ptr->y++;
}

void compute_more() { usleep(100000); }

void long_running_proc() {
  int i=0;
  for (i=0; i < 100; i++) {
    pthread_mutex_lock(&m);
    update_state(&state);
    pthread_mutex_unlock(&m);
    compute_more();
  }
}

int main(int argc, char *argv[]) {
  pthread_t thr;
  state.x = state.y = 0;
  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  sigprocmask(SIG_BLOCK, &set, 0);
  pthread_create(&thr, 0, monitor, 0);
  long_running_proc();
  return 0;
}
