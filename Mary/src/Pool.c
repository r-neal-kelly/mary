#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Mary/Utils.h>
#include <Mary/Allocator.h>
#include <Mary/Pool.h>

MARY_Primitives;

#define MAX_SLOTS 0x1000
#define IDX_RESERVE 16

typedef struct
{
    u32 from;
    u32 to_exclusive;
}
Used_Space; // we might want this to be a Mary_Slice_t instead, although it will take twice as much space, it will be quicker.

static Mary_Bool_t is_started = MARY_FALSE;
Mary_Pool_t MARY_POOLS[MARY_Pool_Slots];

void Mary_Pool_Start()
{
    if (is_started == MARY_TRUE) return;
    else is_started = MARY_TRUE;

    MARY_Assert(MARY_Pool_Slots <= MAX_SLOTS, "Cannot have more than 4096 pool slots.");
    for (I i = 0; i < MARY_Pool_Slots; ++i) MARY_POOLS[i].data = 0;
}

void Mary_Pool_Stop()
{
    if (is_started == MARY_FALSE) return;
    else is_started = MARY_FALSE;
}

Mary_Pool_t *Mary_Pool_Create(Mary_Size_t bytes)
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Pool has not been started.");

    Mary_Pool_t *pool = 0;
    for (I i = 0; i < MARY_Pool_Slots; ++i) {
        if (MARY_POOLS[i].data == 0) {
            pool = &MARY_POOLS[i]; break;
        }
    }
    MARY_Assert(pool != 0, "Out of pools.");

    MARY_Assert(bytes > 0 && bytes <= 0x800000000, "32 gig max!"); // a u32 in index can only count so many u64s
    u64 size_8bit_data = bytes + 7 & -8; // rounded to 8 bytes, 64 bit
    u8 *data = malloc(size_8bit_data + 8);
    if (data) {
        pool->data = data + 8; // avoids accidental deallocation of entire pool.
        pool->bytes = size_8bit_data;
        pool->id = (u64)(pool - MARY_POOLS); // if we need to do more spacing, we can just as whatever offset we need from Allocator_e
        pool->free = size_8bit_data;
        Mary_Vector_Create(&pool->index, MARY_Allocator_Heap, sizeof(Used_Space), IDX_RESERVE); // this should have a param
    } else {
        Mary_Exit_Failure("Out of memory.");
    }

    return pool;
}

void Mary_Pool_Destroy(Mary_Pool_t *pool)
{
    Mary_Vector_Destroy(&pool->index);
    free((u8 *)pool->data - 8);
    pool->data = 0;
}

void *Mary_Pool_Alloc(Mary_Pool_t *pool, Mary_Size_t bytes)
{
    // this func needs to be cleaned up sometime.
    MARY_Assert(bytes > 0 && bytes <= pool->free, "Out of memory.");
    u64 size_8bit_alloc = bytes + 7 & -8;
    u32 size_64bit_alloc = (u32)(size_8bit_alloc >> 3);
    Used_Space index_new = { 0, 0 };
    if (!pool->index.units) {
        index_new.to_exclusive = size_64bit_alloc;
        MARY_Vector_Push(&pool->index, Used_Space, index_new);
    } else {
        Used_Space index_last = *((Used_Space *)pool->index.data + pool->index.units - 1);
        Used_Space index_first = *(Used_Space *)pool->index.data;
        if (size_64bit_alloc <= (pool->bytes >> 3) - index_last.to_exclusive) {
            index_new.from = index_last.to_exclusive;
            index_new.to_exclusive = index_new.from + size_64bit_alloc;
            MARY_Vector_Push(&pool->index, Used_Space, index_new);
        } else if (size_64bit_alloc <= index_first.from) {
            index_new.from = 0;
            index_new.to_exclusive = index_new.from + size_64bit_alloc;
            Mary_Vector_Push_At(&pool->index, 0, &index_new);
        } else {
            MARY_Range(pool->index.data, Used_Space, 0, pool->index.units - 1) {
                if (size_64bit_alloc <= (it.ptr + 1)->from - it.val.to_exclusive) {
                    index_new.from = it.val.to_exclusive;
                    index_new.to_exclusive = index_new.from + size_64bit_alloc;
                    Mary_Vector_Push_At(&pool->index, it.idx + 1, &index_new);
                    break;
                }
            }
        }
    }
    if (index_new.to_exclusive != 0) {
        pool->free -= size_8bit_alloc;
        return (u64 *)pool->data + index_new.from;
    } else {
        Mary_Exit_Failure("Out of memory.");
        return 0;
    }
}

