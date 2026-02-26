#include "dlinked.h"

#include <stdlib.h>

int init_next_node(dlinked_node* node,
                   dlinked_node* prev,
                   DLINKED_DATA_TYPE data) {
  if(node == NULL)
    return -1;

  node->prev = prev;
  node->next = NULL;
  node->data = data;
  return 0;
}
int update_next_node(dlinked_node* node,
                     dlinked_node* next) {
  if(node == NULL)
    return -1;

  node->next = next;
  return 0;
}

/**
 * \brief Функция вычисления количества бит в значении bit
 * 
 * Данная реализация предполагает анализ каждого из 32 битов, что  можно легко оптимизировать. Однако, чтобы сохранить прямолинейность решения, было решено оставить так. Так, можно использовать popcount или 32-popcount при подсчете 0, но в таком случае фактически всегда будут подсчитываться 1, а не указанное значение.
 * 
 * \param[in] node Элемент списка, в котором будет выполняться подсчет.
 * \param[in] bit 0 или 1, подсчитываемый в node.
 * \return Результат вычисления.
 */
int count_bits_in_node(const dlinked_node* node, 
                       DLINKED_DATA_TYPE bit) {
  int res=0;
  DLINKED_DATA_TYPE num;

  if(node == NULL)
    return -1;

  num = node->data;
  bit = bit&1;

  for(int i=0; i<sizeof(DLINKED_DATA_TYPE)*8; ++i) {
    if((num&1) == bit){
      ++res;
    }
    num >>= 1;
  }

  return res;
}

int init_dlinked_list(dlinked_list* list,
                      int num) {
  dlinked_node* prev;

  if(list == NULL)
    return -1;

  list->first = list->last = NULL;

  if(num) {
    list->first = list->last = malloc(sizeof(dlinked_node));
    if(list->last == NULL)
      return -1;
    init_next_node(list->last, NULL, rand());
    --num;
  }
  while(num) {
    prev = list->last;
    list->last = malloc(sizeof(dlinked_node));
    if(list->last == NULL)
      return -1;
    init_next_node(list->last, prev, rand());
    update_next_node(list->last->prev, list->last);
    --num;
  }

  return 0;
}
int destroy_dlinked_list(dlinked_list* list) {
  dlinked_node* next;

  while(list->first) {
    next = list->first->next;
    free(list->first);
    list->first = next;
  }
  list->last = NULL;

  return 0;
}
