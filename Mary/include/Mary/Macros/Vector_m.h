#pragma once

#include <Mary/Vector.h>

#undef MARY_Vector_Static
#undef MARY_Vector_Stack
#undef MARY_Vector_Heap
#undef MARY_Vector_Pool
#undef MARY_Vector_Frame
#undef MARY_Vector_Chain
#undef MARY_Vector_Heap_p
#undef MARY_Vector_Pool_p
#undef MARY_Vector_Frame_p
#undef MARY_Vector_Chain_p
#undef MARY_Vector_Has_Index
#undef MARY_Vector_Is_Empty
#undef MARY_Vector_Point
#undef MARY_Vector_Point_Front
#undef MARY_Vector_Point_Back
#undef MARY_Vector_Point_End
#undef MARY_Vector_At
#undef MARY_Vector_At_Front
#undef MARY_Vector_At_Back
#undef MARY_Vector_Push
#undef MARY_Vector_Pop
#undef MARY_Vector_Point_Push
#undef MARY_Vector_Point_Pop
#undef MARY_Vector_Empty
#undef MARY_Vector_Cast
#undef MARY_Vector_Each
#undef MARY_Vector_Each_Reverse
#undef MARY_Vector_Range
#undef MARY_Vector_Range_Reverse
#undef MARY_Vector_Range_Advance
#undef MARY_Vector_Range_Regress
#undef MARY_Vector_Binary_Search

////// Creation //////

