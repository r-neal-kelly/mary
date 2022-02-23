#include <Mary/String.h>
#include <Mary/Regex.h>
#include MARY_Vector_Namespace

MARY_Primitives;

void Mary_String_Create(Mary_String_t *this, Mary_Allocator_t allocator, Mary_UTF_t utf, Mary_Size_t opt_reserve_units)
{
    MARY_Assert(allocator.type != MARY_ALLOCATOR_STATIC && allocator.type != MARY_ALLOCATOR_STACK,
                "Must allocate static or stack locally. Try MARY_String_Static or MARY_String_Stack.");
    MARY_Assert(utf == 8 || utf == 16 || utf == 32,
                "Invalid 'utf'. It must be 8, 16, or 32.");

    this->allocator = allocator;
    this->unit = MARY_UTF_To_Unit(utf);
    this->bytes = opt_reserve_units ? opt_reserve_units * this->unit : this->unit;
    this->data = Mary_Allocator_Alloc(this->allocator, this->bytes);
    U zero = 0; Mary_Copy(&zero, this->data, this->unit);
    this->units = 1;
    this->codes = 1;
}

void Mary_String_Create_At(Mary_String_t *this, void *at_data, Mary_Size_t at_bytes, Mary_Allocator_t at_allocator, Mary_UTF_t at_utf)
{
    MARY_Assert(at_data != 0,
                "Cannot create at a null pointer.");
    MARY_Assert(at_bytes > 0,
                "Cannot create at a pointer with zero bytes.");
    MARY_Assert(at_utf == 8 || at_utf == 16 || at_utf == 32,
                "Invalid 'at_utf'. It must be 8, 16, or 32.");

    this->allocator = at_allocator;
    this->unit = MARY_UTF_To_Unit(at_utf);
    this->bytes = at_bytes;
    this->data = at_data;
    U zero = 0; Mary_Copy(&zero, this->data, this->unit);
    this->units = 1;
    this->codes = 1;
}

void Mary_String_Create_With(Mary_String_t *this, void *with_data, Mary_Size_t opt_with_bytes, Mary_Allocator_t with_allocator, Mary_UTF_t with_utf)
{
    MARY_Assert(with_data != 0,
                "Cannot create with a null pointer.");
    MARY_Assert(with_utf == 8 || with_utf == 16 || with_utf == 32,
                "Invalid 'with_utf'. It must be 8, 16, or 32.");

    Mary_C_String_Size_t with_size;
    Mary_C_String_Count(with_data, with_utf, MARY_TRUE, &with_size);

    this->allocator = with_allocator;
    this->unit = MARY_UTF_To_Unit(with_utf);
    this->bytes = opt_with_bytes ? opt_with_bytes : with_size.bytes;
    this->data = with_data;
    this->units = with_size.units;
    this->codes = with_size.codes;
}

void Mary_String_Create_From(Mary_String_t *this, Mary_Allocator_t allocator, Mary_UTF_t utf, void *from_data, Mary_UTF_t from_utf)
{
    MARY_Assert(allocator.type != MARY_ALLOCATOR_STATIC && allocator.type != MARY_ALLOCATOR_STACK,
                "Must allocate static or stack locally. Try MARY_String_Static or MARY_String_Stack.");
    MARY_Assert(utf == 8 || utf == 16 || utf == 32,
                "'utf' must be 8, 16, or 32.");
    MARY_Assert(from_data != 0,
                "Cannot create from a null pointer.");
    MARY_Assert(from_utf == 8 || from_utf == 16 || from_utf == 32,
                "'from_utf' must be 8, 16, or 32.");

    MARY_String_Static(from_str, from_utf, from_data);
    Mary_String_Create(this, allocator, utf, from_str.codes * MARY_UTF_To_Max_Units(utf));
    Mary_String_Copy(&from_str, this);
}

void Mary_String_Destroy(Mary_String_t *this)
{
    MARY_Assert(this->allocator.type != MARY_ALLOCATOR_STATIC && this->allocator.type != MARY_ALLOCATOR_STACK,
                "Cannot deallocate static or stack memory.");

    Mary_Allocator_Dealloc(this->allocator, this->data);
}

