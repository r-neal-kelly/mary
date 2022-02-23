#pragma once

#include <stdint.h>

#define MARY_Debug 1

#if (MARY_Debug == 1)

#define MARY_Assert(THIS_IS_TRUE, FAILURE_STR)  \
(                                               \
    (!(THIS_IS_TRUE)) ? (                       \
        Mary_Exit_Assert(#THIS_IS_TRUE,         \
                         FAILURE_STR,           \
                         __func__,              \
                         __FILE__,              \
                         __LINE__),             \
        0                                       \
    ) : (                                       \
        1                                       \
    )                                           \
)

#else

#define MARY_Assert(ITS_NOT_MY_PROBLEM, NOPE_DONT_CARE) 1

#endif

typedef uint64_t Mary_Bool_t;

enum {
    MARY_FALSE,
    MARY_TRUE
};

typedef int64_t Mary_Comparison_t;

enum {
    MARY_IS_LESS = -1,
    MARY_IS_SAME = 0,
    MARY_IS_MORE = 1
};

#define MARY_True(VALUE)\
    ( (VALUE) == MARY_TRUE )

#define MARY_False(VALUE)\
    ( (VALUE) == MARY_FALSE )

#define MARY_Truthy(VALUE)\
    ( (VALUE) > 0 ? MARY_TRUE : MARY_FALSE )

#define MARY_Falsey(VALUE)\
    ( (VALUE) < 1 ? MARY_TRUE : MARY_FALSE )

#define MARY_M\
    do {

#define MARY_W\
    } while (0)

#define MARY_Comma ,

// u = unsigned, s = signed, f = float, r = real, b = bool, e = enum, I = iter, U = unsigned, S = signed;
#define MARY_Primitives   \
    typedef uint8_t u8;   \
    typedef uint16_t u16; \
    typedef uint32_t u32; \
    typedef uint64_t u64; \
    typedef int8_t s8;    \
    typedef int16_t s16;  \
    typedef int32_t s32;  \
    typedef int64_t s64;  \
    typedef float f32;    \
    typedef double f64;   \
    typedef float r32;    \
    typedef double r64;   \
    typedef uint32_t b32; \
    typedef uint64_t b64; \
    typedef uint32_t e32; \
    typedef uint64_t e64; \
    typedef uint64_t I;   \
    typedef uint64_t U;   \
    typedef int64_t S

#define MARY_Swap_16(X) \
(                       \
    (X) >> 8 & 0x00FF | \
    (X) << 8 & 0xFF00   \
)

#define MARY_Swap_32(X)      \
(                            \
    (X) >> 24 & 0x000000FF | \
    (X) >> 8  & 0x0000FF00 | \
    (X) << 8  & 0x00FF0000 | \
    (X) << 24 & 0xFF000000   \
)

#define MARY_Swap_64(X)              \
(                                    \
    (X) >> 56 & 0x00000000000000FF | \
    (X) >> 48 & 0x000000000000FF00 | \
    (X) >> 40 & 0x0000000000FF0000 | \
    (X) >> 32 & 0x00000000FF000000 | \
    (X) << 32 & 0x000000FF00000000 | \
    (X) << 40 & 0x0000FF0000000000 | \
    (X) << 48 & 0x00FF000000000000 | \
    (X) << 56 & 0xFF00000000000000   \
)

#define MARY_Bit_Set(BITS, BIT_IDX)\
    ( (BITS) |=  ( (1llu) << (BIT_IDX) ) )

#define MARY_Bit_Unset(BITS, BIT_IDX)\
    ( (BITS) &= ~( (1llu) << (BIT_IDX) ) )

#define MARY_Bit_Is_Set(BITS, BIT_IDX)\
    ( (1llu) &   ( (BITS) >> (BIT_IDX) ) )

#define MARY_Bit_Toggle(BITS, BIT_IDX)\
    ( (BITS) ^=  ( (1llu) << (BIT_IDX) ) )

#define MARY_Bit_Clear(BITS)\
    ( (BITS) = 0 )

