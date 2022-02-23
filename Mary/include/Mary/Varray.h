#pragma once

#include <Mary/Utils.h>
#include <Mary/Vector.h>

typedef struct
{
  MARY_Pointer_t;
  Mary_Vector_t indices;
}
Mary_Varray_t;

typedef struct
{
  void *data;
  size_t bytes;
  size_t offset;
}
Mary_Varray_Index_t;

void Mary_Varray_Create(Mary_Varray_t *varray, size_t opt_bytes, size_t opt_indices);
void Mary_Varray_Destroy(Mary_Varray_t *varray);
void Mary_Varray_Reserve(Mary_Varray_t *varray, size_t bytes);
void Mary_Varray_Fit(Mary_Varray_t *varray);
void Mary_Varray_Push_Back(Mary_Varray_t *varray, void *in_elem, size_t elem_bytes);
