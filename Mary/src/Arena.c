#include <Mary/OS.h>
#include <Mary/Hashmap.h>
#include <Mary/Arena.h>

MARY_Primitives;

typedef struct
{
    Mary_Vector_t allocs;
    Mary_Vector_t errors;
}
Mary_Arena_Frame_t;

static Mary_Bool_t is_started = MARY_FALSE;
static Mary_Hashmap_t global_arenas;

void Mary_Arena_Start()
{
    if (is_started == MARY_TRUE) {
        return;
    } else {
        Mary_Pool_Start();
        is_started = MARY_TRUE;
    }

    Mary_Hashmap_Create(&global_arenas, sizeof(u64), sizeof(Mary_Arena_t));
    Mary_Arena_Create(MARY_Arena_Thread_Bytes);
}

void Mary_Arena_Stop()
{
    if (is_started == MARY_FALSE) {
        return;
    }

    Mary_Arena_Destroy();
    Mary_Hashmap_Destroy(&global_arenas);

    is_started = MARY_FALSE;
}

void Mary_Arena_Create(Mary_Size_t bytes)
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Arena has not been started.");

    u64 arena_id = Mary_OS_Thread_Current_ID();

    MARY_Assert(!Mary_Hashmap_Contains_Key(&global_arenas, &arena_id),
                "The arena for this thread is already created.");

    Mary_Arena_t arena;
    arena.pool = Mary_Pool_Create(bytes);
    MARY_Vector_Pool_p(&arena.frames, arena.pool, Mary_Arena_Frame_t, MARY_Arena_Reserve_Frames);
    Mary_Hashmap_Assign(&global_arenas, &arena_id, &arena);
}

void Mary_Arena_Destroy()
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Arena has not been started.");

    u64 arena_id = Mary_OS_Thread_Current_ID();

    MARY_Assert(Mary_Hashmap_Contains_Key(&global_arenas, &arena_id),
                "The arena for this thread was never created.");

    Mary_Arena_t *arena = Mary_Hashmap_Point(&global_arenas, &arena_id);

    MARY_Assert(arena->frames.units == 0, "There are still unpopped frames.");

    Mary_Vector_Destroy(&arena->frames);
    Mary_Pool_Destroy(arena->pool);
    Mary_Hashmap_Erase(&global_arenas, &arena_id);
}

Mary_ID_t Mary_Arena_Push()
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Arena has not been started.");

    u64 arena_id = Mary_OS_Thread_Current_ID();
    Mary_Arena_t *arena = Mary_Hashmap_Point(&global_arenas, &arena_id);

    Mary_ID_t frame_id = arena->frames.units;
    Mary_Arena_Frame_t *frame = MARY_Vector_Point_Push(&arena->frames);
    MARY_Vector_Pool_p(&frame->allocs, arena->pool, void *, MARY_Arena_Reserve_Allocs_Per_Frame);
    MARY_Vector_Pool_p(&frame->errors, arena->pool, Mary_Error_t *, 1);

    return frame_id;
}

void Mary_Arena_Pop(Mary_ID_t frame_id)
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Arena has not been started.");

    u64 arena_id = Mary_OS_Thread_Current_ID();
    Mary_Arena_t *arena = Mary_Hashmap_Point(&global_arenas, &arena_id);

    MARY_Assert(arena->frames.units > 0, "No frame available.");
    MARY_Assert(frame_id == arena->frames.units - 1, "Trying to pop another frame.");

    Mary_Arena_Frame_t *frame = MARY_Vector_Point_Pop(&arena->frames);

    MARY_Assert(frame->errors.units == 0, "Unhandled error in arena!");

    MARY_Vector_Each(&frame->allocs, void *) {
        if (Mary_Pool_Has_Data(arena->pool, it.val)) {
            Mary_Pool_Dealloc(arena->pool, it.val);
        } else {
            Mary_Dealloc(it.val);
        }
    }
    Mary_Vector_Destroy(&frame->allocs);

    MARY_Vector_Each(&frame->errors, void *) {
        if (Mary_Pool_Has_Data(arena->pool, it.val)) {
            Mary_Pool_Dealloc(arena->pool, it.val);
        } else {
            Mary_Dealloc(it.val);
        }
    }
    Mary_Vector_Destroy(&frame->errors);
}

void *Mary_Arena_Alloc_Frame(Mary_ID_t frame_id, Mary_Size_t bytes)
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Arena has not been started.");

    u64 arena_id = Mary_OS_Thread_Current_ID();
    Mary_Arena_t *arena = Mary_Hashmap_Point(&global_arenas, &arena_id);

    MARY_Assert(MARY_Vector_Has_Index(&arena->frames, frame_id),
                "Frame doesn't exist.");

    Mary_Arena_Frame_t *frame = MARY_Vector_Point(&arena->frames, frame_id);

    void *data;
    if (Mary_Pool_Has_Free(arena->pool, bytes)) {
        data = Mary_Pool_Alloc(arena->pool, bytes);
    } else {
        data = Mary_Alloc(bytes);
    }

    MARY_Assert(data != 0, "Out of memory.");

    MARY_Vector_Push(&frame->allocs, void *, data);

    return data;
}