void Mary_String_Copy(Mary_String_t *from, Mary_String_t *to)
{
    MARY_Assert(to->data != 0,
                "'to' has not been allocated.");

    Mary_UTF_t from_utf = MARY_Unit_To_UTF(from->unit);
    Mary_UTF_t to_utf = MARY_Unit_To_UTF(to->unit);
    Mary_Vector_t *to_vec = VECTOR_Cast(to);

    if (from_utf == to_utf) {
        Vector_Copy(VECTOR_Cast(from), to_vec);
    } else {
        if (from_utf == 8 && to_utf == 16) {
            Vector_Reserve(to_vec, from->codes * 2);
            u8 *f = from->data;
            u16 *t = to->data;
            Mary_UTF_8_t utf_8; utf_8.char_32 = 1;
            Mary_UTF_16_t utf_16;
            while (utf_8.char_32 != 0) {
                MARY_UTF_Decode_8(utf_8, f);
                f += utf_8.units;
                MARY_UTF_Encode_16(
                    utf_16, utf_8.char_32,
                    (*t++ = utf_16.a),
                    (*t++ = utf_16.a, *t++ = utf_16.b)
                );
            }
            to->units = t - (u16 *)to->data;
        } else if (from_utf == 8 && to_utf == 32) {
            Vector_Reserve(to_vec, from->codes);
            u8 *f = from->data;
            u32 *t = to->data;
            Mary_UTF_8_t utf_8; utf_8.char_32 = 1;
            while (utf_8.char_32 != 0) {
                MARY_UTF_Decode_8(utf_8, f);
                f += utf_8.units;
                (*t++ = utf_8.char_32);
            }
            to->units = t - (u32 *)to->data;
        } else if (from_utf == 16 && to_utf == 8) {
            Vector_Reserve(to_vec, from->codes * 4);
            u16 *f = from->data;
            u8 *t = to->data;
            Mary_UTF_16_t utf_16; utf_16.char_32 = 1;
            Mary_UTF_8_t utf_8;
            while (utf_16.char_32 != 0) {
                MARY_UTF_Decode_16(utf_16, f);
                f += utf_16.units;
                MARY_UTF_Encode_8(
                    utf_8, utf_16.char_32,
                    (*t++ = utf_8.a),
                    (*t++ = utf_8.a, *t++ = utf_8.b),
                    (*t++ = utf_8.a, *t++ = utf_8.b, *t++ = utf_8.c),
                    (*t++ = utf_8.a, *t++ = utf_8.b, *t++ = utf_8.c, *t++ = utf_8.d)
                );
            }
            to->units = t - (u8 *)to->data;
        } else if (from_utf == 16 && to_utf == 32) {
            Vector_Reserve(to_vec, from->codes);
            u16 *f = from->data;
            u32 *t = to->data;
            Mary_UTF_16_t utf_16; utf_16.char_32 = 1;
            while (utf_16.char_32 != 0) {
                MARY_UTF_Decode_16(utf_16, f);
                f += utf_16.units;
                (*t++ = utf_16.char_32);
            }
            to->units = t - (u32 *)to->data;
        } else if (from_utf == 32 && to_utf == 8) {
            Vector_Reserve(to_vec, from->codes * 4);
            u32 *f = from->data;
            u8 *t = to->data;
            Mary_Char_32_t char_32 = 1;
            Mary_UTF_8_t utf_8;
            while (char_32 != 0) {
                char_32 = *f++;
                MARY_UTF_Encode_8(
                    utf_8, char_32,
                    (*t++ = utf_8.a),
                    (*t++ = utf_8.a, *t++ = utf_8.b),
                    (*t++ = utf_8.a, *t++ = utf_8.b, *t++ = utf_8.c),
                    (*t++ = utf_8.a, *t++ = utf_8.b, *t++ = utf_8.c, *t++ = utf_8.d)
                );
            }
            to->units = t - (u8 *)to->data;
        } else if (from_utf == 32 && to_utf == 16) {
            Vector_Reserve(to_vec, from->codes * 2);
            u32 *f = from->data;
            u16 *t = to->data;
            Mary_Char_32_t char_32 = 1;
            Mary_UTF_16_t utf_16;
            while (char_32 != 0) {
                char_32 = *f++;
                MARY_UTF_Encode_16(
                    utf_16, char_32,
                    (*t++ = utf_16.a),
                    (*t++ = utf_16.a, *t++ = utf_16.b)
                );
            }
            to->units = t - (u16 *)to->data;
        }
    }
    Vector_Fit(to_vec);
    to->codes = from->codes;
}

void Mary_String_Recode(Mary_String_t *this, Mary_UTF_t to_utf)
{
    MARY_Assert(to_utf == 8 || to_utf == 16 || to_utf == 32,
                "'to_utf' must be 8, 16, or 32.");

    Mary_UTF_t from_utf = MARY_Unit_To_UTF(this->unit);

    if (MARY_Unit_To_UTF(this->unit) != to_utf) {
        Mary_String_t to_str;
        Mary_String_Create_From(&to_str, MARY_Allocator_Heap, to_utf, this->data, from_utf);
        Vector_Repurpose_With(VECTOR_Cast(this), MARY_Pointer_Cast(&to_str), to_str.unit, to_str.units);
        Mary_String_Destroy(&to_str);
    }
}

void Mary_String_Assign(Mary_String_t *this, Mary_String_t *str)
{
    this->unit = str->unit;
    Vector_Copy(VECTOR_Cast(str), VECTOR_Cast(this));
    //Vector_Fit(VECTOR_Cast(this)); // we should let user do this when they want.
    this->codes = str->codes;
}

