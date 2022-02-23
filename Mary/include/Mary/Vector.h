#pragma once

#include <Mary/Utils.h>
#include <Mary/Allocator.h>

typedef struct Mary_Vector_t Mary_Vector_t;

#define MARY_Vector_t\
    MARY_Pointer_t;\
    Mary_Size_t unit;\
    Mary_Size_t units

struct Mary_Vector_t {
    MARY_Vector_t;
};

// might want to add default units #define
#define MARY_Vector_Grow_Rate 1.7
#define MARY_Vector_Namespace <Mary/Namespace/Vector_In.h>
#define MARY_Vector_Namespace_In <Mary/Namespace/Vector_In.h>
#define MARY_Vector_Namespace_Out <Mary/Namespace/Vector_Out.h>

void Mary_Vector_Create(Mary_Vector_t *this, Mary_Allocator_t allocator, Mary_Size_t unit, Mary_Size_t opt_reserve_units);
void Mary_Vector_Create_At(Mary_Vector_t *this, void *at_data, Mary_Size_t at_bytes, Mary_Allocator_t at_allocator, Mary_Size_t at_unit);
void Mary_Vector_Create_With(Mary_Vector_t *this, void *with_data, Mary_Size_t with_bytes, Mary_Allocator_t with_allocator, Mary_Size_t with_unit, Mary_Size_t with_units);
//void Mary_Vector_Create_From(Mary_Vector_t *this); // like String_t
void Mary_Vector_Repurpose(Mary_Vector_t *this, Mary_Size_t unit, Mary_Size_t opt_reserve_units);
void Mary_Vector_Repurpose_With(Mary_Vector_t *this, Mary_p *with_ptr, Mary_Size_t with_unit, Mary_Size_t with_units); // keep Mary_p?
void Mary_Vector_Copy(Mary_Vector_t *from, Mary_Vector_t *to);
void Mary_Vector_Destroy(Mary_Vector_t *this);
void Mary_Vector_Reserve(Mary_Vector_t *this, Mary_Size_t units);
void Mary_Vector_Grow(Mary_Vector_t *this);
void Mary_Vector_Fit(Mary_Vector_t *this);
void Mary_Vector_Push_At(Mary_Vector_t *this, Mary_Index_t idx, void *in_elem);
void Mary_Vector_Pop_At(Mary_Vector_t *this, Mary_Index_t idx, void *out_elem);
void Mary_Vector_Erase(Mary_Vector_t *this, void *in_elem);
void Mary_Vector_Erase_At(Mary_Vector_t *this, Mary_Index_t idx);
void Mary_Vector_Append_Back(Mary_Vector_t *this, Mary_Vector_t *back);
Mary_Index_t Mary_Vector_Index_Of(Mary_Vector_t *this, void *in_elem, Mary_Bool_t *out_was_found);
Mary_Index_t Mary_Vector_Index_Of_Reverse(Mary_Vector_t *this, void *in_elem, Mary_Bool_t *out_was_found);

#define MARY_Vector_Static(NAME, TYPE, RESERVE_UNITS)
#define MARY_Vector_Stack(NAME, TYPE, RESERVE_UNITS)
#define MARY_Vector_Heap(NAME, TYPE, OPT_RESERVE_UNITS)
#define MARY_Vector_Pool(NAME, POOL, TYPE, OPT_RESERVE_UNITS)
#define MARY_Vector_Frame(NAME, TYPE, OPT_RESERVE_UNITS)
#define MARY_Vector_Chain(NAME, TYPE, OPT_RESERVE_UNITS)
#define MARY_Vector_Heap_p(THIS, TYPE, OPT_RESERVE_UNITS)
#define MARY_Vector_Pool_p(THIS, POOL, TYPE, OPT_RESERVE_UNITS)
#define MARY_Vector_Frame_p(THIS, TYPE, OPT_RESERVE_UNITS)
#define MARY_Vector_Chain_p(THIS, TYPE, OPT_RESERVE_UNITS)
#define MARY_Vector_Has_Index(THIS, IDX)
#define MARY_Vector_Is_Empty(THIS)
#define MARY_Vector_Point(THIS, IDX)
#define MARY_Vector_Point_Front(THIS)
#define MARY_Vector_Point_Back(THIS)
#define MARY_Vector_Point_End(THIS)
#define MARY_Vector_At(THIS, TYPE, IDX)
#define MARY_Vector_At_Front(THIS, TYPE)
#define MARY_Vector_At_Back(THIS, TYPE)
#define MARY_Vector_Push(THIS, TYPE, ITEM)
#define MARY_Vector_Pop(THIS, TYPE)
#define MARY_Vector_Point_Push(THIS)
#define MARY_Vector_Point_Pop(THIS)
#define MARY_Vector_Empty(THIS)
#define MARY_Vector_Cast(THIS)
#define MARY_Vector_Each(THIS, TYPE)
#define MARY_Vector_Each_Reverse(THIS, TYPE)
#define MARY_Vector_Range(THIS, TYPE, FROM, TO_EXCLUSIVE)
#define MARY_Vector_Range_Reverse(THIS, TYPE, FROM, TO_EXCLUSIVE)
#define MARY_Vector_Range_Advance(INDICES)
#define MARY_Vector_Range_Regress(INDICES)
#define MARY_Vector_Binary_Search(THIS, FROM, TO, OUT_IDX, OUT_PTR, OUT_WAS_FOUND, PTR_IS_LESS, PTR_IS_MORE)

#include <Mary/Macros/Vector_m.h>

// I think we should separate what we use from ones we don't and reorganize. above this line, we have reviewed each function.
// these may be useful for strings more than vectors. not sure what to do with them.
void Mary_Vector_Add_Slice(Mary_Vector_t *this, size_t from, size_t to_exclusive, void *opt_in_elems);
void Mary_Vector_Delete_Slice(Mary_Vector_t *this, size_t from, size_t to_exclusive);
void Mary_Vector_Copy_Slice(Mary_Vector_t *this, size_t from, size_t to_exclusive, void *out_elems);
void Mary_Vector_Take_Slice(Mary_Vector_t *this, size_t from, size_t to_exclusive, void *out_elems);
void Mary_Vector_Put_Slice(Mary_Vector_t *this, size_t from, size_t to_exclusive, void *in_elems);
//void Mary_Vector_Sort(Mary_Vector_t *this);
