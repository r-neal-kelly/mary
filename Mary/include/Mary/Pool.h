#pragma once

#include <Mary/Vector.h>

#define MARY_Pool_Slots 1024

typedef struct Mary_Pool_t Mary_Pool_t;

struct Mary_Pool_t
{
  MARY_Pointer_t;
  Mary_ID_t id; // should this just be allocator?
  Mary_Size_t free;
  Mary_Vector_t index; // consider making this always go on the pool. or at least an option for user?
};

void Mary_Pool_Start();
void Mary_Pool_Stop();

Mary_Pool_t *Mary_Pool_Create(Mary_Size_t bytes);
void Mary_Pool_Destroy(Mary_Pool_t *pool);
void *Mary_Pool_Alloc(Mary_Pool_t *pool, Mary_Size_t bytes); // rename these to be more consist. add Calloc
void *Mary_Pool_Realloc(Mary_Pool_t *pool, void *data, Mary_Size_t bytes);
void Mary_Pool_Dealloc(Mary_Pool_t *pool, void *data);
void Mary_Pool_Empty(Mary_Pool_t *pool);
Mary_Bool_t Mary_Pool_Has_Data(Mary_Pool_t *pool, void *data);
Mary_Bool_t Mary_Pool_Has_Free(Mary_Pool_t *pool, Mary_Size_t bytes);
// might be cool to have a func that returns a vector of ptrs to each value in pool.

#define MARY_Pool_Round(BYTES)\
  MARY_Round_to_64bit(BYTES)

#define MARY_Pool_Has_Free(POOL, BYTES)\
  ( (POOL)->free >= MARY_Pool_Round(bytes) )
