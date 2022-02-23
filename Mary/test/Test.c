#include <stdio.h>
#include <stdlib.h>
#include <Mary/Mary.h>
#include <Mary/Test/Test.h>

MARY_Primitives;

////// Pool_t //////

void Test_Mary_Pool_Reallocate()
{
    // this could be a lot more thorough!
    TEST_START(Test_Mary_Pool_Reallocate);

    U bytes = 1024;
    Mary_Pool_t *pool = Mary_Pool_Create(bytes);
    void *p1, *p2, *p3; U free;

    free = bytes;
    p1 = Mary_Pool_Alloc(pool, 8); free -= 8;
    p2 = Mary_Pool_Realloc(pool, p1, 8);
    MARY_Assert(free = pool->free && p1 == p2, 0);
    Mary_Pool_Empty(pool);

    free = bytes;
    p1 = Mary_Pool_Alloc(pool, 8); free -= 8;
    p2 = Mary_Pool_Realloc(pool, p1, 9); free -= 8;
    MARY_Assert(free = pool->free && p1 == p2, 0);
    Mary_Pool_Empty(pool);

    free = bytes;
    p1 = Mary_Pool_Alloc(pool, 8); free -= 8;
    p2 = Mary_Pool_Realloc(pool, p1, 16); free -= 8;
    MARY_Assert(free = pool->free && p1 == p2, 0);
    Mary_Pool_Empty(pool);

    free = bytes;
    p1 = Mary_Pool_Alloc(pool, 8); free -= 8;
    p2 = Mary_Pool_Realloc(pool, p1, 7);
    MARY_Assert(free = pool->free && p1 == p2, 0);
    Mary_Pool_Empty(pool);

    free = bytes;
    p1 = Mary_Pool_Alloc(pool, 8); free -= 8;
    p2 = Mary_Pool_Realloc(pool, p1, 1);
    MARY_Assert(free = pool->free && p1 == p2, 0);
    Mary_Pool_Empty(pool);

    free = bytes;
    p1 = Mary_Pool_Alloc(pool, 8); free -= 8;
    p2 = Mary_Pool_Alloc(pool, 8); free -= 8;
    p3 = Mary_Pool_Realloc(pool, p1, 8);
    MARY_Assert(free = pool->free && p1 == p3 && p1 != p2, 0);
    Mary_Pool_Empty(pool);

    free = bytes;
    p1 = Mary_Pool_Alloc(pool, 8); free -= 8;
    p2 = Mary_Pool_Alloc(pool, 8); free -= 8;
    p3 = Mary_Pool_Realloc(pool, p1, 12); free -= 8;
    MARY_Assert(free = pool->free && p1 != p3 && p3 != p2, 0);
    Mary_Pool_Empty(pool);

    TEST_STOP;
}

////// String_t //////

void Test_MARY_String()
{
    Mary_String_t empt = { 0, 0, 0, 0, 0, 0 };
    MARY_String_Stack(test, 8, "test");
    printf("bytes: %llu, unit: %llu, units: %llu, codes: %llu, %s\n",
           test.bytes, test.unit, test.units, test.codes, (char *)test.data);
}

void Test_String_Create()
{
    TEST_START(Test_String_Create);

    Mary_String_t str;
    Mary_String_Create(&str, MARY_Allocator_Heap, 8, 8);
    MARY_Assert(str.data != 0, 0);
    MARY_Assert(str.bytes == 8, 0);
    MARY_Assert(str.unit == 1, 0);
    MARY_Assert(str.units == 1, 0);
    MARY_Assert(str.codes == 1, 0);
    Mary_String_Destroy(&str);
    Mary_String_Create(&str, MARY_Allocator_Heap, 16, 4);
    MARY_Assert(str.data != 0, 0);
    MARY_Assert(str.bytes == 8, 0);
    MARY_Assert(str.unit == 2, 0);
    MARY_Assert(str.units == 1, 0);
    MARY_Assert(str.codes == 1, 0);
    Mary_String_Destroy(&str);
    Mary_String_Create(&str, MARY_Allocator_Heap, 32, 2);
    MARY_Assert(str.data != 0, 0);
    MARY_Assert(str.bytes == 8, 0);
    MARY_Assert(str.unit == 4, 0);
    MARY_Assert(str.units == 1, 0);
    MARY_Assert(str.codes == 1, 0);
    Mary_String_Destroy(&str);

    TEST_STOP;
}

