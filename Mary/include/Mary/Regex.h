#pragma once

#include <stdint.h>
#include <Mary/Utils.h>
#include <Mary/Pool.h>
#include <Mary/Vector.h>
#include <Mary/String.h>

typedef struct Mary_Regex_t Mary_Regex_t;
typedef Mary_Slice_t Mary_Regex_Match_t;

struct Mary_Regex_t {
    Mary_Allocator_t allocator;
    Mary_String_t expression;
    Mary_Vector_t classes;
    Mary_Enum_t flags;
    Mary_Index_t start;
    void *system;
};

void Mary_Regex_Start();
void Mary_Regex_Stop();
void Mary_Regex_Create(Mary_Regex_t *this, Mary_Allocator_t allocator, Mary_String_t *expression, Mary_C_String_8_t flags);
void Mary_Regex_Destroy(Mary_Regex_t *this);
void Mary_Regex_Execute(Mary_Regex_t *this, Mary_String_t *search, Mary_Vector_t *out_matches);

// I would like to have a simple macro that takes /abc/g and parses it into the correct strings from Mary_Regex_Create.
// instead of requiring that "/" be escaped, just reverse find it from the end of string.

extern uint32_t MARY_Regex_Class_Space[];
extern uint32_t MARY_Regex_Class_Latin[];
extern uint32_t MARY_Regex_Class_Latin_Char[];
extern uint32_t MARY_Regex_Class_Latin_Char_Lower[];
extern uint32_t MARY_Regex_Class_Latin_Char_Upper[];
extern uint32_t MARY_Regex_Class_Latin_Punc[];
extern uint32_t MARY_Regex_Class_Latin_Numb[];
extern uint32_t MARY_Regex_Class_Hebrew[];
extern uint32_t MARY_Regex_Class_Hebrew_Char[];
extern uint32_t MARY_Regex_Class_Greek[];

#define MARY_Regex_Class_Has(CLASS_32, CODE_32, OUT_BOOL)                \
MARY_M                                                                   \
    u32 *class_32 = CLASS_32, code_32 = CODE_32;                         \
    OUT_BOOL = MARY_FALSE;                                               \
    for (I i = 0; class_32[i] != 0; ++i) {                               \
        if (class_32[i + 1] == '-') {                                    \
            if (class_32[i] <= code_32 && code_32 <= class_32[i += 2]) { \
                OUT_BOOL = MARY_TRUE; break;                             \
            }                                                            \
        } else if (class_32[i] == code_32) {                             \
            OUT_BOOL = MARY_TRUE; break;                                 \
        }                                                                \
    }                                                                    \
MARY_W
