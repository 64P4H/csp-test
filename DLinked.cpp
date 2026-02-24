#include "DLinked.hpp"

#ifdef VERB
#include <stdio.h>
#endif

#include <stdlib.h>
#include <omp.h>

DLinkedElem::DLinkedElem(DLinkedElem* prev,
                         int data) :
    prev(prev), next(NULL), data(data) {}
DLinkedElem::DLinkedElem(int data) :
    prev(NULL), next(NULL), data(data) {}

/**
 * \brief Функция вычисления количества бит в значении bit
 * 
 * Данная реализация предполагает анализ каждого из 32 битов, что  можно легко оптимизировать. Однако, чтобы сохранить прямолинейность решения, было решено оставить так. Так, можно использовать popcount или 32-popcount при подсчете 0, но в таком случае фактически всегда будут подсчитываться 1, а не указанное значение.
 * 
 * \param[in] bit 0 или 1, подсчитываемый в this.data.
 * \return Результат вычисления.
 */
int DLinkedElem::countBits(int bit) {
  int res=0;
  int num = data;
  bit = bit&1;

  for(int i=0; i<32; ++i) {
    if((num&1) == bit){
      res++;
    }
    num >>= 1;
  }

  return res;
}

DLinkedList::DLinkedList(int num) {
  if(num) {
    num--;
    first = last = new DLinkedElem(rand());
  }
  while(num) {
    num--;
    last = new DLinkedElem(last, rand());
    last->prev->next = last;
  }
}
DLinkedList::~DLinkedList() {
  DLinkedElem* next;

  while(first != NULL) {
    next = first->next;
    delete first;
    first = next;
  }
  last = NULL;
}

/**
 * \brief Функция вычисления 0 и 1 в два потока с разных направлений до пересечения
 * 
 * Данная функция получила множество реализаций в попытках достичь синхронизации между потоками. 
 * 1. Сперва реализация была основана на MPI, который, как оказалось, создает процессы, а не потоки, которые ввиду одновременного запуска генераторов создавали одинаковые массивы, но процессорная архитектура не позволяла работать над общим связным списком и тем более в роли потоков. От нее было решено полностью отказаться, но ее часть сохранилась неиспользуемым файлом main_mpi.cpp.
 * 2. Вторая реализация предполагала использование пересечений с помощью истории переходов. Такая реализация была зависима от длины этой истории и от непосредственной скорости выполнения программы аппаратной частью компьютера, из-за чего режим с замедлением программы работал значительно стабильнее, чем без. Данная реализация активируется при заданной директиве HIST.
 * 3. Третья реализация добавляла к этой истории отдельный флаг, позволяющий отслеживать, если другой поток остановился, потому что нашел пересечение. В таком случае, если случится так, что один поток перескочит другой, то один поток сможет сообщить другому о пересечении. Эта реализация от предыдущей отличается в первую очередь структурой проверок. Когда в предыдущей сперва уточняется поток, а потом цикл, в этой реализации уточнение потока происходит каждую итерацию цикла. Это очень замедлит программу, но это необходимо, чтобы сохранить подобие параллельности и чтобы синхронно обновлять общие значения. Данная реализация компилируется в режиме "по умолчанию" (без дополнительных флагов). 
 * 4. Четвертая реалиция представляет из себя максимально прямолинейную реализацию, в которой все пространство параллельного выполнения выделена как "критическая", на чем потоки будут выполняться исключительно последовательно, то есть сам смысл распараллеливания на основе потоков пропадает. Пропадание параллельности заметно в большой разнице подсчетов количества 1 и 0. Данная реализация активируется при заданной директиве CRIT.
 * Не смотря на очевидную работоспособность реализации CRIT, даже она не выдает 100% идеальной работы. Никакие специальные команды синхронизации параметров не помогают. Для проверки выполнения была добавлена директива VERB, создающая подробный вывод во время выполнения, а также был выполнен подсчет пройденных элементов, возвращаемый функцией.
 * 
 * \param[out] res0 Количество подсчитанных 0 бит.
 * \param[out] res1 количество подсчитанных 1 бит.
 * \return Количество обработанных элементов.
 */