#define MARY_Vector_Static(NAME, TYPE, RESERVE_UNITS)\
    Mary_Vector_t NAME; static TYPE NAME##_STATIC[RESERVE_UNITS];\
    Mary_Vector_Create_At(&NAME, NAME##_STATIC, sizeof(NAME##_STATIC), MARY_Allocator_Static, sizeof(TYPE))

#define MARY_Vector_Stack(NAME, TYPE, RESERVE_UNITS)\
    Mary_Vector_t NAME; TYPE NAME##_STACK[RESERVE_UNITS];\
    Mary_Vector_Create_At(&NAME, NAME##_STACK, sizeof(NAME##_STACK), MARY_Allocator_Stack, sizeof(TYPE))

#define MARY_Vector_Heap(NAME, TYPE, OPT_RESERVE_UNITS)\
    Mary_Vector_t NAME; Mary_Vector_Create(&NAME, MARY_Allocator_Heap, sizeof(TYPE), OPT_RESERVE_UNITS)

#define MARY_Vector_Pool(NAME, POOL, TYPE, OPT_RESERVE_UNITS)\
    Mary_Vector_t NAME; Mary_Vector_Create(&NAME, MARY_Allocator_Pool((POOL)->id), sizeof(TYPE), OPT_RESERVE_UNITS)

#define MARY_Vector_Frame(NAME, TYPE, OPT_RESERVE_UNITS)\
    Mary_Vector_t NAME; Mary_Vector_Create(&NAME, MARY_Allocator_Frame, sizeof(TYPE), OPT_RESERVE_UNITS)

#define MARY_Vector_Chain(NAME, TYPE, OPT_RESERVE_UNITS)\
    Mary_Vector_t NAME; Mary_Vector_Create(&NAME, MARY_Allocator_Chain, sizeof(TYPE), OPT_RESERVE_UNITS)

#define MARY_Vector_Heap_p(THIS, TYPE, OPT_RESERVE_UNITS)\
    Mary_Vector_Create(THIS, MARY_Allocator_Heap, sizeof(TYPE), OPT_RESERVE_UNITS)

#define MARY_Vector_Pool_p(THIS, POOL, TYPE, OPT_RESERVE_UNITS)\
    Mary_Vector_Create(THIS, MARY_Allocator_Pool((POOL)->id), sizeof(TYPE), OPT_RESERVE_UNITS)

#define MARY_Vector_Frame_p(THIS, TYPE, OPT_RESERVE_UNITS)\
    Mary_Vector_Create(THIS, MARY_Allocator_Frame, sizeof(TYPE), OPT_RESERVE_UNITS)

#define MARY_Vector_Chain_p(THIS, TYPE, OPT_RESERVE_UNITS)\
    Mary_Vector_Create(THIS, MARY_Allocator_Chain, sizeof(TYPE), OPT_RESERVE_UNITS)

////// Booleans //////

#define MARY_Vector_Has_Index(THIS, IDX)\
    ( (IDX) < (THIS)->units )

#define MARY_Vector_Is_Empty(THIS)\
    ( !MARY_Vector_Has_Index(THIS, 0) )

////// Accessing //////

#define MARY_Vector_Point(THIS, IDX)\
    MARY_Cast( (uint8_t *)(THIS)->data + (IDX) * (THIS)->unit, void )

#define MARY_Vector_Point_Front(THIS)\
    MARY_Cast( (THIS)->data, void )

#define MARY_Vector_Point_Back(THIS)\
    MARY_Cast( (uint8_t *)(THIS)->data + ((THIS)->units - 1) * (THIS)->unit, void )

#define MARY_Vector_Point_End(THIS)\
    MARY_Cast( (uint8_t *)(THIS)->data + (THIS)->units * (THIS)->unit, void )

#define MARY_Vector_At(THIS, TYPE, IDX)                                  \
(                                                                        \
    *(TYPE *)(                                                           \
        MARY_Assert(MARY_Vector_Has_Index(THIS, IDX), "Invalid index."), \
        MARY_Assert(sizeof(TYPE) <= (THIS)->unit, "Invalid type."),      \
        (uint8_t *)(THIS)->data + (IDX) * (THIS)->unit                   \
    )                                                                    \
)

#define MARY_Vector_At_Front(THIS, TYPE)                            \
(                                                                   \
    *(TYPE *)(                                                      \
        MARY_Assert(!MARY_Vector_Is_Empty(THIS), "Empty Vector."),  \
        MARY_Assert(sizeof(TYPE) <= (THIS)->unit, "Invalid type."), \
        (THIS)->data                                                \
    )                                                               \
)

#define MARY_Vector_At_Back(THIS, TYPE)                              \
(                                                                    \
    *(TYPE *)(                                                       \
        MARY_Assert(!MARY_Vector_Is_Empty(THIS), "Empty Vector."),   \
        MARY_Assert(sizeof(TYPE) <= (THIS)->unit, "Invalid type."),  \
        (uint8_t *)(THIS)->data + ((THIS)->units - 1) * (THIS)->unit \
    )                                                                \
)

////// Altering //////

#define MARY_Vector_Push(THIS, TYPE, ITEM)           \
(                                                    \
    ++(THIS)->units * (THIS)->unit > (THIS)->bytes ? \
        (Mary_Vector_Grow(THIS), 0) : 0,             \
    MARY_Vector_At_Back(THIS, TYPE) = ITEM           \
)

#define MARY_Vector_Pop(THIS, TYPE)                                 \
(                                                                   \
    *(TYPE *)(                                                      \
        MARY_Assert(!MARY_Vector_Is_Empty(THIS), "Empty Vector."),  \
        MARY_Assert(sizeof(TYPE) <= (THIS)->unit, "Invalid type."), \
        (uint8_t *)(THIS)->data + --(THIS)->units * (THIS)->unit    \
    )                                                               \
)

#define MARY_Vector_Point_Push(THIS)                 \
(                                                    \
    ++(THIS)->units * (THIS)->unit > (THIS)->bytes ? \
        (Mary_Vector_Grow(THIS), 0) : 0,             \
    MARY_Vector_Point_Back(THIS)                     \
)

#define MARY_Vector_Point_Pop(THIS)                            \
(                                                              \
    MARY_Assert(!MARY_Vector_Is_Empty(THIS), "Empty Vector."), \
    --(THIS)->units, MARY_Vector_Point_End(THIS)               \
)

#define MARY_Vector_Empty(THIS)\
    ( (THIS)->units = 0 )

#define MARY_Vector_Cast(THIS)\
    MARY_Cast(THIS, Mary_Vector_t)

////// Loops //////

#define MARY_Vector_Each(THIS, TYPE)    \
for (                                   \
    struct {                            \
        Mary_Vector_t *this;            \
        Mary_Index_t idx;               \
        TYPE *ptr;                      \
        TYPE val;                       \
    } it = {                            \
        THIS,                           \
        0,                              \
        it.this->data,                  \
        *it.ptr                         \
    };                                  \
    it.idx < it.this->units;            \
    (                                   \
        ++it.idx,                       \
        MARY_pp(it.ptr, it.this->unit), \
        it.val = *it.ptr                \
    )                                   \
)

#define MARY_Vector_Each_Reverse(THIS, TYPE)    \
for (                                           \
    struct {                                    \
        Mary_Vector_t *this;                    \
        Mary_Index_t idx;                       \
        TYPE *ptr;                              \
        TYPE val;                               \
    } it = {                                    \
        THIS,                                   \
        it.this->units - 1,                     \
        it.this->units ?                        \
            MARY_Vector_Point_Back(it.this) :   \
            it.this->data,                      \
        *it.ptr                                 \
    };                                          \
    it.idx + 1 > 0;                             \
    (it.idx != 0) ? (                           \
        --it.idx,                               \
        MARY_mm(it.ptr, it.this->unit),         \
        it.val = *it.ptr                        \
    ) : (                                       \
        --it.idx,                               \
        it.val                                  \
    )                                           \
)

#define MARY_Vector_Range(THIS, TYPE, FROM, TO_EXCLUSIVE)   \
for (                                                       \
    struct {                                                \
        Mary_Vector_t *this;                                \
        Mary_Index_t from;                                  \
        Mary_Index_t to_exclusive;                          \
        Mary_Index_t idx;                                   \
        TYPE *ptr;                                          \
        TYPE val;                                           \
    } it = {                                                \
        THIS,                                               \
        (FROM) < it.this->units ?                           \
            (FROM) : it.this->units,                        \
        (TO_EXCLUSIVE) <= it.this->units ?                  \
            (TO_EXCLUSIVE) : it.this->units,                \
        it.from,                                            \
        MARY_Vector_Point(it.this, it.idx),                 \
        *it.ptr                                             \
    };                                                      \
    it.idx < it.to_exclusive;                               \
    (                                                       \
        ++it.idx,                                           \
        MARY_pp(it.ptr, it.this->unit),                     \
        it.val = *it.ptr                                    \
    )                                                       \
)

#define MARY_Vector_Range_Reverse(THIS, TYPE, FROM, TO_EXCLUSIVE)   \
for (                                                               \
    struct {                                                        \
        Mary_Vector_t *this;                                        \
        Mary_Index_t from;                                          \
        Mary_Index_t to_exclusive;                                  \
        Mary_Index_t idx;                                           \
        TYPE *ptr;                                                  \
        TYPE val;                                                   \
    } it = {                                                        \
        THIS,                                                       \
        (FROM) < it.this->units ?                                   \
            (FROM) : it.this->units,                                \
        (TO_EXCLUSIVE) <= it.this->units ?                          \
            (TO_EXCLUSIVE) : it.this->units,                        \
        it.to_exclusive - 1,                                        \
        it.to_exclusive ?                                           \
            MARY_Vector_Point(it.this, it.idx) :                    \
            it.this->data,                                          \
        *it.ptr                                                     \
    };                                                              \
    it.idx + 1 > it.from;                                           \
    (it.idx != 0) ? (                                               \
        --it.idx,                                                   \
        MARY_mm(it.ptr, it.this->unit),                             \
        it.val = *it.ptr                                            \
    ) : (                                                           \
        --it.idx,                                                   \
        it.val                                                      \
    )                                                               \
)

#define MARY_Vector_Range_Advance(INDICES)       \
(                                                \
    it.idx += (INDICES),                         \
    it.ptr = MARY_Vector_Point(it.this, it.idx), \
    it.val = *it.ptr                             \
)

#define MARY_Vector_Range_Regress(INDICES)       \
(                                                \
    it.idx -= (INDICES),                         \
    it.ptr = MARY_Vector_Point(it.this, it.idx), \
    it.val = *it.ptr                             \
)

////// Search //////

#define MARY_Vector_Linear_Search(THIS, IDX, PTR, KEY, ACCESS, IF_FOUND, ELSE)  \
MARY_M                                                                          \
    Mary_Vector_t *this = THIS;                                                 \
    IDX = 0;                                                                    \
    PTR = this->data;                                                           \
    while (MARY_TRUE) {                                                         \
        if (ACCESS == KEY) {                                                    \
            IF_FOUND;                                                           \
            break;                                                              \
        } else if (IDX < this->units) {                                         \
            ++IDX;                                                              \
            MARY_pp(PTR, this->unit);                                           \
        } else {                                                                \
            ELSE;                                                               \
            break;                                                              \
        }                                                                       \
    }                                                                           \
MARY_W

#define MARY_Vector_Binary_Search(THIS, IDX, PTR, KEY, ACCESS, IF_FOUND, ELSE)  \
MARY_M                                                                          \
    Mary_Vector_t *this = THIS;                                                 \
    Mary_Index_t from = 0;                                                      \
    Mary_Index_t to = this->units - 1;                                          \
    while (MARY_TRUE) {                                                         \
        IDX = (from + to) / 2;                                                  \
        PTR = MARY_Vector_Point(this, IDX);                                     \
        if (ACCESS < KEY) {                                                     \
            if (IDX < to) {                                                     \
                from = IDX + 1;                                                 \
            } else {                                                            \
                ELSE;                                                           \
                break;                                                          \
            }                                                                   \
        } else if (ACCESS > KEY) {                                              \
            if (IDX > from) {                                                   \
                to = IDX - 1;                                                   \
            } else {                                                            \
                ELSE;                                                           \
                break;                                                          \
            }                                                                   \
        } else {                                                                \
            IF_FOUND;                                                           \
            break;                                                              \
        }                                                                       \
    }                                                                           \
MARY_W

////// Sort //////

// I want to be able to combine these in a func so that it does the right one for the size.
#define MARY_Vector_Bubble_Sort(BOOL_EXPRESSION)

#define MARY_Vector_Quick_Sort(BOOL_EXPRESSION)
