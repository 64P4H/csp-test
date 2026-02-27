#ifndef THREAD_MAGIC
#define THREAD_MAGIC

#include <pthread.h>

#include "dlinked.h"

/** \brief Структура-аргумент потоковой функции */
typedef struct _thread_arg{
  /** \brief Индекс потока */
  int ind;
  /** \brief Результат выполняемого потока */
  int* res;
  /** \brief Обрабатываемый список */
  dlinked_list** list;
  /** \brief Барьер начала работы потоков */
  pthread_barrier_t* start_barrier;
  /** \brief Мьютекс обработки защищаемых данных */
  pthread_mutex_t* mutex;
} thread_arg;

void* thread_magic(void* args);

#endif
