#include <Mary/Vector.h>

typedef Mary_Vector_t Vector_t;

static void (*const Vector_Create)(Mary_Vector_t *this, Mary_Allocator_t allocator, Mary_Size_t unit, Mary_Size_t opt_reserve_units) = Mary_Vector_Create;
static void (*const Vector_Create_At)(Mary_Vector_t *this, void *at_data, Mary_Size_t at_bytes, Mary_Allocator_t at_allocator, Mary_Size_t at_unit) = Mary_Vector_Create_At;
static void (*const Vector_Create_With)(Mary_Vector_t *this, void *with_data, Mary_Size_t with_bytes, Mary_Allocator_t with_allocator, Mary_Size_t with_unit, Mary_Size_t with_units) = Mary_Vector_Create_With;
static void (*const Vector_Repurpose)(Mary_Vector_t *this, Mary_Size_t unit, Mary_Size_t opt_reserve_units) = Mary_Vector_Repurpose;
static void (*const Vector_Repurpose_With)(Mary_Vector_t *this, Mary_p *with_ptr, Mary_Size_t with_unit, Mary_Size_t with_units) = Mary_Vector_Repurpose_With;
static void (*const Vector_Copy)(Mary_Vector_t *from, Mary_Vector_t *to) = Mary_Vector_Copy;
static void (*const Vector_Destroy)(Mary_Vector_t *this) = Mary_Vector_Destroy;
static void (*const Vector_Reserve)(Mary_Vector_t *this, Mary_Size_t units) = Mary_Vector_Reserve;
static void (*const Vector_Grow)(Mary_Vector_t *this) = Mary_Vector_Grow;
static void (*const Vector_Fit)(Mary_Vector_t *this) = Mary_Vector_Fit;
static void (*const Vector_Push_At)(Mary_Vector_t *this, Mary_Index_t idx, void *in_elem) = Mary_Vector_Push_At;
static void (*const Vector_Pop_At)(Mary_Vector_t *this, Mary_Index_t idx, void *out_elem) = Mary_Vector_Pop_At;
static void (*const Vector_Erase)(Mary_Vector_t *this, void *in_elem) = Mary_Vector_Erase;
static void (*const Vector_Erase_At)(Mary_Vector_t *this, Mary_Index_t idx) = Mary_Vector_Erase_At;
static Mary_Index_t (*const Vector_Index_Of)(Mary_Vector_t *this, void *in_elem, Mary_Bool_t *out_was_found) = Mary_Vector_Index_Of;
static Mary_Index_t (*const Vector_Index_Of_Reverse)(Mary_Vector_t *this, void *in_elem, Mary_Bool_t *out_was_found) = Mary_Vector_Index_Of_Reverse;

#define VECTOR_Static(NAME, TYPE, RESERVE_UNITS) MARY_Vector_Static(NAME, TYPE, RESERVE_UNITS)
#define VECTOR_Stack(NAME, TYPE, RESERVE_UNITS) MARY_Vector_Stack(NAME, TYPE, RESERVE_UNITS)
#define VECTOR_Heap(NAME, TYPE, OPT_RESERVE_UNITS) MARY_Vector_Heap(NAME, TYPE, OPT_RESERVE_UNITS)
#define VECTOR_Pool(NAME, POOL, TYPE, OPT_RESERVE_UNITS) MARY_Vector_Pool(NAME, POOL, TYPE, OPT_RESERVE_UNITS)
#define VECTOR_Frame(NAME, TYPE, OPT_RESERVE_UNITS) MARY_Vector_Frame(NAME, TYPE, OPT_RESERVE_UNITS)
#define VECTOR_Chain(NAME, TYPE, OPT_RESERVE_UNITS) MARY_Vector_Chain(NAME, TYPE, OPT_RESERVE_UNITS)
#define VECTOR_Heap_p(THIS, TYPE, OPT_RESERVE_UNITS) MARY_Vector_Heap_p(THIS, TYPE, OPT_RESERVE_UNITS)
#define VECTOR_Pool_p(THIS, POOL, TYPE, OPT_RESERVE_UNITS) MARY_Vector_Pool_p(THIS, POOL, TYPE, OPT_RESERVE_UNITS)
#define VECTOR_Frame_p(THIS, TYPE, OPT_RESERVE_UNITS) MARY_Vector_Frame_p(THIS, TYPE, OPT_RESERVE_UNITS)
#define VECTOR_Chain_p(THIS, TYPE, OPT_RESERVE_UNITS) MARY_Vector_Chain_p(THIS, TYPE, OPT_RESERVE_UNITS)
#define VECTOR_Has_Index(THIS, IDX) MARY_Vector_Has_Index(THIS, IDX)
#define VECTOR_Is_Empty(THIS) MARY_Vector_Is_Empty(THIS)
#define VECTOR_Point(THIS, IDX) MARY_Vector_Point(THIS, IDX)
#define VECTOR_Point_Front(THIS) MARY_Vector_Point_Front(THIS)
#define VECTOR_Point_Back(THIS) MARY_Vector_Point_Back(THIS)
#define VECTOR_Point_End(THIS) MARY_Vector_Point_End(THIS)
#define VECTOR_At(THIS, TYPE, IDX) MARY_Vector_At(THIS, TYPE, IDX)
#define VECTOR_At_Front(THIS, TYPE) MARY_Vector_At_Front(THIS, TYPE)
#define VECTOR_At_Back(THIS, TYPE) MARY_Vector_At_Back(THIS, TYPE)
#define VECTOR_Push(THIS, TYPE, ITEM) MARY_Vector_Push(THIS, TYPE, ITEM)
#define VECTOR_Pop(THIS, TYPE) MARY_Vector_Pop(THIS, TYPE)
#define VECTOR_Point_Push(THIS) MARY_Vector_Point_Push(THIS)
#define VECTOR_Point_Pop(THIS) MARY_Vector_Point_Pop(THIS)
#define VECTOR_Empty(THIS) MARY_Vector_Empty(THIS)
#define VECTOR_Cast(THIS) MARY_Vector_Cast(THIS)
#define VECTOR_Each(THIS, TYPE) MARY_Vector_Each(THIS, TYPE)
#define VECTOR_Each_Reverse(THIS, TYPE) MARY_Vector_Each_Reverse(THIS, TYPE)
#define VECTOR_Range(THIS, TYPE, FROM, TO_EXCLUSIVE) MARY_Vector_Range(THIS, TYPE, FROM, TO_EXCLUSIVE)
#define VECTOR_Range_Reverse(THIS, TYPE, FROM, TO_EXCLUSIVE) MARY_Vector_Range_Reverse(THIS, TYPE, FROM, TO_EXCLUSIVE)
#define VECTOR_Range_Advance(INDICES) MARY_Vector_Range_Advance(INDICES)
#define VECTOR_Range_Regress(INDICES) MARY_Vector_Range_Regress(INDICES)