void Mary_String_Append_Front(Mary_String_t *this, Mary_String_t *front)
{
    Mary_UTF_t this_utf = MARY_Unit_To_UTF(this->unit);
    Mary_UTF_t front_utf = MARY_Unit_To_UTF(front->unit);
    Mary_Vector_t *this_vec = VECTOR_Cast(this);

    if (this_utf != front_utf) {
        Mary_String_t f_str;
        Mary_String_Create_From(&f_str, MARY_Allocator_Heap, this_utf, front->data, front_utf);

        Mary_Size_t f_str_units = f_str.units - 1;
        Vector_Reserve(this_vec, f_str_units + this_vec->units);
        Mary_Move(this->data, VECTOR_Point(this_vec, f_str_units), this->unit * this->units);
        Mary_Copy(f_str.data, this->data, f_str.unit * f_str_units);
        this->units += f_str_units;
        this->codes += f_str.codes - 1;

        Mary_String_Destroy(&f_str);
    } else {
        Mary_Size_t front_units = front->units - 1;
        Vector_Reserve(this_vec, front_units + this_vec->units);
        Mary_Move(this->data, VECTOR_Point(this_vec, front_units), this->unit * this->units);
        Mary_Copy(front->data, this->data, front->unit * front_units);
        this->units += front_units;
        this->codes += front->codes - 1;
    }
}

void Mary_String_Append_Back(Mary_String_t *this, Mary_String_t *back)
{
    Mary_UTF_t this_utf = MARY_Unit_To_UTF(this->unit);
    Mary_UTF_t back_utf = MARY_Unit_To_UTF(back->unit);
    Mary_Vector_t *this_vec = VECTOR_Cast(this);

    if (this_utf != back_utf) {
        Mary_String_t b_str;
        Mary_String_Create_From(&b_str, MARY_Allocator_Heap, this_utf, back->data, back_utf);

        Vector_Reserve(this_vec, this_vec->units + b_str.units - 1);
        Mary_Copy(b_str.data, VECTOR_Point_Back(this_vec), b_str.unit * b_str.units);
        this->units += b_str.units - 1;
        this->codes += b_str.codes - 1;

        Mary_String_Destroy(&b_str);
    } else {
        Vector_Reserve(this_vec, this_vec->units + back->units - 1);
        Mary_Copy(back->data, VECTOR_Point_Back(this_vec), back->unit * back->units);
        this->units += back->units - 1;
        this->codes += back->codes - 1;
    }
}

void Mary_String_Trim(Mary_String_t *this)
{
    if (this->units == 1) {
        return;
    }

    Mary_Size_t units_left = 0;
    Mary_Size_t units_right = 0;
    Mary_Size_t codes_left = 0;
    Mary_Size_t codes_right = 0;
    Mary_Bool_t is_space;
    void *left = 0;
    void *right = 0;

    MARY_String_Each(this) {
        MARY_Regex_Class_Has(MARY_Regex_Class_Space,
                             it.char_32, is_space);
        left = it.ptr;
        if (is_space) {
            units_left += it.units;
            ++codes_left;
        } else {
            break;
        }
    }

    MARY_String_Each_Reverse(this) {
        MARY_Regex_Class_Has(MARY_Regex_Class_Space,
                             it.char_32, is_space);
        if (is_space) {
            right = it.ptr;
            units_right += it.units;
            ++codes_right;
        } else if (it.char_32 != 0) {
            break;
        }
    }

    Mary_UTF_t utf = MARY_String_Get_UTF(this);

    if (utf == 8) {
        *(u8 *)right = '\0';
    } else if (utf == 16) {
        *(u16 *)right = '\0';
    } else {
        *(u32 *)right = '\0';
    }

    this->units -= units_left + units_right;
    this->codes -= codes_left + codes_right;

    if (units_left) {
        Mary_Move(left, this->data, this->units * this->unit);
    }
}

Mary_Index_t Mary_String_Seek_Unit(Mary_String_t *this, Mary_Index_t code_idx)
{
    MARY_Assert(code_idx <= this->codes,
                "Invalid code index.");

    if (MARY_String_Get_UTF(this) == 32) {
        return code_idx;
    }

    if (code_idx == 0) {
        return 0;
    } else if (code_idx == this->codes) {
        return this->units;
    } else if (code_idx == this->codes - 1) {
        return this->units - 1;
    }

    if (code_idx < this->codes / 2 + 1) {
        MARY_String_Each(this) {
            if (it.code_idx == code_idx) {
                return it.unit_idx;
            }
        }
    } else {
        MARY_String_Each_Reverse(this) {
            if (it.code_idx == code_idx) {
                return it.unit_idx;
            }
        }
    }

    return ~0;
}

////////////////////////////////////////////////

void Mary_String_8_Match(Mary_String_t *mary_string, Mary_String_t *regex)
{
    // take the matches from an Regex_Execute and narrow down by flags. Maybe we should do the flags in here, not the compiler.
}

void Mary_String_8_Replace(Mary_String_t *mary_string, u8 *regex, u8 *replacement)
{

}