#define MARY_Swap_Unit(PTR1, PTR2, TEMP, UNIT) \
(                                              \
    Mary_Copy(PTR1, TEMP, UNIT),               \
    Mary_Copy(PTR2, PTR1, UNIT),               \
    Mary_Copy(TEMP, PTR2, UNIT)                \
)

#define MARY_Cast(PTR, TYPE)\
    ( (TYPE *)(PTR) )

#define MARY_Point(PTR, TYPE, IDX)\
    ( MARY_Cast(PTR, TYPE) + IDX )

#define MARY_At(PTR, TYPE, IDX)\
    ( *MARY_Point(PTR, TYPE, IDX) )

#define MARY_Point_Unit(PTR, UNIT, IDX)\
    MARY_Cast( (uint8_t *)(PTR) + (IDX) * (UNIT), void )

#define MARY_pp(PTR, BYTES)\
    ( PTR = MARY_Cast( (uint8_t *)PTR + (BYTES), void ) )

#define MARY_mm(PTR, BYTES)\
    ( PTR = MARY_Cast( (uint8_t *)PTR - (BYTES), void ) )

// MARY_Point_To_Byte?
#define MARY_Index_Byte(PTR, BYTES)\
    MARY_Cast( MARY_Cast( PTR, uint8_t ) + (BYTES), void )

// MARY_Point_To_Unit?
#define MARY_Index_Unit(PTR, TYPE, UNITS)\
    ( MARY_Cast((PTR), TYPE) + (UNITS) )

#define MARY_Range(PTR, TYPE, FROM, TO_EXCLUSIVE)   \
for (                                               \
    struct {                                        \
        Mary_Index_t from;                          \
        Mary_Index_t to_exclusive;                  \
        Mary_Index_t idx;                           \
        TYPE *ptr;                                  \
        TYPE val;                                   \
    } it = {                                        \
        FROM,                                       \
        TO_EXCLUSIVE,                               \
        it.from,                                    \
        MARY_Cast(PTR, TYPE) + it.idx,              \
        *it.ptr                                     \
    };                                              \
    it.idx < it.to_exclusive;                       \
    (                                               \
        ++it.idx,                                   \
        ++it.ptr,                                   \
        it.val = *it.ptr                            \
    )                                               \
)

#define MARY_Range_Reverse(PTR, TYPE, FROM, TO_EXCLUSIVE)   \
for (                                                       \
    struct {                                                \
        Mary_Index_t from;                                  \
        Mary_Index_t to_exclusive;                          \
        Mary_Index_t idx;                                   \
        TYPE *ptr;                                          \
        TYPE val;                                           \
    } it = {                                                \
        FROM,                                               \
        TO_EXCLUSIVE,                                       \
        it.to_exclusive - 1,                                \
        it.to_exclusive ?                                   \
            MARY_Cast(PTR, TYPE) + it.idx : PTR,            \
        *it.ptr                                             \
    };                                                      \
    it.idx + 1 > it.from;                                   \
    (it.idx != 0) ? (                                       \
        --it.idx,                                           \
        --it.ptr,                                           \
        it.val = *it.ptr                                    \
    ) : (                                                   \
        --it.idx,                                           \
        it.val                                              \
    )                                                       \
)

#define MARY_Range_Advance(INDICES)\
    ( it.idx += (INDICES), it.ptr += (INDICES), it.val = *it.ptr )

#define MARY_Range_Regress(INDICES)\
    ( it.idx -= (INDICES), it.ptr -= (INDICES), it.val = *it.ptr )

#define MARY_Clip_Number(NUMBER, FROM, TO)\
    ( (NUMBER) < (FROM) ? (FROM) : (NUMBER) > (TO) ? (TO) : (NUMBER) );

#define MARY_Round_to_64bit(BYTES)\
    ( (BYTES) + 7 & -8 )

#define MARY_Is_Power_2(NUMBER)\
    ( (NUMBER) != 0 && ((NUMBER) & (NUMBER) - 1) == 0 )