int DLinkedList::threadMagic(int* res0,
                             int* res1){
  int tid;
  int num = 0;
  DLinkedElem* cur1 = last;
  DLinkedElem* cur0 = first;

#ifdef HIST
  DLinkedElem* cur1e;
  DLinkedElem* cur0e;
  DLinkedElem* cur1ee;
  DLinkedElem* cur0ee;

  if(first==last){
    *res0 = first->countBits(0);
    return 1;
  }

// firstprivate позволяет инициализировать значения res, иначе он потеряет указатели
#pragma omp parallel \
  firstprivate(tid, res0, res1) \
  shared(cur0, cur1, cur0e, cur1e, cur0ee, cur1ee) \
  num_threads(2) \
  reduction(+:num)
  {
    tid = omp_get_thread_num();
    if(tid) { //thread 1
      while((cur0 != cur1)
            and (cur0e != cur1) 
            and (cur0ee != cur1)) {
        #ifdef VERB
        printf("<%i< %p %p %p %p %i %i\n",
               tid, cur1, cur0, cur0e, cur0ee, *res1, num);
        #endif
        *res1 += cur1->countBits(tid);
        // #pragma omp critical ???
        cur1ee = cur1e;
        cur1e = cur1;
        cur1 = cur1->prev;
        num++;
      }
    }
    else { //thread 0
      while((cur0 != cur1)
            and (cur0 != cur1e) 
            and (cur0 != cur1ee)) {
        #ifdef VERB
        printf("<%i< %p %p %p %p %i %i\n",
               tid, cur0, cur1, cur1e, cur1ee, *res0, num);
        #endif
        *res0 += cur0->countBits(tid);
        cur0ee = cur0e;
        cur0e = cur0;
        cur0 = cur0->next;
        num++;
      }
    }
    #pragma omp barrier
  }

#else
#ifdef CRIT

  int flag = 0;

  if(first==last){
    *res0 = first->countBits(0);
    return 1;
  }

#pragma omp parallel \
    firstprivate(tid, res0, res1) \
    shared(cur0, cur1, flag) \
    num_threads(2) \
    reduction(+:num)
  {
    tid = omp_get_thread_num();
    while(1) {
      #pragma omp critical
      {
        if(flag != 2) {
          if(flag == 1) {
            flag = 2;
          }
          if(tid) {
            #ifdef VERB
            printf(">%i> %p %p %i %i\n",
                   tid, cur1, cur0, *res1, num);
            #endif
            *res1 += cur1->countBits(tid);
            cur1 = cur1->prev;
            num++;
          }
          else {
            #ifdef VERB
            printf("<%i< %p %p %i %i\n",
                   tid, cur0, cur1, *res0, num);
            #endif
            *res0 += cur0->countBits(tid);
            cur0 = cur0->next;
            num++;
          }
          if(cur0 == cur1)
            flag=1;
          #pragma omp flush (cur0, cur1, flag)
          // не помогает
        }
      }
      if(flag)
        break;
    }
    #pragma omp barrier
  }

#else //DEFAULT

  int flag = 0;
  DLinkedElem* cur1e;
  DLinkedElem* cur0e;

  if(first==last){
    *res0 = first->countBits(0);
    return 1;
  }

#pragma omp parallel \
    firstprivate(tid, res0, res1) \
    shared(cur0, cur1, cur0e, cur1e, flag) \
    num_threads(2) \
    reduction(+:num)
  {
    tid = omp_get_thread_num();
    while(1) {
      #pragma omp critical
      {
        if(tid)
          cur1e = cur1->prev;
        else
          cur0e = cur0->next;
        if(cur0 == cur1e or cur0e == cur1)
          flag=1;
        else if(cur0 == cur1)
          flag=2;
        // #pragma omp flush (cur0, cur0e, cur1, cur1e, flag)
        // не помогает
      }
      if(flag==1)
        break;
      if(tid){
        #ifdef VERB
        printf(">%i> %p %p %p %i %i\n",
               tid, cur1, cur0, cur0e, *res1, num);
        #endif
        *res1 += cur1->countBits(tid);
        cur1 = cur1e;
        num++;
      }
      else {
        #ifdef VERB
        printf("<%i< %p %p %p %i %i\n",
               tid, cur0, cur1, cur1e, *res0, num);
        #endif
        *res0 += cur0->countBits(tid);
        cur0 = cur0e;
        num++;
      }

      if(flag==2)
        break;
      
    }
    #pragma omp barrier
  }

// DEFAULT
#endif // CRIT
#endif // HIST

  return num;
}
