#include <Mary/Vector.h>

MARY_Primitives;

void Mary_Vector_Create(Mary_Vector_t *this, Mary_Allocator_t allocator, Mary_Size_t unit, Mary_Size_t opt_reserve_units)
{
    MARY_Assert(allocator.type != MARY_ALLOCATOR_STATIC &&
                allocator.type != MARY_ALLOCATOR_STACK,
                "Must allocate static or stack locally. "
                "Try MARY_Vector_Static or MARY_Vector_Stack.");

    this->allocator = allocator;
    this->bytes = (opt_reserve_units) ? (opt_reserve_units * unit) : unit;
    this->data = Mary_Allocator_Alloc(this->allocator, this->bytes);
    this->unit = unit;
    this->units = 0;
}

void Mary_Vector_Create_At(Mary_Vector_t *this, void *at_data, Mary_Size_t at_bytes, Mary_Allocator_t at_allocator, Mary_Size_t at_unit)
{
    this->allocator = at_allocator;
    this->bytes = at_bytes;
    this->data = at_data;
    this->unit = at_unit;
    this->units = 0;
}

void Mary_Vector_Create_With(Mary_Vector_t *this, void *with_data, Mary_Size_t with_bytes, Mary_Allocator_t with_allocator, Mary_Size_t with_unit, Mary_Size_t with_units)
{
    this->allocator = with_allocator;
    this->bytes = with_bytes;
    this->data = with_data;
    this->unit = with_unit;
    this->units = with_units;
}

void Mary_Vector_Repurpose(Mary_Vector_t *this, Mary_Size_t unit, Mary_Size_t opt_reserve_units)
{
    this->unit = unit;
    this->units = 0;
    Mary_Vector_Reserve(this, opt_reserve_units);
}

void Mary_Vector_Repurpose_With(Mary_Vector_t *this, Mary_p *with_ptr, Mary_Size_t with_unit, Mary_Size_t with_units)
{
    this->unit = with_unit;
    this->units = with_units;
    if (this->units * this->unit > this->bytes) {
        Mary_Vector_Grow(this);
    }
    Mary_Copy(with_ptr->data, this->data, with_unit * with_units);
}

void Mary_Vector_Copy(Mary_Vector_t *from, Mary_Vector_t *to)
{
    MARY_Assert(to->data != 0, "Param 'to' has not been allocated.");
    MARY_Assert(from->unit == to->unit, "Non-matching unit.");

    Mary_Vector_Reserve(to, from->units);
    Mary_Copy(from->data, to->data, from->unit * from->units);
    to->units = from->units;
}

void Mary_Vector_Destroy(Mary_Vector_t *this)
{
    MARY_Assert(this->allocator.type != MARY_ALLOCATOR_STATIC &&
                this->allocator.type != MARY_ALLOCATOR_STACK,
                "Trying to deallocate static or stack memory.");

    Mary_Allocator_Dealloc(this->allocator, this->data);
}

void Mary_Vector_Reserve(Mary_Vector_t *this, Mary_Size_t units)
{
    Mary_Size_t new_bytes = units * this->unit;
    if (new_bytes > this->bytes) {
        MARY_Assert(this->allocator.type != MARY_ALLOCATOR_STATIC &&
                    this->allocator.type != MARY_ALLOCATOR_STACK,
                    "Trying to reallocate static or stack memory.");

        this->data = Mary_Allocator_Realloc(this->allocator, this->data, new_bytes);
        this->bytes = new_bytes;
    }
}

void Mary_Vector_Grow(Mary_Vector_t *this)
{
    MARY_Assert(this->allocator.type != MARY_ALLOCATOR_STATIC &&
                this->allocator.type != MARY_ALLOCATOR_STACK,
                "Trying to reallocate static or stack memory.");

    this->bytes = (u64)(this->units * MARY_Vector_Grow_Rate + 1) * this->unit;
    this->data = Mary_Allocator_Realloc(this->allocator, this->data, this->bytes);
}

void Mary_Vector_Fit(Mary_Vector_t *this)
{
    Mary_Size_t fit_bytes = this->units ? this->unit * this->units : this->unit;
    if (this->bytes != fit_bytes) {
        MARY_Assert(this->allocator.type != MARY_ALLOCATOR_STATIC &&
                    this->allocator.type != MARY_ALLOCATOR_STACK,
                    "Trying to reallocate static or stack memory.");

        this->data = Mary_Allocator_Realloc(this->allocator, this->data, fit_bytes);
        this->bytes = fit_bytes;
    }
}

void Mary_Vector_Push_At(Mary_Vector_t *this, Mary_Index_t idx, void *in_elem)
{
    MARY_Assert(MARY_Vector_Has_Index(this, idx), "Invalid index.");

    if (this->units + 1 * this->unit > this->bytes) {
        Mary_Vector_Grow(this);
    }
    u8 *idx_ptr = MARY_Vector_Point(this, idx);
    Mary_Move(idx_ptr, idx_ptr + this->unit, (this->units - idx) * this->unit);
    Mary_Copy(in_elem, idx_ptr, this->unit);
    ++this->units;
}

