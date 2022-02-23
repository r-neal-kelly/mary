#pragma once

#include <Mary/Utils.h>
#include <Mary/Allocator.h>
#include <Mary/Vector.h>
#include <Mary/Pool.h>

#define MARY_Arena_Thread_Bytes 0x300000
#define MARY_Arena_Reserve_Frames 8
#define MARY_Arena_Reserve_Allocs_Per_Frame 8
#define MARY_Arena_Frame_ID_Name MARY_FRAME_ID

enum Mary_Arena_Zone_e
{
    MARY_ARENA_FRAME = MARY_ALLOCATOR_FRAME,
    MARY_ARENA_CHAIN = MARY_ALLOCATOR_CHAIN,
    MARY_ARENA_ERROR = MARY_ALLOCATOR_ERROR
};

typedef struct Mary_Arena_t Mary_Arena_t;

struct Mary_Arena_t
{
    Mary_Pool_t *pool;
    Mary_Vector_t frames;
};

void Mary_Arena_Start();
void Mary_Arena_Stop();
void Mary_Arena_Create();
void Mary_Arena_Destroy();
Mary_ID_t Mary_Arena_Push();
void Mary_Arena_Pop(Mary_ID_t frame_id);
void *Mary_Arena_Alloc_Frame(Mary_ID_t frame_id, Mary_Size_t bytes);
void *Mary_Arena_Alloc_Chain(Mary_ID_t frame_id, Mary_Size_t bytes);
void *Mary_Arena_Alloc_Error(Mary_ID_t frame_id, Mary_Size_t bytes); // we have bytes on here for the sake of variable error_types
void *Mary_Arena_Realloc_Frame(Mary_ID_t frame_id, void *data, Mary_Size_t bytes);
void *Mary_Arena_Realloc_Chain(Mary_ID_t frame_id, void *data, Mary_Size_t bytes);
void Mary_Arena_Dealloc_Frame(Mary_ID_t frame_id, void *data);
void Mary_Arena_Dealloc_Chain(Mary_ID_t frame_id, void *data);
void *Mary_Arena_Alloc(Mary_Enum_t zone, Mary_ID_t frame_id, Mary_Size_t bytes);
void *Mary_Arena_Realloc(Mary_Enum_t zone, Mary_ID_t frame_id, void *data, Mary_Size_t bytes);
void Mary_Arena_Dealloc(Mary_Enum_t zone, Mary_ID_t frame_id, void *data);
void Mary_Arena_Chain(Mary_ID_t frame_id, void *data);
void Mary_Arena_Save(); // I think it would be useful to have a way to get data out of pool to user's location. eg if a lib function returns a chain, but the user wants to keep it.
void Mary_Arena_Empty(); // clears the entire allocation both in pool and in heap.

#define MARY_Arena_In\
    Mary_ID_t MARY_Arena_Frame_ID_Name = Mary_Arena_Push();

#define MARY_Arena_Out\
    Mary_Arena_Pop(MARY_Arena_Frame_ID_Name);

#define MARY_Arena_Return(RETURN)  \
MARY_M                             \
    MARY_Arena_Out; return RETURN; \
MARY_W

#define MARY_Arena_Chain(DATA)\
    Mary_Arena_Chain(MARY_Arena_Frame_ID_Name, DATA)

#define MARY_Arena_Chain_Return(DATA) \
MARY_M                                \
    MARY_Arena_Chain(DATA);           \
    MARY_Arena_Return(DATA);          \
MARY_W

#define MARY_Arena_Alloc(ZONE, BYTES)                                                    \
(                                                                                        \
    ZONE == MARY_ARENA_FRAME ? Mary_Arena_Alloc_Frame(MARY_Arena_Frame_ID_Name, BYTES) : \
    ZONE == MARY_ARENA_CHAIN ? Mary_Arena_Alloc_Chain(MARY_Arena_Frame_ID_Name, BYTES) : \
    ZONE == MARY_ARENA_ERROR ? Mary_Arena_Alloc_Error(MARY_Arena_Frame_ID_Name, BYTES) : \
    (MARY_Assert(MARY_FALSE, "Invalid allocator"), 0)                                    \
)

#define MARY_Arena_Dealloc(ZONE, BYTES)                                                    \
(                                                                                          \
    ZONE == MARY_ARENA_CHAIN ? Mary_Arena_Dealloc_Chain(MARY_Arena_Frame_ID_Name, BYTES) : \
    ZONE == MARY_ARENA_FRAME ? Mary_Arena_Dealloc_Frame(MARY_Arena_Frame_ID_Name, BYTES) : \
    (MARY_Assert(MARY_FALSE, "Invalid allocator"), 0)                                      \
)
