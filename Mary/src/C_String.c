#include <Mary/C_String.h>
#include <Mary/Vector.h>

MARY_Primitives;

Mary_Size_t Mary_C_String_Count_Bytes(Mary_C_String_t c_string, Mary_UTF_t utf, Mary_Bool_t with_null)
{
    MARY_Assert(utf == 8 || utf == 16 || utf == 32,
                "UTF must be 8, 16, or 32.");

    Mary_Size_t bytes = 0;

    if (utf == 8) {
        for (u8 *p = c_string; *p != 0; bytes += 1, ++p);
        if (MARY_Truthy(with_null)) {
            bytes += 1;
        }
    } else if (utf == 16) {
        for (u16 *p = c_string; *p != 0; bytes += 2, ++p);
        if (MARY_Truthy(with_null)) {
            bytes += 2;
        }
    } else {
        for (u32 *p = c_string; *p != 0; bytes += 4, ++p);
        if (MARY_Truthy(with_null)) {
            bytes += 4;
        }
    }

    return bytes;
}

Mary_Size_t Mary_C_String_Count_Units(Mary_C_String_t c_string, Mary_UTF_t utf, Mary_Bool_t with_null)
{
    MARY_Assert(utf == 8 || utf == 16 || utf == 32,
                "UTF must be 8, 16, or 32.");

    Mary_Size_t units = 0;

    if (utf == 8) {
        for (u8 *p = c_string; *p != 0; ++units, ++p);
    } else if (utf == 16) {
        for (u16 *p = c_string; *p != 0; ++units, ++p);
    } else {
        for (u32 *p = c_string; *p != 0; ++units, ++p);
    }

    if (MARY_Truthy(with_null)) {
        ++units;
    }

    return units;
}

Mary_Size_t Mary_C_String_Count_Codes(Mary_C_String_t c_string, Mary_UTF_t utf, Mary_Bool_t with_null)
{
    MARY_Assert(utf == 8 || utf == 16 || utf == 32,
                "UTF must be 8, 16, or 32.");

    Mary_Size_t codes = 0;

    if (utf == 8) {
        for (u8 *p = c_string; *p != 0; ++codes, p += MARY_UTF_Ptr_8_To_Units_8(p));
    } else if (utf == 16) {
        for (u16 *p = c_string; *p != 0; ++codes, p += MARY_UTF_Ptr_16_To_Units_16(p));
    } else {
        for (u32 *p = c_string; *p != 0; ++codes, ++p);
    }

    if (MARY_Truthy(with_null)) {
        ++codes;
    }

    return codes;
}

void Mary_C_String_Count(Mary_C_String_t c_string, Mary_UTF_t utf, Mary_Bool_t with_null, Mary_C_String_Size_t *out_size)
{
    MARY_Assert(utf == 8 || utf == 16 || utf == 32,
                "UTF must be 8, 16, or 32.");

    out_size->codes = 0;

    if (utf == 8) {
        u8 *p = c_string;
        for (; *p != 0; ++out_size->codes, p += MARY_UTF_Ptr_8_To_Units_8(p));
        out_size->units = p - (u8 *)c_string;
    } else if (utf == 16) {
        u16 *p = c_string;
        for (; *p != 0; ++out_size->codes, p += MARY_UTF_Ptr_16_To_Units_16(p));
        out_size->units = p - (u16 *)c_string;
    } else {
        for (u32 *p = c_string; *p != 0; ++out_size->codes, ++p);
        out_size->units = out_size->codes;
    }

    if (MARY_Truthy(with_null)) {
        ++out_size->units, ++out_size->codes;
    }

    out_size->bytes = out_size->units > 0 ?
        out_size->units * MARY_UTF_To_Unit(utf) : 0;
}

