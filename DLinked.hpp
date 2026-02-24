#ifndef DLINKED
#define DLINKED

struct DLinkedElem{
  DLinkedElem* prev;
  DLinkedElem* next;
  int data;

  DLinkedElem(DLinkedElem* prev,
              int data);
  DLinkedElem(int data);

  int countBits(int bit);
};

struct DLinkedList{
  DLinkedElem* first;
  DLinkedElem* last;

  DLinkedList(int num);
  ~DLinkedList();

  int threadMagic(int* res0,
                  int* res1);
};

#endif
