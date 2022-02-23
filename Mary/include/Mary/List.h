#pragma once

#include <stdint.h>
#include <Mary/Pool.h>

typedef struct
{
  void *next;
  void *prev;
}
Mary_Link_t;

typedef struct
{
  Mary_Pool_t *links;
  size_t unit;
  size_t units;
  Mary_Link_t *front;
  Mary_Link_t *back;
}
Mary_List_t;

void Mary_List_Create(Mary_List_t *list, size_t unit, size_t opt_units);
void Mary_List_Destroy(Mary_List_t *list);
void Mary_List_Push_Back(Mary_List_t *list, void *in_elem);
void Mary_List_Push_Front(Mary_List_t *list);
void Mary_List_Push_At(Mary_List_t *list); // was Insert
void Mary_List_Pop_Back(Mary_List_t *list);
void Mary_List_Pop_Front(Mary_List_t *list);
void Mary_List_Pop_At(Mary_List_t *list); // was Eject
void Mary_List_Assign(Mary_List_t *list);
void *Mary_List_Point(Mary_List_t *list, size_t idx);
void Mary_List_Next(Mary_List_t *list, void *out_ptr);
void Mary_List_At(Mary_List_t *list, size_t idx, void *out_elem);
void Mary_List_Back(Mary_List_t *list);
void Mary_List_Front(Mary_List_t *list);
void Mary_List_Tail(Mary_List_t *list);
void Mary_List_Erase(Mary_List_t *list);
void Mary_List_Erase_At(Mary_List_t *list, size_t idx);

// this needs to be updated to take a LIST_PTR instead, to be consistent with Vector_Each
#define MARY_List_Each(LIST, TYPE)                                                                 \
  for                                                                                              \
  (                                                                                                \
    struct { size_t idx; Mary_Link_t *link; TYPE *ptr; TYPE val; }                                 \
    range = { 0, LIST.front, (TYPE *)(LIST.front+1), LIST.front ? *(TYPE *)(LIST.front+1) : 0 };   \
    range.link != 0;                                                                               \
    ++range.idx, range.link = range.link->next,                                                    \
    range.ptr = (TYPE *)(range.link + 1), range.val = range.link ? *range.ptr : 0                  \
  )