void Mary_C_String_Copy(Mary_C_String_p *from, Mary_UTF_t from_utf, Mary_C_String_p *to, Mary_UTF_t to_utf, Mary_Bool_t with_null)
{
    MARY_Assert(from_utf == 8 || from_utf == 16 || from_utf == 32,
                "UTF must be 8, 16, or 32.");
    MARY_Assert(to_utf == 8 || to_utf == 16 || to_utf == 32,
                "UTF must be 8, 16, or 32.");

    #define A(THIS_IS_TRUE)\
        MARY_Assert(THIS_IS_TRUE, "External array out of space.")

    if (from_utf != to_utf) {
        if (from_utf == 8 && to_utf == 16) {
            u8 *f = from->data;
            u16 *t = to->data, *end = MARY_Index_Byte(to->data, to->bytes);
            Mary_UTF_8_t utf_8;
            Mary_UTF_16_t utf_16;
            while (*f != 0) {
                MARY_UTF_Decode_8(utf_8, f);
                f += utf_8.units;
                MARY_UTF_Encode_16(
                    utf_16, utf_8.char_32,
                    (A(t + 1 <= end), *t++ = utf_16.a),
                    (A(t + 2 <= end), *t++ = utf_16.a, *t++ = utf_16.b)
                );
            }
            if (MARY_Truthy(with_null)) {
                A(t != end), *t = 0;
            }
        } else if (from_utf == 8 && to_utf == 32) {
            u8 *f = from->data;
            u32 *t = to->data, *end = MARY_Index_Byte(to->data, to->bytes);
            Mary_UTF_8_t utf_8;
            while (*f != 0) {
                MARY_UTF_Decode_8(utf_8, f);
                f += utf_8.units;
                (A(t + 1 <= end), *t++ = utf_8.char_32);
            }
            if (MARY_Truthy(with_null)) {
                A(t != end), *t = 0;
            }
        } else if (from_utf == 16 && to_utf == 8) {
            u16 *f = from->data;
            u8 *t = to->data, *end = MARY_Index_Byte(to->data, to->bytes);
            Mary_UTF_16_t utf_16;
            Mary_UTF_8_t utf_8;
            while (*f != 0) {
                MARY_UTF_Decode_16(utf_16, f);
                f += utf_16.units;
                MARY_UTF_Encode_8(
                    utf_8, utf_16.char_32,
                    (A(t + 1 <= end), *t++ = utf_8.a),
                    (A(t + 2 <= end), *t++ = utf_8.a, *t++ = utf_8.b),
                    (A(t + 3 <= end), *t++ = utf_8.a, *t++ = utf_8.b, *t++ = utf_8.c),
                    (A(t + 4 <= end), *t++ = utf_8.a, *t++ = utf_8.b, *t++ = utf_8.c, *t++ = utf_8.d)
                );
            }
            if (MARY_Truthy(with_null)) {
                A(t != end), *t = 0;
            }
        } else if (from_utf == 16 && to_utf == 32) {
            u16 *f = from->data;
            u32 *t = to->data, *end = MARY_Index_Byte(to->data, to->bytes);
            Mary_UTF_16_t utf_16;
            while (*f != 0) {
                MARY_UTF_Decode_16(utf_16, f);
                f += utf_16.units;
                (A(t + 1 <= end), *t++ = utf_16.char_32);
            }
            if (MARY_Truthy(with_null)) {
                A(t != end), *t = 0;
            }
        } else if (from_utf == 32 && to_utf == 8) {
            u32 *f = from->data;
            u8 *t = to->data, *end = MARY_Index_Byte(to->data, to->bytes);
            Mary_Char_32_t char_32;
            Mary_UTF_8_t utf_8;
            while (*f != 0) {
                char_32 = *f++;
                MARY_UTF_Encode_8(
                    utf_8, char_32,
                    (A(t + 1 <= end), *t++ = utf_8.a),
                    (A(t + 2 <= end), *t++ = utf_8.a, *t++ = utf_8.b),
                    (A(t + 3 <= end), *t++ = utf_8.a, *t++ = utf_8.b, *t++ = utf_8.c),
                    (A(t + 4 <= end), *t++ = utf_8.a, *t++ = utf_8.b, *t++ = utf_8.c, *t++ = utf_8.d)
                );
            }
            if (MARY_Truthy(with_null)) {
                A(t != end), *t = 0;
            }
        } else if (from_utf == 32 && to_utf == 16) {
            u32 *f = from->data;
            u16 *t = to->data, *end = MARY_Index_Byte(to->data, to->bytes);
            Mary_Char_32_t char_32;
            Mary_UTF_16_t utf_16;
            while (*f != 0) {
                char_32 = *f++;
                MARY_UTF_Encode_16(
                    utf_16, char_32,
                    (A(t + 1 <= end), *t++ = utf_16.a),
                    (A(t + 2 <= end), *t++ = utf_16.a, *t++ = utf_16.b)
                );
            }
            if (MARY_Truthy(with_null)) {
                A(t != end), *t = 0;
            }
        }
    } else {
        if (MARY_Truthy(with_null)) {
            A(from->bytes <= to->bytes);
            Mary_Copy(from->data, to->data, from->bytes);
        } else {
            A(from->bytes - MARY_UTF_To_Unit(from_utf) <= to->bytes);
            Mary_Copy(from->data, to->data, from->bytes - MARY_UTF_To_Unit(from_utf));
        }
    }

    #undef A
}

Mary_u64_t Mary_C_String_To_Hexadecimal(Mary_C_String_t c_str, Mary_UTF_t utf, Mary_C_String_t *opt_out_ptr)
{
    MARY_Vector_Stack(stack, u64, 16);
    Mary_Bool_t is_negative = MARY_FALSE;
    u64 number = 0;
    u64 power = 1;

    #define TO_HEXADECIMAL                                      \
    MARY_M                                                      \
        if (chr == '-') {                                       \
            is_negative = MARY_TRUE;                            \
            chr = (++str)[0];                                   \
        }                                                       \
                                                                \
        while (chr == '0' || chr == 'x' || chr == 'X') {        \
            chr = (++str)[0];                                   \
        }                                                       \
                                                                \
        while (MARY_TRUE) {                                     \
            if ('0' <= chr && chr <= '9') {                     \
                MARY_Vector_Push(&stack, u64, chr - '0');       \
            } else if ('A' <= chr && chr <= 'F') {              \
                MARY_Vector_Push(&stack, u64, chr - 'A' + 10);  \
            } else if ('a' <= chr && chr <= 'f') {              \
                MARY_Vector_Push(&stack, u64, chr - 'a' + 10);  \
            } else {                                            \
                break;                                          \
            }                                                   \
            chr = (++str)[0];                                   \
        }                                                       \
                                                                \
        while (stack.units > 0) {                               \
            number += MARY_Vector_Pop(&stack, u64) * power;     \
            power *= 16;                                        \
        }                                                       \
                                                                \
        if (is_negative) {                                      \
            number = (u64)-(s64)number;                         \
        }                                                       \
                                                                \
        if (opt_out_ptr != 0) {                                 \
            *opt_out_ptr = str;                                 \
        }                                                       \
    MARY_W

    if (utf == 8) {
        Mary_C_String_8_t str = c_str;
        Mary_Char_8_t chr = str[0];
        TO_HEXADECIMAL;
    } else if (utf == 16) {
        Mary_C_String_16_t str = c_str;
        Mary_Char_16_t chr = str[0];
        TO_HEXADECIMAL;
    } else if (utf == 32) {
        Mary_C_String_32_t str = c_str;
        Mary_Char_32_t chr = str[0];
        TO_HEXADECIMAL;
    }

    return number;
}