void Mary_Vector_Pop_At(Mary_Vector_t *this, Mary_Index_t idx, void *out_elem)
{
    MARY_Assert(MARY_Vector_Has_Index(this, idx), "Invalid index.");

    u8 *idx_ptr = MARY_Vector_Point(this, idx);
    Mary_Copy(idx_ptr, out_elem, this->unit);
    Mary_Move(idx_ptr + this->unit, idx_ptr, (--this->units - idx) * this->unit);
}

void Mary_Vector_Erase(Mary_Vector_t *this, void *in_elem)
{
    Mary_Bool_t was_found = 0; Mary_Index_t idx =
        Mary_Vector_Index_Of(this, in_elem, &was_found);
    if (was_found) {
        MARY_Assert(MARY_Vector_Has_Index(this, idx), "Invalid index.");

        u8 *idx_ptr = MARY_Vector_Point(this, idx);
        Mary_Move(idx_ptr + this->unit, idx_ptr, (--this->units - idx) * this->unit);
    }
}

void Mary_Vector_Erase_At(Mary_Vector_t *this, Mary_Index_t idx)
{
    MARY_Assert(MARY_Vector_Has_Index(this, idx), "Invalid index.");

    u8 *idx_ptr = MARY_Vector_Point(this, idx);
    Mary_Move(idx_ptr + this->unit, idx_ptr, (--this->units - idx) * this->unit);
}

void Mary_Vector_Append_Back(Mary_Vector_t *this, Mary_Vector_t *back)
{
    MARY_Assert(back->data != 0, "'back' has not been allocated.");
    MARY_Assert(this->unit == back->unit, "Non-matching unit.");

    Mary_Vector_Reserve(this, this->units + back->units);
    Mary_Copy(back->data, MARY_Vector_Point_End(this), back->units * back->unit);
    this->units += back->units;
}

Mary_Index_t Mary_Vector_Index_Of(Mary_Vector_t *this, void *in_elem, Mary_Bool_t *out_was_found)
{
    I i = 0;
    u8 *p = MARY_Vector_Point_Front(this);
    *out_was_found = MARY_FALSE;
    for (; i < this->units; ++i, p += this->unit) {
        if (Mary_Is_Same(p, in_elem, this->unit)) {
            *out_was_found = MARY_TRUE;
            break;
        }
    }
    return i;
}

Mary_Index_t Mary_Vector_Index_Of_Reverse(Mary_Vector_t *this, void *in_elem, Mary_Bool_t *out_was_found)
{
    I i = this->units - 1;
    u8 *p = MARY_Vector_Point_Back(this);
    *out_was_found = MARY_FALSE;
    for (; i + 1 > 0; --i, p -= this->unit) {
        if (Mary_Is_Same(p, in_elem, this->unit)) {
            *out_was_found = MARY_TRUE;
            break;
        }
    }
    return i;
}

////////////////

/*
void Mary_Vector_Add_Slice(Mary_Vector_t *vector, size_t from, size_t to_exclusive, void *opt_in_elems)
{
  size_t bytes = vector->bytes;
  size_t unit = vector->unit;
  size_t old_units = vector->units;
  size_t slice_units = to_exclusive - from;
  size_t new_units = old_units + slice_units;
  if (new_units * unit > bytes)
  {
    Mary_Vector_Reserve(vector, new_units);
  }
  void *data = vector->data;
  u8 *p_0 = (u8 *)data;
  u8 *p_from = p_0 + from * unit;
  u8 *p_to = p_0 + to_exclusive * unit;
  memmove(p_to, p_from, (old_units - from) * unit);
  if (opt_in_elems)
  {
    memcpy(p_from, opt_in_elems, slice_units * unit);
  }
  vector->units = new_units;
}

void Mary_Vector_Delete_Slice(Mary_Vector_t *vector, size_t from, size_t to_exclusive)
{
  void *data = vector->data;
  size_t unit = vector->unit;
  size_t units = vector->units;
  u8 *p_0 = (u8 *)data;
  u8 *p_from = p_0 + from * unit;
  u8 *p_to = p_0 + to_exclusive * unit;
  memmove(p_from, p_to, (units - to_exclusive) * unit);
  vector->units = units - (to_exclusive - from);
}

void Mary_Vector_Copy_Slice(Mary_Vector_t *vector, size_t from, size_t to_exclusive, void *out_elems)
{
  void *data = vector->data;
  size_t unit = vector->unit;
  u8 *p_0 = (u8 *)data;
  u8 *p_from = p_0 + from * unit;
  u8 *p_to = p_0 + to_exclusive * unit;
  memmove(out_elems, p_from, (to_exclusive - from) * unit);
}

void Mary_Vector_Take_Slice(Mary_Vector_t *vector, size_t from, size_t to_exclusive, void *out_elems)
{
  void *data = vector->data;
  size_t unit = vector->unit;
  size_t units = vector->units;
  u8 *p_0 = (u8 *)data;
  u8 *p_from = p_0 + from * unit;
  u8 *p_to = p_0 + to_exclusive * unit;
  memmove(out_elems, p_from, (to_exclusive - from) * unit);
  memmove(p_from, p_to, (units - to_exclusive) * unit);
  vector->units = units - (to_exclusive - from);
}

void Mary_Vector_Put_Slice(Mary_Vector_t *v, size_t from, size_t to_exclusive, void *in_elems)
{
  memcpy((u8 *)v->data + from * v->unit, in_elems, (to_exclusive - from) * v->unit);
}
*/