void Test_String_Recode_Speed()
{
    u8 *c_str = MARY_C_String(8, "aϑ☱𗷱");
    Mary_String_t str0; Mary_String_Create_From(&str0, MARY_Allocator_Heap, 8, c_str, 8);
    Mary_String_t str1; Mary_String_Create_From(&str1, MARY_Allocator_Heap, 8, c_str, 8);
    for (u64 i = 0; i < 0x3000; ++i)
    {
        Mary_String_Append_Back(&str0, &str1);
    }
    for (u64 i = 0; i < 16; ++i)
    {
        MARY_Benchmark(Mary_String_Recode(&str0, 32), 1);
        Mary_String_Recode(&str0, 8);
    }
}

void Test_String_Copy_To()
{
    /*u8 array[11];
    Mary_C_String_t c_str = MARY_C_String(32, "aϑ☱𗷱");
    Mary_String_t str; Mary_String_Create_From(&str, HEAP, 32, c_str, 32);
    Mary_p array_p = { array, sizeof(array) };
    Mary_String_Copy_To(&str, &array_p, MARY_UTF_8);
    for (I i = 0; ; ++i)
    {
    printf("%u ", array[i]);
    if (array[i] == 0) break;
    }*/
}

static void String_Macro_Decode(u16 *from, u8 *to)
{
    u16 *f = from; Mary_UTF_16_t utf_16; utf_16.char_32 = 1; Mary_UTF_8_t utf_8;
    for (u8 *t = to; utf_16.char_32 != 0;)
    {
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
}

static void String_Statement_Decode(u16 *from, u8 *to)
{
    u16 *f = from; u8 *t = to; u32 code = 1;

    while (code != 0)
    {
        if (*f < 0xD800 || *f > 0xDFFF)
        {
            code = *f++;
        } else
        {
            code = (*f - 0xD800 << 10) + (*(f + 1) - 0xDC00) + 0x10000, f += 2;
        }

        if (code < 0x000080)
        {
            *t++ = code;
        } else if (code < 0x000800)
        {
            *t++ = 0xC0 | code >> 6;
            *t++ = 0x80 | code & 0x3F;
        } else if (code < 0x010000)
        {
            *t++ = 0xE0 | code >> 12;
            *t++ = 0x80 | code >> 6 & 0x3F;
            *t++ = 0x80 | code & 0x3F;
        } else if (code < 0x110000)
        {
            *t++ = 0xF0 | code >> 18;
            *t++ = 0x80 | code >> 12 & 0x3F;
            *t++ = 0x80 | code >> 6 & 0x3F;
            *t++ = 0x80 | code & 0x3F;
        }
    }
}

static void String_Expression_Decode(u16 *from, u8 *to)
{
    u16 *f = from; u8 *t = to; u32 code = 1;

    while (code != 0)
    {
        *f < 0xD800 || *f > 0xDFFF ?
            (code = *f++) :
            (code = *f++ - 0xD800 << 10, code = *f++ - 0xDC00 + 0x10000);

        code < 0x000080 ?
            (*t++ = code) :
            code < 0x000800 ?
            (*t++ = 0xC0 | code >> 6,
             *t++ = 0x80 | code & 0x3F) :
            code < 0x010000 ?
            (*t++ = 0xE0 | code >> 12,
             *t++ = 0x80 | code >> 6 & 0x3F,
             *t++ = 0x80 | code & 0x3F) :
            code < 0x110000 ?
            (*t++ = 0xF0 | code >> 18,
             *t++ = 0x80 | code >> 12 & 0x3F,
             *t++ = 0x80 | code >> 6 & 0x3F,
             *t++ = 0x80 | code & 0x3F) : 0;
    }
}

void Test_String_Decode_Speed()
{
    #define C_STRING_REPEATS 0x100000
    #define TRIALS 24

    Mary_C_String_16_t c_str_16 = MARY_C_String(16, "aϑ☱𗷱");
    Mary_Size_t from_bytes = sizeof(u16) * 5 * C_STRING_REPEATS + sizeof(u16) * 1; // 1 for null.
    Mary_Size_t to_bytes = sizeof(u8) * 10 * C_STRING_REPEATS + sizeof(u8) * 1;
    u16 *from = malloc(from_bytes), *from_end = MARY_Index_Byte(from, from_bytes);
    u8 *to = malloc(to_bytes), *to_end = MARY_Index_Byte(from, to_bytes);

    for (u16 *f = from; f < from_end - 1; f += 5)
    {
        Mary_Copy(c_str_16, f, sizeof(u16) * 5);
    }
    *(from_end - 1) = 0;

    MARY_Benchmark(String_Macro_Decode(from, to), TRIALS);
    MARY_Benchmark(String_Statement_Decode(from, to), TRIALS);
    MARY_Benchmark(String_Expression_Decode(from, to), TRIALS);

    free(from), free(to);

    #undef C_STRING_REPEATS
    #undef TRIALS
}

////// ideas //////

typedef struct { u64 a, b; }                  Big_Return_t;
typedef struct { u8 a, b, c, d, e, f, g, h; } Middle_Return_t;
typedef uint64_t                              Small_Return_t;
static Big_Return_t    Big_Return() { return (Big_Return_t) { 10, 12 }; }
static Middle_Return_t Middle_Return() { return (Middle_Return_t) { 10, 12, 3, 34, 1, 5, 6, 1 }; }
static Small_Return_t  Small_Return() { return MARY_Random(64); }

void Test_Big_Return()
{
    u64 temp = 0; Big_Return_t a; Small_Return_t c; Middle_Return_t b;
    MARY_Benchmark(for (u64 i = 0; i < 0x100000; ++i) (a = Big_Return(), temp += ((u64 *)&a)[MARY_Random(1)]), 12);
    MARY_Benchmark(for (u64 i = 0; i < 0x100000; ++i) (b = Middle_Return(), temp += ((u8 *)&b)[MARY_Random(7)]), 12);
    //MARY_Benchmark(for (u64 i = 0; i < 0x10000; ++i) (c = Small_Return(), temp += ((u8 *)&c)[MARY_Random(7)]), 12);
    MARY_Benchmark(for (u64 i = 0; i < 0x100000; ++i) (c = Small_Return(), temp += c), 12);
    printf("%llu\n", temp);
}

void Test_Bit_Flags()
{
    TEST_START(Test_Bit_Flags);

    for (U i = 0, val = 1, flags = val; i < 64; ++i, val *= 2, flags = val)
    {
        MARY_Bit_Unset(flags, i); MARY_Assert(flags == 0, 0);
        MARY_Assert(MARY_Bit_Is_Set(flags, i) == MARY_FALSE, 0);
        MARY_Bit_Set(flags, i); MARY_Assert(flags == val, 0);
        MARY_Assert(MARY_Bit_Is_Set(flags, i) == MARY_TRUE, 0);
        //Mary_Print_Bits(&flags, sizeof(flags), 1);
    }

    for (U i = 0, val = 0xAAAAAAAAAAAAAAAA, flags = val; i < 64; ++i)
    {
        if (!MARY_Bit_Is_Set(flags, i))
        {
            MARY_Bit_Set(flags, i); MARY_Assert(flags != val, 0);
            MARY_Assert(MARY_Bit_Is_Set(flags, i) == MARY_TRUE, 0);
            //Mary_Print_Bits(&flags, sizeof(flags), 1);
            MARY_Bit_Unset(flags, i); MARY_Assert(flags == val, 0);
            MARY_Assert(MARY_Bit_Is_Set(flags, i) == MARY_FALSE, 0);
        } else
        {
            MARY_Bit_Unset(flags, i); MARY_Assert(flags != val, 0);
            MARY_Assert(MARY_Bit_Is_Set(flags, i) == MARY_FALSE, 0);
            //Mary_Print_Bits(&flags, sizeof(flags), 1);
            MARY_Bit_Set(flags, i); MARY_Assert(flags == val, 0);
            MARY_Assert(MARY_Bit_Is_Set(flags, i) == MARY_TRUE, 0);
        }
    }

    U flags = 0xAAAAAAAAAAAAAAAA;
    for (U i = 0; i < 64; ++i)
    {
        if (MARY_Bit_Is_Set(flags, i))
        {
            MARY_Bit_Unset(flags, i);
            MARY_Assert(MARY_Bit_Is_Set(flags, i) == MARY_FALSE, 0);
        } else
        {
            MARY_Bit_Set(flags, i);
            MARY_Assert(MARY_Bit_Is_Set(flags, i) == MARY_TRUE, 0);
        }
        //Mary_Print_Bits(&flags, sizeof(flags), 1);
    }
    MARY_Assert(flags == 0x5555555555555555, 0);

    for (U i = 0; i < 64; ++i)
    {
        MARY_Bit_Toggle(flags, i);
    }
    MARY_Assert(flags == 0xAAAAAAAAAAAAAAAA, 0);

    MARY_Bit_Clear(flags);
    MARY_Assert(flags == 0, 0);

    TEST_STOP;
}
