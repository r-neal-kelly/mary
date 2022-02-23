#pragma once

#include <Mary/Utils.h>
#include <Mary/UTF.h>

typedef void *Mary_C_String_t;
typedef uint8_t *Mary_C_String_8_t;
typedef uint16_t *Mary_C_String_16_t;
typedef uint32_t *Mary_C_String_32_t;

// not sure I really want these!!!
typedef struct { Mary_C_String_t data; Mary_Size_t bytes; } Mary_C_String_p; // I'm really not sure if these should have allocator field or not.
typedef struct { Mary_C_String_8_t data; Mary_Size_t bytes; } Mary_C_String_8_p; // should we add a units field for these three?
typedef struct { Mary_C_String_16_t data; Mary_Size_t bytes; } Mary_C_String_16_p;
typedef struct { Mary_C_String_32_t data; Mary_Size_t bytes; } Mary_C_String_32_p;

typedef struct { Mary_Size_t bytes, units, codes; } Mary_C_String_Size_t;

#define MARY_C_String(UTF, C_STR)   \
(                                   \
    UTF == 8  ? (void *)u8##C_STR : \
    UTF == 16 ? (void *) u##C_STR : \
    UTF == 32 ? (void *) U##C_STR : \
                (void *) ""         \
)

#define MARY_C_String_p(NAME, DATA, BYTES)\
    Mary_C_String_p NAME = { DATA, BYTES }

#define MARY_C_String_8_p(NAME, DATA, BYTES)\
    Mary_C_String_8_p NAME = { DATA, BYTES }

#define MARY_C_String_16_p(NAME, DATA, BYTES)\
    Mary_C_String_16_p NAME = { DATA, BYTES }

#define MARY_C_String_32_p(NAME, DATA, BYTES)\
    Mary_C_String_32_p NAME = { DATA, BYTES }

Mary_Size_t Mary_C_String_Count_Bytes(Mary_C_String_t c_string, Mary_UTF_t utf, Mary_Bool_t with_null);
Mary_Size_t Mary_C_String_Count_Units(Mary_C_String_t c_string, Mary_UTF_t utf, Mary_Bool_t with_null);
Mary_Size_t Mary_C_String_Count_Codes(Mary_C_String_t c_string, Mary_UTF_t utf, Mary_Bool_t with_null);
void Mary_C_String_Count(Mary_C_String_t c_string, Mary_UTF_t utf, Mary_Bool_t with_null, Mary_C_String_Size_t *out_size);
void Mary_C_String_Copy(Mary_C_String_p *from, Mary_UTF_t from_utf, Mary_C_String_p *to, Mary_UTF_t to_utf, Mary_Bool_t with_null);
Mary_u64_t Mary_C_String_To_Hexadecimal(Mary_C_String_t c_str, Mary_UTF_t utf, Mary_C_String_t *opt_out_ptr);