void *Mary_Arena_Alloc_Chain(Mary_ID_t frame_id, Mary_Size_t bytes)
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Arena has not been started.");

    u64 arena_id = Mary_OS_Thread_Current_ID();
    Mary_Arena_t *arena = Mary_Hashmap_Point(&global_arenas, &arena_id);

    MARY_Assert(MARY_Vector_Has_Index(&arena->frames, frame_id - 1),
                "Chain doesn't exist.");

    Mary_Arena_Frame_t *chain = MARY_Vector_Point(&arena->frames, frame_id - 1);

    void *data;
    if (Mary_Pool_Has_Free(arena->pool, bytes)) {
        data = Mary_Pool_Alloc(arena->pool, bytes);
    } else {
        data = Mary_Alloc(bytes);
    }

    MARY_Assert(data != 0, "Out of memory.");

    MARY_Vector_Push(&chain->allocs, void *, data);

    return data;
}

void *Mary_Arena_Alloc_Error(Mary_ID_t frame_id, Mary_Size_t bytes)
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Arena has not been started.");

    u64 arena_id = Mary_OS_Thread_Current_ID();
    Mary_Arena_t *arena = Mary_Hashmap_Point(&global_arenas, &arena_id);

    MARY_Assert(MARY_Vector_Has_Index(&arena->frames, frame_id - 1),
                "Chain doesn't exist.");

    Mary_Arena_Frame_t *chain = MARY_Vector_Point(&arena->frames, frame_id - 1);

    void *data;
    if (Mary_Pool_Has_Free(arena->pool, bytes)) {
        data = Mary_Pool_Alloc(arena->pool, bytes);
    } else {
        data = Mary_Alloc(bytes);
    }

    MARY_Assert(data != 0, "Out of memory.");

    MARY_Vector_Push(&chain->errors, Mary_Error_t *, data);

    return data;
}

void *Mary_Arena_Realloc_Frame(Mary_ID_t frame_id, void *data, Mary_Size_t bytes)
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Arena has not been started.");

    u64 arena_id = Mary_OS_Thread_Current_ID();
    Mary_Arena_t *arena = Mary_Hashmap_Point(&global_arenas, &arena_id);

    MARY_Assert(MARY_Vector_Has_Index(&arena->frames, frame_id),
                "Frame doesn't exist.");

    Mary_Arena_Frame_t *frame = MARY_Vector_Point(&arena->frames, frame_id);
    Mary_Bool_t was_found; Mary_Index_t idx =
        Mary_Vector_Index_Of_Reverse(&frame->allocs, &data, &was_found);

    MARY_Assert(was_found == MARY_TRUE, "Could not find data in frame.");

    Mary_Vector_Erase_At(&frame->allocs, idx);

    if (Mary_Pool_Has_Data(arena->pool, data)) {
        data = Mary_Pool_Realloc(arena->pool, data, bytes);
    } else {
        data = Mary_Realloc(data, bytes);
    }

    MARY_Assert(data != 0, "Out of memory.");

    MARY_Vector_Push(&frame->allocs, void *, data);

    return data;
}

void *Mary_Arena_Realloc_Chain(Mary_ID_t frame_id, void *data, Mary_Size_t bytes)
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Arena has not been started.");

    u64 arena_id = Mary_OS_Thread_Current_ID();
    Mary_Arena_t *arena = Mary_Hashmap_Point(&global_arenas, &arena_id);

    MARY_Assert(MARY_Vector_Has_Index(&arena->frames, frame_id - 1),
                "Chain doesn't exist.");

    Mary_Arena_Frame_t *chain = MARY_Vector_Point(&arena->frames, frame_id - 1);
    Mary_Bool_t was_found; Mary_Index_t idx =
        Mary_Vector_Index_Of_Reverse(&chain->allocs, &data, &was_found);

    MARY_Assert(was_found == MARY_TRUE, "Could not find data in frame.");

    Mary_Vector_Erase_At(&chain->allocs, idx);

    if (Mary_Pool_Has_Data(arena->pool, data)) {
        data = Mary_Pool_Realloc(arena->pool, data, bytes);
    } else {
        data = Mary_Realloc(data, bytes);
    }

    MARY_Assert(data != 0, "Out of memory.");

    MARY_Vector_Push(&chain->allocs, void *, data);

    return data;
}

