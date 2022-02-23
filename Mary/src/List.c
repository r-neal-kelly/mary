#include <string.h>
#include <Mary/List.h>

MARY_Primitives;

#define DEFAULT_UNITS 8
#define GROWTH_RATE 1.7f

void Mary_List_Create(Mary_List_t *list, size_t unit, size_t opt_units)
{
  size_t units = (opt_units) ? opt_units : DEFAULT_UNITS;
  unit = (sizeof(Mary_Link_t) + unit) + 7 & -8;
  list->links = Mary_Pool_Create(units * unit);
  list->unit = unit;
  list->units = 0;
  list->front = 0; // add a middle? make it easier for a binary search.
  list->back = 0;
}

void Mary_List_Destroy(Mary_List_t *list)
{
  Mary_Pool_Destroy(list->links);
}

void Mary_List_Reserve(Mary_List_t *list, size_t units)
{

}

void Mary_List_Push_Back(Mary_List_t *list, void *in_elem)
{
  if (list->links->free == 0)
  {
    size_t bytes = list->unit * (size_t)(list->units * GROWTH_RATE + 1);
    Mary_Pool_t *links = Mary_Pool_Create(bytes);
    Mary_Link_t *old = list->front;
    Mary_Link_t *new = Mary_Pool_Alloc(links, list->unit);
    Mary_Link_t *front = new;
    memcpy(new, old, list->unit);
    old = old->next;
    for (Mary_Link_t *prev = new; old != 0; prev = new)
    {
      new = Mary_Pool_Alloc(links, list->unit);
      memcpy(new, old, list->unit);
      old = old->next;
      prev->next = new;
      new->prev = prev;
    }
    Mary_Pool_Destroy(list->links);
    list->links = links;
    list->front = front;
    list->back = new;
  }
  Mary_Link_t *link = Mary_Pool_Alloc(list->links, list->unit);
  if (list->units == 0)
  {
    link->prev = 0;
    list->front = link;
  }
  else
  {
    link->prev = list->back;
    Mary_Link_t *prev = list->back;
    if (prev)
    {
      prev->next = link;
    }
  }
  link->next = 0;
  list->back = link;
  memcpy(link + 1, in_elem, list->unit - sizeof(Mary_Link_t));
  ++list->units;
}

/*inline void *Mary_List_Point(Mary_List_t *mary_list, size_t index)
{
  if (index == 0)
  {
    return mary_list->front;
  }
  else if (index == mary_list->units - 1)
  {
    return mary_list->back;
  }
  else
  {
    size_t unit = mary_list->unit;
    u8 *p; memcpy(&p, (u8 *)mary_list->front + unit, SIZE_PTR);
    for (size_t i = 1; i < index; ++i)
    {
      memcpy(&p, p + unit, SIZE_PTR);
    }
    return p;
  }
}

inline void Mary_List_Next(Mary_List_t *mary_list, void *out_ptr)
{
  memcpy(&out_ptr, (u8 *)out_ptr + mary_list->unit, SIZE_PTR);
}

void Mary_List_At(Mary_List_t *mary_list, size_t index, void *out_elem)
{
  if (index == 0)
  {
    memcpy(out_elem, mary_list->front, mary_list->unit);
  }
  else if (index == mary_list->units - 1)
  {
    memcpy(out_elem, mary_list->back, mary_list->unit);
  }
  else
  {
    memcpy(out_elem, Mary_List_Point(mary_list, index), mary_list->unit);
  }
}

void Mary_List_Erase_At(Mary_List_t *mary_list, size_t index)
{
  u8 *front = mary_list->front;
  size_t unit = mary_list->unit;
  if (index == 0)
  {
    u8 *next; memcpy(&next, front + unit, SIZE_PTR);
    Mary_Pool_Dealloc(&mary_list->pool, front);
    mary_list->front = next;
  }
  else
  {
    u8 *prev = mary_list->front;
    for (size_t i = 1; i < index; ++i)
    {
      memcpy(&prev, prev + unit, SIZE_PTR);
    }
    u8 *self; memcpy(&self, prev + unit, SIZE_PTR);
    u8 *next; memcpy(&next, self + unit, SIZE_PTR);
    Mary_Pool_Dealloc(&mary_list->pool, self);
    memcpy(prev + unit, &next, SIZE_PTR);
    if (index == mary_list->units - 1)
    {
      mary_list->back = prev;
    }
  }
  --mary_list->units;
}*/
