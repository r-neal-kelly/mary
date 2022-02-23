#pragma once

#include <Mary/Utils.h>
#include <Mary/Allocator.h>
#include <Mary/UTF.h>
#include <Mary/Vector.h>
#include <Mary/C_String.h>

typedef struct Mary_String_t Mary_String_t;

struct Mary_String_t {
    MARY_Vector_t;
    Mary_Size_t codes;
};

void Mary_String_Create(Mary_String_t *this, Mary_Allocator_t allocator, Mary_UTF_t utf, Mary_Size_t opt_reserve_units);
void Mary_String_Create_At(Mary_String_t *this, void *at_data, Mary_Size_t at_bytes, Mary_Allocator_t at_allocator, Mary_UTF_t at_utf);
void Mary_String_Create_With(Mary_String_t *this, void *with_data, Mary_Size_t opt_with_bytes, Mary_Allocator_t with_allocator, Mary_UTF_t with_utf);
void Mary_String_Create_From(Mary_String_t *this, Mary_Allocator_t allocator, Mary_UTF_t utf, void *from_data, Mary_UTF_t from_utf);
void Mary_String_Destroy(Mary_String_t *this);
void Mary_String_Copy(Mary_String_t *from, Mary_String_t *to);
void Mary_String_Recode(Mary_String_t *this, Mary_UTF_t to_utf);
void Mary_String_Assign(Mary_String_t *this, Mary_String_t *str);
void Mary_String_Append_Front(Mary_String_t *this, Mary_String_t *front);
void Mary_String_Append_Back(Mary_String_t *this, Mary_String_t *back);
void Mary_String_Trim(Mary_String_t *this);
Mary_Index_t Mary_String_Seek_Unit(Mary_String_t *this, Mary_Index_t code_idx);

#define MARY_String_Static(NAME, UTF, C_STR)
#define MARY_String_Stack(NAME, UTF, C_STR)
#define MARY_String_Heap(NAME, UTF, C_STR)
#define MARY_String_Pool(NAME, POOL, UTF, C_STR)
#define MARY_String_Frame(NAME, UTF, C_STR)
#define MARY_String_Chain(NAME, UTF, C_STR)
#define MARY_String_Heap_p(THIS, UTF, C_STR)
#define MARY_String_Pool_p(THIS, POOL, UTF, C_STR)
#define MARY_String_Frame_p(THIS, UTF, C_STR)
#define MARY_String_Chain_p(THIS, UTF, C_STR)
#define MARY_String_Is_Created(THIS)
#define MARY_String_Get_UTF(THIS)
#define MARY_String_Point_Front(THIS)
#define MARY_String_Point_Back(THIS)
#define MARY_String_Point_End(THIS)
#define MARY_String_Assign(THIS, UTF, C_STR)
#define MARY_String_Append_Front(THIS, UTF, C_STR)
#define MARY_String_Append_Back(THIS, UTF, C_STR)
#define MARY_String_Each(THIS)
#define MARY_String_Each_Reverse(THIS)
#define MARY_String_Range(THIS, FROM, TO_EXCLUSIVE)
#define MARY_String_Range_Reverse(THIS, FROM, TO_EXCLUSIVE)
#define MARY_String_Range_Advance(INDICES)
#define MARY_String_Range_Regress(INDICES)
#define MARY_String_Range_Alter(FROM, TO_EXCLUSIVE) // maybe not?

#include <Mary/Macros/String_m.h>

//////////////////////////////////////////////////////////

// Split()
// Join()
// Cluster() should group the clusters in the string together (clusters are letters along with combining marks, it's lang specific)
// Alphabet() should return whatever lang it is, see Lang.h

// we could make a split that simply puts 0 at the start of each section where it should split, and return a vector of ptrs.
// split on & [a, b, c, & = 0, d, e, f, & = 0...] -> [&a, &d]
// join will put a split back together with whatever string is given by user.
