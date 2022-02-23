#pragma once

#include <Mary/String.h>

#undef MARY_String_Static
#undef MARY_String_Stack
#undef MARY_String_Heap
#undef MARY_String_Pool
#undef MARY_String_Frame
#undef MARY_String_Chain
#undef MARY_String_Heap_p
#undef MARY_String_Pool_p
#undef MARY_String_Frame_p
#undef MARY_String_Chain_p
#undef MARY_String_Is_Created
#undef MARY_String_Get_UTF
#undef MARY_String_Point_Front
#undef MARY_String_Point_Back
#undef MARY_String_Point_End
#undef MARY_String_Assign
#undef MARY_String_Append_Front
#undef MARY_String_Append_Back
#undef MARY_String_Each
#undef MARY_String_Each_Reverse
#undef MARY_String_Range
#undef MARY_String_Range_Reverse
#undef MARY_String_Range_Advance
#undef MARY_String_Range_Regress

////// Creation //////

#define MARY_String_Static(NAME, UTF, C_STR)\
    Mary_String_t NAME; Mary_String_Create_With(&NAME, C_STR, 0, MARY_Allocator_Static, UTF)

#define MARY_String_Stack(NAME, UTF, C_STR)\
    Mary_String_t NAME; uint##UTF##_t NAME##_stack[] = C_STR;\
    Mary_String_Create_With(&NAME, NAME##_stack, 0, MARY_Allocator_Stack, UTF)

#define MARY_String_Heap(NAME, UTF, C_STR)\
    Mary_String_t NAME; Mary_String_Create_From(&NAME, MARY_Allocator_Heap, UTF, C_STR, UTF)

#define MARY_String_Pool(NAME, POOL, UTF, C_STR)\
    Mary_String_t NAME; Mary_String_Create_From(&NAME, MARY_Allocator_Pool((POOL)->id), UTF, C_STR, UTF)

#define MARY_String_Frame(NAME, UTF, C_STR)\
    Mary_String_t NAME; Mary_String_Create_From(&NAME, MARY_Allocator_Frame, UTF, C_STR, UTF)

#define MARY_String_Chain(NAME, UTF, C_STR)\
    Mary_String_t NAME; Mary_String_Create_From(&NAME, MARY_Allocator_Chain, UTF, C_STR, UTF)

#define MARY_String_Heap_p(THIS, UTF, C_STR)\
    Mary_String_Create_From(THIS, MARY_Allocator_Heap, UTF, C_STR, UTF)

#define MARY_String_Pool_p(THIS, POOL, UTF, C_STR)\
    Mary_String_Create_From(THIS, MARY_Allocator_Pool((POOL)->id), UTF, C_STR, UTF)

#define MARY_String_Frame_p(THIS, UTF, C_STR)\
    Mary_String_Create_From(THIS, MARY_Allocator_Frame, UTF, C_STR, UTF)

#define MARY_String_Chain_p(THIS, UTF, C_STR)\
    Mary_String_Create_From(THIS, MARY_Allocator_Chain, UTF, C_STR, UTF)

////// Booleans //////

// this doesn't work, we would need an explicit is_created on the pointer or string type.
#define MARY_String_Is_Created(THIS)    \
(                                       \
    (THIS)->data != 0 &&                \
    (THIS)->codes > 0                   \
)

////// Accessing //////

#define MARY_String_Get_UTF(THIS)\
    MARY_Unit_To_UTF((THIS)->unit)

/*
// I want these to handle codes, not units.
#define MARY_String_Point_Front(THIS)\
    MARY_Vector_Point_Front(THIS)

#define MARY_String_Point_Back(THIS)\
    MARY_Vector_Point_Back(THIS)

#define MARY_String_Point_End(THIS)\
    MARY_Vector_Point_End(THIS)
*/

////// Altering //////

#define MARY_String_Assign(THIS, UTF, C_STR)    \
MARY_M                                          \
    MARY_String_Static(static_str, UTF, C_STR); \
    Mary_String_Assign(THIS, &static_str);      \
MARY_W

#define MARY_String_Append_Front(THIS, UTF, C_STR) \
MARY_M                                             \
    MARY_String_Static(static_str, UTF, C_STR);    \
    Mary_String_Append_Front(THIS, &static_str);   \
MARY_W

#define MARY_String_Append_Back(THIS, UTF, C_STR) \
MARY_M                                            \
    MARY_String_Static(static_str, UTF, C_STR);   \
    Mary_String_Append_Back(THIS, &static_str);   \
MARY_W

////// Loops //////

#define MARY_String_Each(THIS)                      \
for (                                               \
    struct {                                        \
        Mary_String_t *this;                        \
        Mary_Index_t code_idx;                      \
        Mary_Index_t unit_idx;                      \
        Mary_Void_t *ptr;                           \
        Mary_UTF_t utf;                             \
        union {                                     \
            Mary_Char_8_t *ptr_8;                   \
            Mary_Char_16_t *ptr_16;                 \
            Mary_Char_32_t *ptr_32;                 \
        } ptr_u;                                    \
        union {                                     \
            Mary_UTF_8_t utf_8;                     \
            Mary_UTF_16_t utf_16;                   \
            Mary_UTF_32_t utf_32;                   \
        } utf_u;                                    \
        Mary_Size_t units;                          \
        Mary_Char_32_t char_32;                     \
    } it = {                                        \
        THIS,                                       \
        0,                                          \
        0,                                          \
        it.this->data,                              \
        MARY_String_Get_UTF(it.this),               \
        { 0 },                                      \
        { 0 },                                      \
        0,                                          \
        (it.utf == 8) ? (                           \
            it.ptr_u.ptr_8 = it.ptr,                \
            Mary_UTF_Decode_8(&it.utf_u.utf_8,      \
                               it.ptr_u.ptr_8),     \
            it.units = it.utf_u.utf_8.units,        \
            it.utf_u.utf_8.char_32                  \
        ) : (it.utf == 16) ? (                      \
            it.ptr_u.ptr_16 = it.ptr,               \
            Mary_UTF_Decode_16(&it.utf_u.utf_16,    \
                                it.ptr_u.ptr_16),   \
            it.units = it.utf_u.utf_16.units,       \
            it.utf_u.utf_16.char_32                 \
        ) : (                                       \
            it.ptr_u.ptr_32 = it.ptr,               \
            MARY_UTF_Decode_32(it.utf_u.utf_32,     \
                               it.ptr_u.ptr_32),    \
            it.units = it.utf_u.utf_32.units,       \
            it.utf_u.utf_32.char_32                 \
        )                                           \
    };                                              \
    it.code_idx < it.this->codes;                   \
    (                                               \
        (it.utf == 8) ? (                           \
            ++it.code_idx,                          \
            it.unit_idx += it.units,                \
            it.ptr_u.ptr_8 += it.units,             \
            it.ptr = it.ptr_u.ptr_8,                \
            MARY_UTF_Decode_8(it.utf_u.utf_8,       \
                              it.ptr_u.ptr_8),      \
            it.units = it.utf_u.utf_8.units,        \
            it.char_32 = it.utf_u.utf_8.char_32     \
        ) : (it.utf == 16) ? (                      \
            ++it.code_idx,                          \
            it.unit_idx += it.units,                \
            it.ptr_u.ptr_16 += it.units,            \
            it.ptr = it.ptr_u.ptr_16,               \
            MARY_UTF_Decode_16(it.utf_u.utf_16,     \
                               it.ptr_u.ptr_16),    \
            it.units = it.utf_u.utf_16.units,       \
            it.char_32 = it.utf_u.utf_16.char_32    \
        ) : (                                       \
            ++it.code_idx,                          \
            it.unit_idx += it.units,                \
            it.ptr_u.ptr_32 += it.units,            \
            it.ptr = it.ptr_u.ptr_32,               \
            MARY_UTF_Decode_32(it.utf_u.utf_32,     \
                               it.ptr_u.ptr_32),    \
            it.units = it.utf_u.utf_32.units,       \
            it.char_32 = it.utf_u.utf_32.char_32    \
        )                                           \
    )                                               \
)

#define MARY_String_Each_Reverse(THIS)                      \
for (                                                       \
    struct {                                                \
        Mary_String_t *this;                                \
        Mary_Index_t code_idx;                              \
        Mary_Index_t unit_idx;                              \
        Mary_Void_t *ptr;                                   \
        Mary_UTF_t utf;                                     \
        union {                                             \
            Mary_Char_8_t *ptr_8;                           \
            Mary_Char_16_t *ptr_16;                         \
            Mary_Char_32_t *ptr_32;                         \
        } ptr_u;                                            \
        union {                                             \
            Mary_UTF_8_t utf_8;                             \
            Mary_UTF_16_t utf_16;                           \
            Mary_UTF_32_t utf_32;                           \
        } utf_u;                                            \
        Mary_Size_t units;                                  \
        Mary_Char_32_t char_32;                             \
    } it = {                                                \
        THIS,                                               \
        it.this->codes,                                     \
        it.this->units,                                     \
        MARY_Vector_Point_End(it.this),                     \
        MARY_String_Get_UTF(it.this),                       \
        { 0 },                                              \
        { 0 },                                              \
        0,                                                  \
        (it.utf == 8) ? (                                   \
            it.ptr_u.ptr_8 = it.ptr,                        \
            Mary_UTF_Decode_8_Reverse(&it.utf_u.utf_8,      \
                                       it.ptr_u.ptr_8),     \
            it.units = it.utf_u.utf_8.units,                \
            --it.code_idx,                                  \
            it.unit_idx -= it.units,                        \
            it.ptr_u.ptr_8 -= it.units,                     \
            it.ptr = it.ptr_u.ptr_8,                        \
            it.utf_u.utf_8.char_32                          \
        ) : (it.utf == 16) ? (                              \
            it.ptr_u.ptr_16 = it.ptr,                       \
            Mary_UTF_Decode_16_Reverse(&it.utf_u.utf_16,    \
                                        it.ptr_u.ptr_16),   \
            it.units = it.utf_u.utf_16.units,               \
            --it.code_idx,                                  \
            it.unit_idx -= it.units,                        \
            it.ptr_u.ptr_16 -= it.units,                    \
            it.ptr = it.ptr_u.ptr_16,                       \
            it.utf_u.utf_16.char_32                         \
        ) : (                                               \
            it.ptr_u.ptr_32 = it.ptr,                       \
            MARY_UTF_Decode_32_Reverse(it.utf_u.utf_32,     \
                                       it.ptr_u.ptr_32),    \
            it.units = it.utf_u.utf_32.units,               \
            --it.code_idx,                                  \
            it.unit_idx -= it.units,                        \
            it.ptr_u.ptr_32 -= it.units,                    \
            it.ptr = it.ptr_u.ptr_32,                       \
            it.utf_u.utf_32.char_32                         \
        )                                                   \
    };                                                      \
    it.code_idx + 1 > 0;                                    \
    (it.code_idx != 0) ? (                                  \
        (it.utf == 8) ? (                                   \
            MARY_UTF_Decode_8_Reverse(it.utf_u.utf_8,       \
                                      it.ptr_u.ptr_8),      \
            it.units = it.utf_u.utf_8.units,                \
            --it.code_idx,                                  \
            it.unit_idx -= it.units,                        \
            it.ptr_u.ptr_8 -= it.units,                     \
            it.ptr = it.ptr_u.ptr_8,                        \
            it.char_32 = it.utf_u.utf_8.char_32             \
        ) : (it.utf == 16) ? (                              \
            MARY_UTF_Decode_16_Reverse(it.utf_u.utf_16,     \
                                       it.ptr_u.ptr_16),    \
            it.units = it.utf_u.utf_16.units,               \
            --it.code_idx,                                  \
            it.unit_idx -= it.units,                        \
            it.ptr_u.ptr_16 -= it.units,                    \
            it.ptr = it.ptr_u.ptr_16,                       \
            it.char_32 = it.utf_u.utf_16.char_32            \
        ) : (                                               \
            MARY_UTF_Decode_32_Reverse(it.utf_u.utf_32,     \
                                       it.ptr_u.ptr_32),    \
            it.units = it.utf_u.utf_32.units,               \
            --it.code_idx,                                  \
            it.unit_idx -= it.units,                        \
            it.ptr_u.ptr_32 -= it.units,                    \
            it.ptr = it.ptr_u.ptr_32,                       \
            it.char_32 = it.utf_u.utf_32.char_32            \
        )                                                   \
    ) : (                                                   \
        --it.code_idx,                                      \
        it.char_32                                          \
    )                                                       \
)

#define MARY_String_Range(THIS, FROM, TO_EXCLUSIVE)     \
for (                                                   \
    struct {                                            \
        Mary_String_t *this;                            \
        Mary_Index_t from;                              \
        Mary_Index_t to_exclusive;                      \
        Mary_Index_t code_idx;                          \
        Mary_Index_t unit_idx;                          \
        Mary_Void_t *ptr;                               \
        Mary_UTF_t utf;                                 \
        union {                                         \
            Mary_Char_8_t *ptr_8;                       \
            Mary_Char_16_t *ptr_16;                     \
            Mary_Char_32_t *ptr_32;                     \
        } ptr_u;                                        \
        union {                                         \
            Mary_UTF_8_t utf_8;                         \
            Mary_UTF_16_t utf_16;                       \
            Mary_UTF_32_t utf_32;                       \
        } utf_u;                                        \
        Mary_Size_t units;                              \
        Mary_Char_32_t char_32;                         \
    } it = {                                            \
        THIS,                                           \
        ((FROM) < it.this->codes) ? (                   \
            FROM                                        \
        ) : (                                           \
            MARY_Assert(0, "Invalid FROM"),             \
            it.this->units                              \
        ),                                              \
        ((TO_EXCLUSIVE) <= it.this->codes) ? (          \
            TO_EXCLUSIVE                                \
        ) : (                                           \
            MARY_Assert(0, "Invalid TO_EXCLUSIVE"),     \
            it.this->units                              \
        ),                                              \
        it.from,                                        \
        Mary_String_Seek_Unit(it.this, it.code_idx),    \
        MARY_Vector_Point(it.this, it.unit_idx),        \
        MARY_String_Get_UTF(it.this),                   \
        { 0 },                                          \
        { 0 },                                          \
        0,                                              \
        (it.utf == 8) ? (                               \
            it.ptr_u.ptr_8 = it.ptr,                    \
            Mary_UTF_Decode_8(&it.utf_u.utf_8,          \
                               it.ptr_u.ptr_8),         \
            it.units = it.utf_u.utf_8.units,            \
            it.utf_u.utf_8.char_32                      \
        ) : (it.utf == 16) ? (                          \
            it.ptr_u.ptr_16 = it.ptr,                   \
            Mary_UTF_Decode_16(&it.utf_u.utf_16,        \
                                it.ptr_u.ptr_16),       \
            it.units = it.utf_u.utf_16.units,           \
            it.utf_u.utf_16.char_32                     \
        ) : (                                           \
            it.ptr_u.ptr_32 = it.ptr,                   \
            MARY_UTF_Decode_32(it.utf_u.utf_32,         \
                               it.ptr_u.ptr_32),        \
            it.units = it.utf_u.utf_32.units,           \
            it.utf_u.utf_32.char_32                     \
        )                                               \
    };                                                  \
    it.code_idx < it.to_exclusive;                      \
    (                                                   \
        (it.utf == 8) ? (                               \
            ++it.code_idx,                              \
            it.unit_idx += it.units,                    \
            it.ptr_u.ptr_8 += it.units,                 \
            it.ptr = it.ptr_u.ptr_8,                    \
            MARY_UTF_Decode_8(it.utf_u.utf_8,           \
                              it.ptr_u.ptr_8),          \
            it.units = it.utf_u.utf_8.units,            \
            it.char_32 = it.utf_u.utf_8.char_32         \
        ) : (it.utf == 16) ? (                          \
            ++it.code_idx,                              \
            it.unit_idx += it.units,                    \
            it.ptr_u.ptr_16 += it.units,                \
            it.ptr = it.ptr_u.ptr_16,                   \
            MARY_UTF_Decode_16(it.utf_u.utf_16,         \
                               it.ptr_u.ptr_16),        \
            it.units = it.utf_u.utf_16.units,           \
            it.char_32 = it.utf_u.utf_16.char_32        \
        ) : (                                           \
            ++it.code_idx,                              \
            it.unit_idx += it.units,                    \
            it.ptr_u.ptr_32 += it.units,                \
            it.ptr = it.ptr_u.ptr_32,                   \
            MARY_UTF_Decode_32(it.utf_u.utf_32,         \
                               it.ptr_u.ptr_32),        \
            it.units = it.utf_u.utf_32.units,           \
            it.char_32 = it.utf_u.utf_32.char_32        \
        )                                               \
    )                                                   \
)

#define MARY_String_Range_Reverse(THIS, FROM, TO_EXCLUSIVE) \
for (                                                       \
    struct {                                                \
        Mary_String_t *this;                                \
        Mary_Index_t from;                                  \
        Mary_Index_t to_exclusive;                          \
        Mary_Index_t code_idx;                              \
        Mary_Index_t unit_idx;                              \
        Mary_Void_t *ptr;                                   \
        Mary_UTF_t utf;                                     \
        union {                                             \
            Mary_Char_8_t *ptr_8;                           \
            Mary_Char_16_t *ptr_16;                         \
            Mary_Char_32_t *ptr_32;                         \
        } ptr_u;                                            \
        union {                                             \
            Mary_UTF_8_t utf_8;                             \
            Mary_UTF_16_t utf_16;                           \
            Mary_UTF_32_t utf_32;                           \
        } utf_u;                                            \
        Mary_Size_t units;                                  \
        Mary_Char_32_t char_32;                             \
    } it = {                                                \
        THIS,                                               \
        ((FROM) < it.this->codes) ? (                       \
            FROM                                            \
        ) : (                                               \
            MARY_Assert(0, "Invalid FROM"),                 \
            it.this->units                                  \
        ),                                                  \
        ((TO_EXCLUSIVE) <= it.this->codes) ? (              \
            TO_EXCLUSIVE                                    \
        ) : (                                               \
            MARY_Assert(0, "Invalid TO_EXCLUSIVE"),         \
            it.this->units                                  \
        ),                                                  \
        it.to_exclusive,                                    \
        Mary_String_Seek_Unit(it.this, it.code_idx),        \
        MARY_Vector_Point(it.this, it.unit_idx),            \
        MARY_String_Get_UTF(it.this),                       \
        { 0 },                                              \
        { 0 },                                              \
        0,                                                  \
        (it.utf == 8) ? (                                   \
            it.ptr_u.ptr_8 = it.ptr,                        \
            Mary_UTF_Decode_8_Reverse(&it.utf_u.utf_8,      \
                                       it.ptr_u.ptr_8),     \
            it.units = it.utf_u.utf_8.units,                \
            --it.code_idx,                                  \
            it.unit_idx -= it.units,                        \
            it.ptr_u.ptr_8 -= it.units,                     \
            it.ptr = it.ptr_u.ptr_8,                        \
            it.utf_u.utf_8.char_32                          \
        ) : (it.utf == 16) ? (                              \
            it.ptr_u.ptr_16 = it.ptr,                       \
            Mary_UTF_Decode_16_Reverse(&it.utf_u.utf_16,    \
                                        it.ptr_u.ptr_16),   \
            it.units = it.utf_u.utf_16.units,               \
            --it.code_idx,                                  \
            it.unit_idx -= it.units,                        \
            it.ptr_u.ptr_16 -= it.units,                    \
            it.ptr = it.ptr_u.ptr_16,                       \
            it.utf_u.utf_16.char_32                         \
        ) : (                                               \
            it.ptr_u.ptr_32 = it.ptr,                       \
            MARY_UTF_Decode_32_Reverse(it.utf_u.utf_32,     \
                                       it.ptr_u.ptr_32),    \
            it.units = it.utf_u.utf_32.units,               \
            --it.code_idx,                                  \
            it.unit_idx -= it.units,                        \
            it.ptr_u.ptr_32 -= it.units,                    \
            it.ptr = it.ptr_u.ptr_32,                       \
            it.utf_u.utf_32.char_32                         \
        )                                                   \
    };                                                      \
    it.code_idx + 1 > it.from;                              \
    (it.code_idx != 0) ? (                                  \
        (it.utf == 8) ? (                                   \
            MARY_UTF_Decode_8_Reverse(it.utf_u.utf_8,       \
                                      it.ptr_u.ptr_8),      \
            it.units = it.utf_u.utf_8.units,                \
            --it.code_idx,                                  \
            it.unit_idx -= it.units,                        \
            it.ptr_u.ptr_8 -= it.units,                     \
            it.ptr = it.ptr_u.ptr_8,                        \
            it.char_32 = it.utf_u.utf_8.char_32             \
        ) : (it.utf == 16) ? (                              \
            MARY_UTF_Decode_16_Reverse(it.utf_u.utf_16,     \
                                       it.ptr_u.ptr_16),    \
            it.units = it.utf_u.utf_16.units,               \
            --it.code_idx,                                  \
            it.unit_idx -= it.units,                        \
            it.ptr_u.ptr_16 -= it.units,                    \
            it.ptr = it.ptr_u.ptr_16,                       \
            it.char_32 = it.utf_u.utf_16.char_32            \
        ) : (                                               \
            MARY_UTF_Decode_32_Reverse(it.utf_u.utf_32,     \
                                       it.ptr_u.ptr_32),    \
            it.units = it.utf_u.utf_32.units,               \
            --it.code_idx,                                  \
            it.unit_idx -= it.units,                        \
            it.ptr_u.ptr_32 -= it.units,                    \
            it.ptr = it.ptr_u.ptr_32,                       \
            it.char_32 = it.utf_u.utf_32.char_32            \
        )                                                   \
    ) : (                                                   \
        --it.code_idx,                                      \
        it.char_32                                          \
    )                                                       \
)
