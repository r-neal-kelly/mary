#pragma once

#include <Mary/Utils.h>

enum Mary_Allocator_e
{
    MARY_ALLOCATOR_STATIC,
    MARY_ALLOCATOR_STACK,
    MARY_ALLOCATOR_HEAP,
    MARY_ALLOCATOR_POOL,
    MARY_ALLOCATOR_FRAME,
    MARY_ALLOCATOR_CHAIN,
    MARY_ALLOCATOR_ERROR
};

typedef struct Mary_Pool_t Mary_Pool_t;
typedef struct Mary_Arena_t Mary_Arena_t;
typedef struct Mary_Allocator_t Mary_Allocator_t;

struct Mary_Allocator_t
{
    Mary_Enum_32_t type;
    uint32_t var;
};

void *Mary_Allocator_Alloc(Mary_Allocator_t allocator, Mary_Size_t bytes);
void *Mary_Allocator_Calloc(Mary_Allocator_t allocator, Mary_Size_t unit, Mary_Size_t units);
void *Mary_Allocator_Realloc(Mary_Allocator_t allocator, void *data, Mary_Size_t bytes);
void Mary_Allocator_Dealloc(Mary_Allocator_t allocator, void *data);

#define MARY_Allocator_Static\
    ( (Mary_Allocator_t){ MARY_ALLOCATOR_STATIC, 0 } )

#define MARY_Allocator_Stack\
    ( (Mary_Allocator_t){ MARY_ALLOCATOR_STACK, 0 } )

#define MARY_Allocator_Heap\
    ( (Mary_Allocator_t){ MARY_ALLOCATOR_HEAP, 0 } )

#define MARY_Allocator_Pool(POOL_ID)\
    ( (Mary_Allocator_t){ MARY_ALLOCATOR_POOL, (uint32_t)(POOL_ID) } )

#define MARY_Allocator_Frame\
    ( (Mary_Allocator_t){ MARY_ALLOCATOR_FRAME, (uint32_t)(MARY_Arena_Frame_ID_Name) } )

#define MARY_Allocator_Chain\
    ( (Mary_Allocator_t){ MARY_ALLOCATOR_CHAIN, (uint32_t)(MARY_Arena_Frame_ID_Name) } )

#define MARY_Allocator_Error\
    ( (Mary_Allocator_t){ MARY_ALLOCATOR_ERROR, (uint32_t)(MARY_Arena_Frame_ID_Name) } )



typedef struct Mary_Pointer_t Mary_Pointer_t;
typedef Mary_Pointer_t Mary_p;

#define MARY_Pointer_t\
    void *data;\
    Mary_Size_t bytes;\
    Mary_Allocator_t allocator

struct Mary_Pointer_t
{
    MARY_Pointer_t;
};

#define MARY_Pointer(NAME, DATA, BYTES, ALLOCATOR)\
    Mary_p NAME = { DATA, BYTES, ALLOCATOR }

#define MARY_p(DATA, BYTES, ALLOCATOR)\
    MARY_Pointer(DATA, BYTES, ALLOCATOR)

#define MARY_Pointer_Cast(PTR)\
    MARY_Cast(PTR, Mary_p)