typedef uint8_t Mary_u8_t;
typedef uint16_t Mary_u16_t;
typedef uint32_t Mary_u32_t;
typedef uint64_t Mary_u64_t;

typedef int8_t Mary_s8_t;
typedef int16_t Mary_s16_t;
typedef int32_t Mary_s32_t;
typedef int64_t Mary_s64_t;

typedef uint8_t Mary_Byte_t;
typedef void Mary_Void_t;

typedef uint8_t Mary_Char_8_t;
typedef uint16_t Mary_Char_16_t;
typedef uint32_t Mary_Char_32_t;

typedef uint64_t Mary_Enum_t;
typedef uint8_t Mary_Enum_8_t;
typedef uint16_t Mary_Enum_16_t;
typedef uint32_t Mary_Enum_32_t;
typedef uint64_t Mary_Enum_64_t;

typedef uint64_t Mary_Index_t;
typedef uint8_t Mary_Index_8_t;
typedef uint16_t Mary_Index_16_t;
typedef uint32_t Mary_Index_32_t;
typedef uint64_t Mary_Index_64_t;

typedef uint64_t Mary_ID_t;
typedef uint8_t Mary_ID_8_t;
typedef uint16_t Mary_ID_16_t;
typedef uint32_t Mary_ID_32_t;
typedef uint64_t Mary_ID_64_t;

typedef uint64_t Mary_Size_t;
typedef uint8_t Mary_Size_8_t;
typedef uint16_t Mary_Size_16_t;
typedef uint32_t Mary_Size_32_t;
typedef uint64_t Mary_Size_64_t;

typedef Mary_Size_t Mary_Size_1D_t;
typedef struct Mary_Size_2D_t Mary_Size_2D_t;
typedef struct Mary_Size_3D_t Mary_Size_3D_t;

struct Mary_Size_2D_t {
    Mary_Size_t width, height;
};

struct Mary_Size_3D_t{
    Mary_Size_t width, height, depth;
};

// this needs to go in it's own file, where others can inherit from it, I think.
typedef struct Mary_Error_t Mary_Error_t;

struct Mary_Error_t {
    uint64_t id;
    uint64_t info;
};






typedef struct Mary_Slice_t Mary_Slice_t;

struct Mary_Slice_t {
    Mary_Index_t from;
    Mary_Index_t to_exclusive;
};

void Mary_Exit_Success();
void Mary_Exit_Success_Message(char *message);
void Mary_Exit_Failure(char *error_string);
void Mary_Exit_Assert(char *assertion, char *message, char *func, char *file, int line); // we might want to get a stack trace in this func.
Mary_Bool_t Mary_Is_Little_Endian();
Mary_Bool_t Mary_Is_Big_Endian();
void Mary_Print_Bits(void *value, Mary_Size_t size, Mary_Bool_t little_endian); // I really want to have a hex to decimal func. Will be a fun func!
uint64_t Mary_Random(uint64_t from, uint64_t to_exclusive);
void *Mary_Alloc(Mary_Size_t bytes);
void *Mary_Calloc(Mary_Size_t unit, Mary_Size_t units);
void *Mary_Realloc(void *data, Mary_Size_t bytes);
void Mary_Dealloc(void *data);
void Mary_Zero(void *data, Mary_Size_t bytes);
void *Mary_Copy(void *from, void *to, Mary_Size_t bytes);
void *Mary_Move(void *from, void *to, Mary_Size_t bytes);
void Mary_Swap(void *one, void *two, Mary_Size_t bytes);
void Mary_Set(void *data, void *val, Mary_Size_t val_unit, Mary_Size_t val_units); // should I add data_bytes as a param to help bound_check?
Mary_Bool_t Mary_Is_Same(void *one, void *two, Mary_Size_t bytes);
int64_t Mary_Compare(void *one, void *two, Mary_Size_t bytes); // should have its own return type, maybe Mary_Compare_t

// planned types
// Mary_Biarray_t, quickly extendable on either side. Mary_Queue_t? Deque?
// Mary_Map_t, essentially just a hashmap with vector index pointer to buckets.
