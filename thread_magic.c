#include "thread_magic.h"

#include <pthread.h>

#include "dlinked.h"

/**
 * \brief Функция вычисления 0 и 1 в два потока с разных направлений до пересечения
 * 
 * Данная функция получила множество реализаций в попытках достичь синхронизации между потоками. После предложений по модификации был принят ряд решений:
 * 1. Выделить функцию работы с потоками отдельно от двусвязного списка.
 * 2. Использовать библиотеку pthreads.h для ручного создания потоков и работы с mutex.
 * 3. Упорядочить проверку работы потоков так, чтобы при пересечении, общий элемент просчитывался одним конкретный заранее определенным потоком.
 * 4. Использовать дополнительный mutex, чтобы по нему запустить потоки одновременно (иначе задержки в создании второго потока хватает, чтобы первый выполнил всю задачу).
 * 5. Дополнительно было решено перейти на Си. Без особых причин, просто с его контролем лично мне думать проще. 
 * 
 * \param arg Передает thread_arg-параметр, включающий в себя индекс потока, указатель на результат, указатели на указатели курсоров потоков, а также указатели на mutex.
 * \return Количество обработанных элементов.
 */

void* thread_magic(void* args) {
  thread_arg* t_arg = (thread_arg*)args;
  int num = 0;

  int* res = t_arg->res;
  dlinked_node** cur0 = t_arg->cur0;
  dlinked_node** cur1 = t_arg->cur1;
  pthread_mutex_t* mutex = t_arg->mutex;

  pthread_mutex_lock(t_arg->start_mutex); // для приближения
  pthread_mutex_unlock(t_arg->start_mutex); // одновременного начала

  if(t_arg->ind) {
    while(1) {
      pthread_mutex_lock(mutex);
      if((*cur0) == (*cur1))
        break;

      *res += count_bits_in_node((*cur1), 1);
      ++num;

      (*cur1) = (*cur1)->prev;
      pthread_mutex_unlock(mutex);
    }
  }
  else {
    while(1) {
      pthread_mutex_lock(mutex);
      *res += count_bits_in_node((*cur0), 0);
      ++num;

      if((*cur0) == (*cur1))
        break;
      (*cur0) = (*cur0)->next;
      pthread_mutex_unlock(mutex);
    }
  }
  pthread_mutex_unlock(mutex);

  return (void*) num;
}
