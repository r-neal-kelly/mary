#include <stdlib.h>
#include <string.h>
#include <Mary/Varray.h>

MARY_Primitives;

void Mary_Varray_Create(Mary_Varray_t *varray, size_t opt_bytes, size_t opt_indices)
{
  size_t bytes = (opt_bytes) ? (opt_bytes) : 1;
  void *data = malloc(bytes); MARY_Assert(data != 0, 0);
  varray->data = data, varray->bytes = bytes;
  Mary_Vector_Create(&varray->indices, MARY_Allocator_Heap, sizeof(Mary_Varray_Index_t), opt_indices); // this should have an allocator param instead.
}

void Mary_Varray_Destroy(Mary_Varray_t *varray)
{
  free(varray->data);
  Mary_Vector_Destroy(&varray->indices);
}

void Mary_Varray_Reserve(Mary_Varray_t *varray, size_t bytes)
{
  if (bytes > varray->bytes)
  {
    void *data = realloc(varray->data, bytes); MARY_Assert(data != 0, 0);
    varray->data = data, varray->bytes = bytes;
  }
}

void Mary_Varray_Fit(Mary_Varray_t *varray)
{

}

void Mary_Varray_Push_Back(Mary_Varray_t *varray, void *in_elem, size_t elem_bytes)
{
  u8 *data = varray->data; size_t offset = 0;
  if (varray->indices.units)
  {
    Mary_Varray_Index_t *last_idx = MARY_Vector_Point(&varray->indices, varray->indices.units - 1);
    data = (u8 *)last_idx->data + last_idx->bytes;
    offset = last_idx->offset + last_idx->bytes;
  }
  Mary_Varray_Reserve(varray, offset + elem_bytes); // I need to update data ptr if true!!! major bug waiting to happen!!!
  Mary_Varray_Index_t new_idx = { data, elem_bytes, offset };
  MARY_Vector_Push(&varray->indices, Mary_Varray_Index_t, new_idx);
  memcpy(data, in_elem, elem_bytes);
}
