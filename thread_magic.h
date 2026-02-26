#ifndef THREAD_MAGIC
#define THREAD_MAGIC

#include <pthread.h>

#include "dlinked.h"

typedef struct _thread_arg{
  int ind;
  int* res;
  dlinked_node** cur0; // = list->first
  dlinked_node** cur1; // = list->last
  pthread_mutex_t* start_mutex;
  pthread_mutex_t* mutex;
} thread_arg;

void* thread_magic(void* args);

#endif