static Used_Space *Get_Slice(Mary_Pool_t *pool, u64 *data, Mary_Index_t *out_slice_idx)
{
    MARY_Assert(!MARY_Vector_Is_Empty(&pool->index),
                "No slices in pool index.");

    Mary_Index_t idx;
    Used_Space *slice;
    u64 target_from = data - (u64 *)pool->data;

    if (pool->index.units <= 16) {
        MARY_Vector_Linear_Search(&pool->index, idx, slice, target_from, slice->from,
                                  { *out_slice_idx = idx; return slice; },
                                  { return 0; });
    } else {
        MARY_Vector_Binary_Search(&pool->index, idx, slice, target_from, slice->from,
                                  { *out_slice_idx = idx; return slice; },
                                  { return 0; });
    }
}

void *Mary_Pool_Realloc(Mary_Pool_t *pool, void *data, Mary_Size_t bytes)
{
    MARY_Assert(!MARY_Vector_Is_Empty(&pool->index), "No allocations in pool.");
    MARY_Assert(bytes > 0, "Bytes must be more than zero.");

    Mary_Index_t slice_idx;
    Used_Space *slice = Get_Slice(pool, data, &slice_idx);
    MARY_Assert(slice != 0, "Data ptr is not in pool.");

    Mary_Size_t old_bytes = (slice->to_exclusive - slice->from) * 8;
    Mary_Size_t new_bytes = MARY_Pool_Round(bytes);

    if (new_bytes == old_bytes) {
        return data;
    } else if (new_bytes < old_bytes) {
        Mary_Size_t diff_64s = (old_bytes - new_bytes) / 8;

        slice->to_exclusive -= (u32)diff_64s;
        pool->free += diff_64s * 8;
        return data;
    } else {
        Mary_Size_t free_64s = pool->free / 8;
        Mary_Size_t diff_64s = (new_bytes - old_bytes) / 8;

        if (slice_idx == pool->index.units - 1 &&
            slice->to_exclusive + (u32)diff_64s <= free_64s) {
            slice->to_exclusive += (u32)diff_64s;
            pool->free -= diff_64s * 8;
            return data;
        } else if (slice->to_exclusive + (u32)diff_64s <= (slice + 1)->from) {
            slice->to_exclusive += (u32)diff_64s;
            pool->free -= diff_64s * 8;
            return data;
        } else {
            Mary_Vector_Erase_At(&pool->index, slice_idx);
            pool->free += old_bytes;
            void *new_data = Mary_Pool_Alloc(pool, new_bytes);
            Mary_Copy(data, new_data, old_bytes);
            return new_data;
        }
    }
}

void Mary_Pool_Dealloc(Mary_Pool_t *pool, void *data)
{
    MARY_Assert(!MARY_Vector_Is_Empty(&pool->index), "No allocations in pool.");

    Mary_Index_t slice_idx;
    Used_Space *slice = Get_Slice(pool, data, &slice_idx);
    MARY_Assert(slice != 0, "Data ptr is not in pool.");

    pool->free += slice->to_exclusive - slice->from << 3;
    Mary_Vector_Erase_At(&pool->index, slice_idx);
}

void Mary_Pool_Empty(Mary_Pool_t *pool)
{
    MARY_Vector_Empty(&pool->index);
    pool->free = pool->bytes;
}

Mary_Bool_t Mary_Pool_Has_Data(Mary_Pool_t *pool, void *data)
{
    if (!MARY_Vector_Is_Empty(&pool->index)) {
        u64 *start = pool->data, *end = start + pool->bytes, *p = data;
        if (start <= p && p < end) {
            Mary_Index_t slice_idx;
            return MARY_Truthy(Get_Slice(pool, data, &slice_idx));
        }
    }

    return MARY_FALSE;
}

Mary_Bool_t Mary_Pool_Has_Free(Mary_Pool_t *pool, Mary_Size_t bytes)
{
    return pool->free >= MARY_Pool_Round(bytes);
}
