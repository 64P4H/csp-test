#ifndef DLINKED
#define DLINKED

#define DLINKED_DATA_TYPE int

typedef struct _dlinked_node{
  struct _dlinked_node* prev;
  struct _dlinked_node* next;
  DLINKED_DATA_TYPE data;
} dlinked_node;

int init_next_node(dlinked_node* node,
                   dlinked_node* prev,
                   DLINKED_DATA_TYPE data);
int update_next_node(dlinked_node* node,
                     dlinked_node* next);
int count_bits_in_node(const dlinked_node* node, 
                       DLINKED_DATA_TYPE bit);

typedef struct _dlinked_list{
  dlinked_node* first;
  dlinked_node* last;
} dlinked_list;

int init_dlinked_list(dlinked_list* list,
                      int num);
int destroy_dlinked_list(dlinked_list* list);

#endif
