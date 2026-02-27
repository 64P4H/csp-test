#include "thread_magic.h"

#include <pthread.h>
#include <stdlib.h>

#include "dlinked.h"

// #include <stdio.h>

/**
 * \brief Функция вычисления 0 и 1 в два потока с разных направлений до пересечения
 * 
 * \param arg Передает thread_arg-параметр, включающий в себя индекс потока, указатель на результат, указатель на указатель списка, а также указатели на барьер и мьютекс.
 * \return Количество обработанных элементов.
 */
void* thread_magic(void* args) {
  thread_arg* t_arg = (thread_arg*)args;
  dlinked_node* _node = NULL;
  dlinked_list** list = t_arg->list;
  pthread_mutex_t* mutex = t_arg->mutex;

  int num = 0; // Количество обработанных элементов
  int _res = 0;
  
  // Для приближения одновременного начала работы потоков
  pthread_barrier_wait(t_arg->start_barrier);

  if(t_arg->ind) {
    while(1) {
      pthread_mutex_lock(mutex);
      _node = (*list)->last;
      if(_node == NULL)
        break;

      (*list)->last = _node->prev;
      if((*list)->last) {
        (*list)->last->next = NULL;
      }
      else {
        (*list)->first = NULL;
      }

      pthread_mutex_unlock(mutex);

      _res += count_bits_in_node(_node, 1);
      ++num;
      free(_node);
    }
  }
  else {
    while(1) {
      pthread_mutex_lock(mutex);
      _node = (*list)->first;
      if(_node == NULL)
        break;

      (*list)->first = _node->next;
      if((*list)->first) {
        (*list)->first->prev = NULL;
      }
      else {
        (*list)->last = NULL;
      }

      pthread_mutex_unlock(mutex);

      _res += count_bits_in_node(_node, 0);
      ++num;
      free(_node);
    }
  }
  pthread_mutex_unlock(mutex);

  (*t_arg->res) = _res;

  return (void*) num;
}
