#pragma once

// might want to call this Mary/Unicode.h

#include <Mary/Utils.h>

typedef uint64_t Mary_UTF_t;
typedef struct Mary_UTF_8_t Mary_UTF_8_t;
typedef struct Mary_UTF_16_t Mary_UTF_16_t;
typedef struct Mary_UTF_32_t Mary_UTF_32_t;
typedef union Mary_UTF_u Mary_UTF_u;

struct Mary_UTF_8_t {
    Mary_Size_t units;
    Mary_Char_32_t char_32;
    Mary_Char_8_t a, b, c, d;
};

struct Mary_UTF_16_t {
    Mary_Size_t units;
    Mary_Char_32_t char_32;
    Mary_Char_16_t a, b;
};

struct Mary_UTF_32_t {
    Mary_Size_t units;
    Mary_Char_32_t char_32;
};

union Mary_UTF_u {
    Mary_UTF_8_t utf_8;
    Mary_UTF_16_t utf_16;
    Mary_UTF_32_t utf_32;
};

void Mary_UTF_Encode_8(Mary_UTF_8_t *utf_8, Mary_Char_32_t char_32);
void Mary_UTF_Decode_8(Mary_UTF_8_t *utf_8, Mary_Char_8_t *ptr);
void Mary_UTF_Decode_8_Reverse(Mary_UTF_8_t *utf_8, Mary_Char_8_t *ptr_end);
void Mary_UTF_Encode_16(Mary_UTF_16_t *utf_16, Mary_Char_32_t char_32);
void Mary_UTF_Decode_16(Mary_UTF_16_t *utf_16, Mary_Char_16_t *ptr);
void Mary_UTF_Decode_16_Reverse(Mary_UTF_16_t *utf_16, Mary_Char_16_t *ptr_end);

#define MARY_UTF_32_To_Units_8(UTF_32)
#define MARY_UTF_32_To_Units_16(UTF_32)
#define MARY_UTF_Ptr_8_To_Units_8(PTR)
#define MARY_UTF_Ptr_16_To_Units_16(PTR)
#define MARY_UTF_Encode_8(UTF_8, CHAR_32, CASE_1, CASE_2, CASE_3, CASE_4)
#define MARY_UTF_Decode_8(UTF_8, PTR)
#define MARY_UTF_Decode_8_Reverse(UTF_8, PTR_END)
#define MARY_UTF_Encode_16(UTF_16, CHAR_32, CASE_1, CASE_2)
#define MARY_UTF_Decode_16(UTF_16, PTR)
#define MARY_UTF_Decode_16_Reverse(UTF_16, PTR_END)
#define MARY_UTF_Encode_32(UTF_32, CHAR_32, CASE_1)
#define MARY_UTF_Decode_32(UTF_32, PTR)
#define MARY_UTF_Decode_32_Reverse(UTF_32, PTR_END)

#include <Mary/Macros/UTF_m.h>

#define MARY_UTF_To_Unit(UTF)\
    ( (UTF) >> 3 )

#define MARY_Unit_To_UTF(UNIT)\
    ( (UNIT) << 3 )

#define MARY_UTF_To_Max_Units(UTF)\
    ( (UTF) == 8 ? 4 : (UTF) == 16 ? 2 : 1 )