void Mary_Arena_Dealloc_Frame(Mary_ID_t frame_id, void *data)
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Arena has not been started.");

    u64 arena_id = Mary_OS_Thread_Current_ID();
    Mary_Arena_t *arena = Mary_Hashmap_Point(&global_arenas, &arena_id);

    MARY_Assert(MARY_Vector_Has_Index(&arena->frames, frame_id),
                "Frame doesn't exist.");

    Mary_Arena_Frame_t *frame = MARY_Vector_Point(&arena->frames, frame_id);
    Mary_Bool_t was_found; Mary_Index_t idx =
        Mary_Vector_Index_Of_Reverse(&frame->allocs, &data, &was_found);

    MARY_Assert(was_found == MARY_TRUE, "Could not find data in frame.");

    Mary_Vector_Erase_At(&frame->allocs, idx);

    if (Mary_Pool_Has_Data(arena->pool, data)) {
        Mary_Pool_Dealloc(arena->pool, data);
    } else {
        Mary_Dealloc(data);
    }
}

void Mary_Arena_Dealloc_Chain(Mary_ID_t frame_id, void *data)
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Arena has not been started.");

    u64 arena_id = Mary_OS_Thread_Current_ID();
    Mary_Arena_t *arena = Mary_Hashmap_Point(&global_arenas, &arena_id);

    MARY_Assert(MARY_Vector_Has_Index(&arena->frames, frame_id - 1),
                "Chain doesn't exist.");

    Mary_Arena_Frame_t *chain = MARY_Vector_Point(&arena->frames, frame_id - 1);
    Mary_Bool_t was_found; Mary_Index_t idx =
        Mary_Vector_Index_Of_Reverse(&chain->allocs, &data, &was_found);

    MARY_Assert(was_found == MARY_TRUE, "Could not find data in frame.");

    Mary_Vector_Erase_At(&chain->allocs, idx);

    if (Mary_Pool_Has_Data(arena->pool, data)) {
        Mary_Pool_Dealloc(arena->pool, data);
    } else {
        Mary_Dealloc(data);
    }
}

void *Mary_Arena_Alloc(Mary_Enum_t zone, Mary_ID_t frame_id, Mary_Size_t bytes)
{
    if (zone == MARY_ARENA_FRAME) {
        return Mary_Arena_Alloc_Frame(frame_id, bytes);
    } else if (zone == MARY_ARENA_CHAIN) {
        return Mary_Arena_Alloc_Chain(frame_id, bytes);
    } else if (zone == MARY_ARENA_ERROR) {
        return Mary_Arena_Alloc_Error(frame_id, bytes); // maybe we should force error creation to use the specific call, because we don't really want a vector for example, to create one.
    } else {
        MARY_Assert(MARY_FALSE, "Invalid zone.");
        return 0;
    }
}

void *Mary_Arena_Realloc(Mary_Enum_t zone, Mary_ID_t frame_id, void *data, Mary_Size_t bytes)
{
    if (zone == MARY_ARENA_FRAME) {
        return Mary_Arena_Realloc_Frame(frame_id, data, bytes);
    } else if (zone == MARY_ARENA_CHAIN) {
        return Mary_Arena_Realloc_Chain(frame_id, data, bytes);
    } else {
        MARY_Assert(MARY_FALSE, "Invalid zone.");
        return 0;
    }
}

void Mary_Arena_Dealloc(Mary_Enum_t zone, Mary_ID_t frame_id, void *data)
{
    if (zone == MARY_ARENA_CHAIN) {
        Mary_Arena_Dealloc_Chain(frame_id, data);
    } else if (zone == MARY_ARENA_FRAME) {
        Mary_Arena_Dealloc_Frame(frame_id, data);
    } else {
        MARY_Assert(MARY_FALSE, "Invalid zone.");
    }
}

void Mary_Arena_Chain(Mary_ID_t frame_id, void *data)
{
    MARY_Assert(is_started == MARY_TRUE, "Mary_Arena has not been started.");

    u64 arena_id = Mary_OS_Thread_Current_ID();
    Mary_Arena_t *arena = Mary_Hashmap_Point(&global_arenas, &arena_id);

    MARY_Assert(MARY_Vector_Has_Index(&arena->frames, frame_id),
                "Frame doesn't exist.");

    Mary_Arena_Frame_t *frame = MARY_Vector_Point(&arena->frames, frame_id);
    Mary_Arena_Frame_t *chain = MARY_Vector_Point(&arena->frames, frame_id - 1);
    Mary_Bool_t was_found; Mary_Index_t idx =
        Mary_Vector_Index_Of_Reverse(&frame->allocs, &data, &was_found);

    MARY_Assert(was_found == MARY_TRUE, "Could not find data in frame.");

    Mary_Vector_Erase_At(&frame->allocs, idx);

    MARY_Vector_Push(&chain->allocs, void *, data);
}

void Mary_Arena_Empty(Mary_Arena_t *arena)
{
    // need to check that there are no frames present.
}
