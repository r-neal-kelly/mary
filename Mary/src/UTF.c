#include <Mary/UTF.h>

MARY_Primitives;

void Mary_UTF_Encode_8(Mary_UTF_8_t *utf_8, Mary_Char_32_t char_32)
{
    utf_8->char_32 = char_32;

    if (char_32 < 0x000080) {
        utf_8->units = 1;
        utf_8->a = char_32;
    } else if (char_32 < 0x000800) {
        utf_8->units = 2;
        utf_8->a = 0xC0 | char_32 >> 0x06;
        utf_8->b = 0x80 | char_32 & 0x3F;
    } else if (char_32 < 0x010000) {
        utf_8->units = 3;
        utf_8->a = 0xE0 | char_32 >> 0x0C;
        utf_8->b = 0x80 | char_32 >> 0x06 & 0x3F;
        utf_8->c = 0x80 | char_32 & 0x3F;
    } else if (char_32 < 0x110000) {
        utf_8->units = 4;
        utf_8->a = 0xF0 | char_32 >> 0x12;
        utf_8->b = 0x80 | char_32 >> 0x0C & 0x3F;
        utf_8->c = 0x80 | char_32 >> 0x06 & 0x3F;
        utf_8->d = 0x80 | char_32 & 0x3F;
    } else {
        MARY_Assert(MARY_FALSE,
                    "Unsupported unicode range.");
    }
}

void Mary_UTF_Decode_8(Mary_UTF_8_t *utf_8, Mary_Char_8_t *ptr)
{
    Mary_Char_8_t utf_8_a = *ptr;

    if (utf_8_a >> 7 == 0x00) {
        utf_8->units = 1;
        utf_8->a = utf_8_a;
        utf_8->char_32 = utf_8_a;
    } else if (utf_8_a >> 5 == 0x06) {
        utf_8->units = 2;
        utf_8->a = utf_8_a;
        utf_8->b = *(ptr + 1);
        utf_8->char_32 =
            (utf_8->a ^ 0xC0) << 0x06 |
            (utf_8->b ^ 0x80);
    } else if (utf_8_a >> 4 == 0x0E) {
        utf_8->units = 3;
        utf_8->a = utf_8_a;
        utf_8->b = *(ptr + 1);
        utf_8->c = *(ptr + 2);
        utf_8->char_32 =
            (utf_8->a ^ 0xE0) << 0x0C |
            (utf_8->b ^ 0x80) << 0x06 |
            (utf_8->c ^ 0x80);
    } else if (utf_8_a >> 3 == 0x1E) {
        utf_8->units = 4;
        utf_8->a = utf_8_a;
        utf_8->b = *(ptr + 1);
        utf_8->c = *(ptr + 2);
        utf_8->d = *(ptr + 3);
        utf_8->char_32 =
            (utf_8->a ^ 0xF0) << 0x12 |
            (utf_8->b ^ 0x80) << 0x0C |
            (utf_8->c ^ 0x80) << 0x06 |
            (utf_8->d ^ 0x80);
    } else {
        MARY_Assert(MARY_FALSE,
                    "Unsupported unicode range.");
    }
}

void Mary_UTF_Decode_8_Reverse(Mary_UTF_8_t *utf_8, Mary_Char_8_t *ptr_end)
{
    if (*(ptr_end - 1) >> 6 != 0x2) {
        utf_8->units = 1;
        utf_8->a = *(ptr_end - 1);
        utf_8->char_32 = utf_8->a;
    } else if (*(ptr_end - 2) >> 6 != 0x2) {
        utf_8->units = 2;
        utf_8->a = *(ptr_end - 2);
        utf_8->b = *(ptr_end - 1);
        utf_8->char_32 =
            (utf_8->a ^ 0xC0) << 0x06 |
            (utf_8->b ^ 0x80);
    } else if (*(ptr_end - 3) >> 6 != 0x2) {
        utf_8->units = 3;
        utf_8->a = *(ptr_end - 3);
        utf_8->b = *(ptr_end - 2);
        utf_8->c = *(ptr_end - 1);
        utf_8->char_32 =
            (utf_8->a ^ 0xE0) << 0x0C |
            (utf_8->b ^ 0x80) << 0x06 |
            (utf_8->c ^ 0x80);
    } else if (*(ptr_end - 4) >> 6 != 0x2) {
        utf_8->units = 4;
        utf_8->a = *(ptr_end - 4);
        utf_8->b = *(ptr_end - 3);
        utf_8->c = *(ptr_end - 2);
        utf_8->d = *(ptr_end - 1);
        utf_8->char_32 =
            (utf_8->a ^ 0xF0) << 0x12 |
            (utf_8->b ^ 0x80) << 0x0C |
            (utf_8->c ^ 0x80) << 0x06 |
            (utf_8->d ^ 0x80);
    } else {
        MARY_Assert(MARY_FALSE,
                    "Unsupported unicode range.");
    }
}

void Mary_UTF_Encode_16(Mary_UTF_16_t *utf_16, Mary_Char_32_t char_32)
{
    utf_16->char_32 = char_32;

    if (char_32 < 0x10000) {
        utf_16->units = 1;
        utf_16->a = char_32;
    } else {
        utf_16->units = 2;
        utf_16->a = (char_32 - 0x10000 >> 10) + 0xD800;
        utf_16->b = (char_32 - 0x10000 & 0x3FF) + 0xDC00;
    }
}

void Mary_UTF_Decode_16(Mary_UTF_16_t *utf_16, Mary_Char_16_t *ptr)
{
    Mary_Char_16_t utf_16_a = *ptr;
    utf_16->a = utf_16_a;

    if (utf_16_a < 0xD800 || 0xDBFF < utf_16_a) {
        utf_16->units = 1;
        utf_16->char_32 = utf_16_a;
    } else {
        Mary_Char_16_t utf_16_b = *(ptr + 1);

        if (utf_16_b < 0xDC00 || 0xDFFF < utf_16_b) {
            utf_16->units = 1;
            utf_16->char_32 = utf_16_a;
        } else {
            utf_16->units = 2;
            utf_16->b = utf_16_b;
            utf_16->char_32 =
                (utf_16_a - 0xD800 << 10) +
                (utf_16_b - 0xDC00) +
                0x10000;
        }
    }
}

void Mary_UTF_Decode_16_Reverse(Mary_UTF_16_t *utf_16, Mary_Char_16_t *ptr_end)
{
    Mary_Char_16_t utf_16_a = *(ptr_end - 1);

    if (utf_16_a < 0xDC00 || 0xDFFF < utf_16_a) {
        utf_16->units = 1;
        utf_16->a = utf_16_a;
        utf_16->char_32 = utf_16_a;
    } else {
        Mary_Char_16_t utf_16_b = utf_16_a;
        utf_16_a = *(ptr_end - 2);

        if (utf_16_a < 0xD800 || 0xDBFF < utf_16_a) {
            utf_16->units = 1;
            utf_16->a = utf_16_b;
            utf_16->char_32 = utf_16_b;
        } else {
            utf_16->units = 2;
            utf_16->a = utf_16_a;
            utf_16->b = utf_16_b;
            utf_16->char_32 =
                (utf_16_a - 0xD800 << 10) +
                (utf_16_b - 0xDC00) +
                0x10000;
        }
    }
}
