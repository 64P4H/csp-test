/**
 * \mainpage
 * 
 * Данная программа выполнена Туром Тимофеев в качестве тестового задания. Задание было сформулировано следующим образом:
 * 1. Сформировать в памяти двусвязный список определенного размера, содержащий случайные значения, которые следует получить с генератора случайных чисел.
 * 2. Запускать два потока (threads).
 * 2.1 Поток №1: начиная с головы списка будет подсчитывать количество 
 нулевых битов в значениях элементов и количество пройденных 
 элементов, освобождая учтённый элемент сразу после учёта.
 * 2.2 Поток №2: начиная с хвоста списка будет подсчитывать количество 
 единичных битов в значениях элементов и количество пройденных 
 элементов, освобождая учтённый элемент сразу после учёта.
 * 3. По окончании элементов списка приложение будет выводить 
 результаты подсчёта.
 * 4. Нужно обеспечить однократную обработку каждого элемента списка 
 (каждый должен быть учтён, но только один раз, одним из потоков).
 * 5. Желательно, чтобы в потоках работали не две разные функции, а 
 одна, принимающая особенности работы через аргументы.
 * 
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

#include "dlinked.h"
#include "thread_magic.h"

#define NUM 100

int main(){
  srand(time(NULL));

  int r;
  dlinked_list* list;
  // dlinked_node* node;

  pthread_t threads[2];
  int res0 = 0;
  int res1 = 0;
  dlinked_node* cur0;
  dlinked_node* cur1;
  pthread_mutex_t start_mutex;
  pthread_mutex_t mutex;
  thread_arg t_args[2] = {
    {0, &res0, &cur0, &cur1, &start_mutex, &mutex},
    {1, &res1, &cur0, &cur1, &start_mutex, &mutex}
  };

  int nums[2] = {0, 0};


  list = malloc(sizeof(dlinked_list));
  if(list == NULL) {
    exit(-1);
  }
  r = init_dlinked_list(list, NUM);
  if(r){
    fprintf(stderr, "init list malloc error\n");
    exit(-1);
  }
  cur0 = list->first;
  cur1 = list->last;

  pthread_mutex_init(&start_mutex, NULL);
  pthread_mutex_init(&mutex, NULL);

  pthread_mutex_lock(&start_mutex); // для единовременного начала
  pthread_create(threads, NULL, thread_magic, t_args);
  pthread_create(threads+1, NULL, thread_magic, t_args+1);
  pthread_mutex_unlock(&start_mutex);

  printf("All threads created\n");

  r = pthread_join(threads[0], (void**)nums);
  if(r){
    fprintf(stderr, "error on thread0 join\n");
    exit(-1);
  }
  r = pthread_join(threads[1], (void**)(nums+1));
  if(r){
    fprintf(stderr, "error on thread1 join\n");
    exit(-1);
  }

  printf("Main program ended\n");
  printf("Zero bits: %i\n", res0);
  printf("One bits: %i\n", res0);
  printf("List elements: %i\n", NUM);
  printf("By thread 0: %i\n", nums[0]);
  printf("By thread 1: %i\n", nums[1]);
  printf("Intersection addresses: %p %p\n", cur0, cur1);

  pthread_mutex_destroy(&start_mutex);
  pthread_mutex_destroy(&mutex);

  destroy_dlinked_list(list);
  free(list);

  exit(0);
}
