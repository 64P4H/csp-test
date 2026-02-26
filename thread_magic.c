#include "thread_magic.h"

#include <pthread.h>

#include "dlinked.h"

// #include <stdio.h>

/**
 * \brief Функция вычисления 0 и 1 в два потока с разных направлений до пересечения
 * 
 * \param arg Передает thread_arg-параметр, включающий в себя индекс потока, указатель на результат, указатели на указатели курсоров потоков, а также указатели на mutex.
 * \return Количество обработанных элементов.
 */
void* thread_magic(void* args) {
  thread_arg* t_arg = (thread_arg*)args;
  int num = 0; // Количество обработанных элементов

  int _res = 0;
  dlinked_node* _node = NULL;
  int _stop = 0;
  int* stop = t_arg->stop;
  dlinked_node** cur0 = t_arg->cur0;
  dlinked_node** cur1 = t_arg->cur1;
  pthread_mutex_t* mutex = t_arg->mutex;
  
  (*stop) = _stop = ((*cur0) == (*cur1));
  _node = *cur0;
  // Для приближения одновременного начала работы потоков
  pthread_barrier_wait(t_arg->start_barrier);

  if(t_arg->ind) {
    while(1) {
      if(_stop)
        break;

      pthread_mutex_lock(mutex);
      _node = *cur1;
      (*cur1) = (*cur1)->prev;
      (*stop) = _stop = ((*cur0) == (*cur1)) | (*stop);
      pthread_mutex_unlock(mutex);

      _res += count_bits_in_node(_node, 1);
      ++num;
      // printf(">1> %i: %p %p %p %i\n",
      //   num, *cur0, *cur1, _node, _stop);
    }
  }
  else {
    while(1) {
      _res += count_bits_in_node(_node, 0);
      ++num;

      pthread_mutex_lock(mutex);
      (*cur0) = (*cur0)->next;
      _node = *cur0;
      (*stop) = _stop = ((*cur0) == (*cur1)) | (*stop);
      pthread_mutex_unlock(mutex);

      if(_stop)
        break;

      // printf(">0> %i: %p %p %p %i\n",
      //   num, *cur0, *cur1, _node, _stop);
    }
  }

  *(t_arg->res) = _res;

  pthread_exit((void*) num);
}

























// void* thread_magic(void* args) {
//   thread_arg* t_arg = (thread_arg*)args;
//   int num = 0; // Количество обработанных элементов

//   //int* res = t_arg->res; // Индивидуальный результат, сохраняемый вовне
//   int _res = 0;
//   dlinked_node* _node = NULL;
//   int _stop = 0;
//   dlinked_node** cur0 = t_arg->cur0; // Нулевой курсор, сохраняемый вовне
//   dlinked_node** cur1 = t_arg->cur1; // Первый курсор, сохраняемый вовне
//   pthread_mutex_t* mutex = t_arg->mutex; // Мьютекс для защищаемых данных

//   // Для приближения одновременного начала работы потоков
//   pthread_barrier_wait(t_arg->start_barrier);

//   if(t_arg->ind) {
//     while(1) {
//       pthread_mutex_lock(mutex);
//       _stop = ((*cur0) == (*cur1));
//       _node = *cur1;
//       (*cur1) = (*cur1)->prev;
//       pthread_mutex_unlock(mutex);

//       printf(">1> %i: %p %p %p %i\n",
//         num, *cur0, *cur1, _node, _stop);
//       if(_stop) break;

//       _res += count_bits_in_node(_node, 1);
//       ++num;

//       // (*cur1) = (*cur1)->prev;
//       // pthread_mutex_unlock(mutex);
//     }
//     pthread_mutex_lock(mutex);
//     (*cur1) = (*cur1)->next;
//     pthread_mutex_unlock(mutex);
//   }
//   else {
//     while(1) {
//       pthread_mutex_lock(mutex);
//       _stop = ((*cur0) == (*cur1));
//       _node = *cur0;
//       (*cur0) = (*cur0)->next;
//       pthread_mutex_unlock(mutex);
//       _res += count_bits_in_node(_node, 0);
//       ++num;

//       // _stop = ((*cur0) == (*cur1));
//       printf(">0> %i: %p %p %p %i\n",
//         num, *cur0, *cur1, _node, _stop);
//       if(_stop) break;
//       // (*cur0) = (*cur0)->next;
//       // pthread_mutex_unlock(mutex);
//     }
//     pthread_mutex_lock(mutex);
//     (*cur0) = (*cur0)->prev;
//     pthread_mutex_unlock(mutex);
//   }
//   // pthread_mutex_unlock(mutex);

//   *(t_arg->res) = _res;

//   return (void*) num;
// }
