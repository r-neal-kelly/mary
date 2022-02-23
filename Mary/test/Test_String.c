#include <Mary/Utils.h>
#include <Mary/Arena.h>
#include <Mary/String.h>
#include <Mary/OS.h>
#include <Mary/Test/Test.h> // this should prob. be one level up.

MARY_Primitives;

static void Test_String_Create();
static void Test_String_Recode();
static void Test_String_Assign();
static void Test_String_Append();
static void Test_String_Trim();
static void Test_String_Decode();
static void Test_String_Seek_Unit();
static void Test_String_Each();
static void Test_String_Range();

void Test_String()
{
    Mary_Arena_Start();

    MARY_Arena_In;

    Test_String_Create();
    Test_String_Recode();
    Test_String_Assign();
    Test_String_Append();
    Test_String_Trim();
    Test_String_Decode();
    Test_String_Seek_Unit();
    Test_String_Each();
    Test_String_Range();

    MARY_Arena_Out;

    Mary_Arena_Stop();
}

static void Test_String_Create()
{
    MARY_Arena_In;

    TEST_START(Test_String_Create);

    Mary_Pool_t *pool = Mary_Pool_Create(256);

    MARY_String_Static(static_str, 8, u8"aϑ☱𗷱");
    MARY_Assert(static_str.data != 0, 0);
    MARY_Assert(static_str.bytes == 11, 0);
    MARY_Assert(static_str.allocator.type == MARY_ALLOCATOR_STATIC, 0);
    MARY_Assert(static_str.unit == sizeof(u8), 0);
    MARY_Assert(static_str.units == 11, 0);
    MARY_Assert(static_str.codes == 5, 0);

    MARY_String_Stack(stack_str, 16, u"aϑ☱𗷱");
    MARY_Assert(stack_str.data != 0, 0);
    MARY_Assert(stack_str.bytes == 12, 0);
    MARY_Assert(stack_str.allocator.type == MARY_ALLOCATOR_STACK, 0);
    MARY_Assert(stack_str.unit == sizeof(u16), 0);
    MARY_Assert(stack_str.units == 6, 0);
    MARY_Assert(stack_str.codes == 5, 0);

    MARY_String_Heap(heap_str, 32, U"aϑ☱𗷱");
    MARY_Assert(heap_str.data != 0, 0);
    MARY_Assert(heap_str.bytes == 20, 0);
    MARY_Assert(heap_str.allocator.type == MARY_ALLOCATOR_HEAP, 0);
    MARY_Assert(heap_str.unit == sizeof(u32), 0);
    MARY_Assert(heap_str.units == 5, 0);
    MARY_Assert(heap_str.codes == 5, 0);
    Mary_String_Destroy(&heap_str);

    MARY_String_Pool(pool_str, pool, 8, u8"aϑ☱𗷱");
    MARY_Assert(pool_str.data != 0, 0);
    MARY_Assert(pool_str.bytes == 11, 0);
    MARY_Assert(pool_str.allocator.type == MARY_ALLOCATOR_POOL, 0);
    MARY_Assert(pool_str.allocator.var == pool->id, 0);
    MARY_Assert(pool_str.unit == sizeof(u8), 0);
    MARY_Assert(pool_str.units == 11, 0);
    MARY_Assert(pool_str.codes == 5, 0);
    Mary_String_Destroy(&pool_str);

    MARY_String_Frame(frame_str, 16, u"aϑ☱𗷱");
    MARY_Assert(frame_str.data != 0, 0);
    MARY_Assert(frame_str.bytes == 12, 0);
    MARY_Assert(frame_str.allocator.type == MARY_ALLOCATOR_FRAME, 0);
    MARY_Assert(frame_str.allocator.var == MARY_Arena_Frame_ID_Name, 0);
    MARY_Assert(frame_str.unit == sizeof(u16), 0);
    MARY_Assert(frame_str.units == 6, 0);
    MARY_Assert(frame_str.codes == 5, 0);

    MARY_String_Chain(chain_str, 32, U"aϑ☱𗷱");
    MARY_Assert(chain_str.data != 0, 0);
    MARY_Assert(chain_str.bytes == 20, 0);
    MARY_Assert(chain_str.allocator.type == MARY_ALLOCATOR_CHAIN, 0);
    MARY_Assert(chain_str.allocator.var == MARY_Arena_Frame_ID_Name, 0);
    MARY_Assert(chain_str.unit == sizeof(u32), 0);
    MARY_Assert(chain_str.units == 5, 0);
    MARY_Assert(chain_str.codes == 5, 0);

    Mary_Pool_Destroy(pool);

    TEST_STOP;

    MARY_Arena_Out;
}

static void Test_String_Recode()
{
    MARY_Arena_In;

    TEST_START(Test_String_Recode);

    Mary_C_String_8_t c_str_8 = MARY_C_String(8, "aϑ☱𗷱");
    Mary_C_String_16_t c_str_16 = MARY_C_String(16, "aϑ☱𗷱");
    Mary_C_String_32_t c_str_32 = MARY_C_String(32, "aϑ☱𗷱");

    MARY_String_Frame(str, 32, c_str_32);

    Mary_String_Recode(&str, 32);
    MARY_Assert(Mary_Is_Same(str.data, c_str_32, str.unit * str.units), 0);

    Mary_String_Recode(&str, 8);
    MARY_Assert(Mary_Is_Same(str.data, c_str_8, str.unit * str.units), 0);

    Mary_String_Recode(&str, 32);
    MARY_Assert(Mary_Is_Same(str.data, c_str_32, str.unit * str.units), 0);

    Mary_String_Recode(&str, 16);
    MARY_Assert(Mary_Is_Same(str.data, c_str_16, str.unit * str.units), 0);

    Mary_String_Recode(&str, 8);
    MARY_Assert(Mary_Is_Same(str.data, c_str_8, str.unit * str.units), 0);

    Mary_String_Recode(&str, 16);
    MARY_Assert(Mary_Is_Same(str.data, c_str_16, str.unit * str.units), 0);

    Mary_String_Recode(&str, 32);
    MARY_Assert(Mary_Is_Same(str.data, c_str_32, str.unit * str.units), 0);

    TEST_STOP;

    MARY_Arena_Out;
}

static void Test_String_Assign()
{
    MARY_Arena_In;

    TEST_START(Test_String_Assign);

    MARY_String_Frame(str, 8, "");
    Mary_Bool_t is_same = 0;

    MARY_String_Assign(&str, 8, u8"aϑ☱𗷱");
    is_same = Mary_Is_Same(str.data, u8"aϑ☱𗷱", str.unit * str.units);
    MARY_Assert(is_same, 0);
    MARY_Assert(str.allocator.type == MARY_ALLOCATOR_FRAME, 0);
    MARY_Assert(str.unit == sizeof(u8), 0);
    MARY_Assert(str.units == 11, 0);
    MARY_Assert(str.codes == 5, 0);

    MARY_String_Assign(&str, 16, u"ϑ☱𗷱a");
    is_same = Mary_Is_Same(str.data, u"ϑ☱𗷱a", str.unit * str.units);
    MARY_Assert(is_same, 0);
    MARY_Assert(str.allocator.type == MARY_ALLOCATOR_FRAME, 0);
    MARY_Assert(str.unit == sizeof(u16), 0);
    MARY_Assert(str.units == 6, 0);
    MARY_Assert(str.codes == 5, 0);

    MARY_String_Assign(&str, 32, U"☱𗷱aϑ");
    is_same = Mary_Is_Same(str.data, U"☱𗷱aϑ", str.unit * str.units);
    MARY_Assert(is_same, 0);
    MARY_Assert(str.allocator.type == MARY_ALLOCATOR_FRAME, 0);
    MARY_Assert(str.unit == sizeof(u32), 0);
    MARY_Assert(str.units == 5, 0);
    MARY_Assert(str.codes == 5, 0);

    TEST_STOP;

    MARY_Arena_Out;
}

static void Test_String_Append()
{
    MARY_Arena_In;

    TEST_START(Test_String_Append);

    MARY_String_Frame(str, 8, "");
    Mary_Bool_t is_same = 0;

    MARY_String_Append_Back(&str, 8, u8"love");
    is_same = Mary_Is_Same(str.data, "love", str.unit * str.units);
    MARY_Assert(is_same, 0);

    MARY_String_Append_Back(&str, 8, u8" and hate");
    is_same = Mary_Is_Same(str.data, "love and hate", str.unit * str.units);
    MARY_Assert(is_same, 0);

    MARY_String_Append_Back(&str, 16, u" are both good");
    is_same = Mary_Is_Same(str.data, "love and hate are both good", str.unit * str.units);
    MARY_Assert(is_same, 0);

    MARY_String_Append_Back(&str, 32, U" when righteous");
    is_same = Mary_Is_Same(str.data, "love and hate are both good when righteous", str.unit * str.units);
    MARY_Assert(is_same, 0);

    MARY_String_Append_Front(&str, 32, U" ");
    is_same = Mary_Is_Same(str.data, " love and hate are both good when righteous", str.unit * str.units);
    MARY_Assert(is_same, 0);

    MARY_String_Append_Front(&str, 16, u"Listen!");
    is_same = Mary_Is_Same(str.data, "Listen! love and hate are both good when righteous", str.unit * str.units);
    MARY_Assert(is_same, 0);

    MARY_String_Append_Front(&str, 8, u8"Lo! ");
    is_same = Mary_Is_Same(str.data, "Lo! Listen! love and hate are both good when righteous", str.unit * str.units);
    MARY_Assert(is_same, 0);

    TEST_STOP;

    MARY_Arena_Out;
}

static void Test_String_Trim()
{
    MARY_Arena_In;

    TEST_START(Test_String_Trim);

    Mary_Bool_t is_same = 0;

    MARY_String_Frame(str8, 8, u8" \x00A0 aϑ☱𗷱 \x00A0 ");
    Mary_String_Trim(&str8);
    is_same = Mary_Is_Same(str8.data, u8"aϑ☱𗷱", str8.unit * str8.units);
    MARY_Assert(is_same, 0);
    MARY_Assert(str8.allocator.type == MARY_ALLOCATOR_FRAME, 0);
    MARY_Assert(str8.unit == sizeof(u8), 0);
    MARY_Assert(str8.units == 11, 0);
    MARY_Assert(str8.codes == 5, 0);

    MARY_String_Frame(str16, 16, u" \x00A0 aϑ☱𗷱 \x00A0 ");
    Mary_String_Trim(&str16);
    is_same = Mary_Is_Same(str16.data, u"aϑ☱𗷱", str16.unit * str16.units);
    MARY_Assert(is_same, 0);
    MARY_Assert(str16.allocator.type == MARY_ALLOCATOR_FRAME, 0);
    MARY_Assert(str16.unit == sizeof(u16), 0);
    MARY_Assert(str16.units == 6, 0);
    MARY_Assert(str16.codes == 5, 0);

    MARY_String_Frame(str32, 32, U" \x00A0 aϑ☱𗷱 \x00A0 ");
    Mary_String_Trim(&str32);
    is_same = Mary_Is_Same(str32.data, U"aϑ☱𗷱", str32.unit * str32.units);
    MARY_Assert(is_same, 0);
    MARY_Assert(str32.allocator.type == MARY_ALLOCATOR_FRAME, 0);
    MARY_Assert(str32.unit == sizeof(u32), 0);
    MARY_Assert(str32.units == 5, 0);
    MARY_Assert(str32.codes == 5, 0);

    TEST_STOP;

    MARY_Arena_Out;
}

static void Test_String_Decode()
{
    #define CODE_1 97
    #define CODE_2 977
    #define CODE_3 9777
    #define CODE_4 97777
    #define CODE_5 0

    MARY_Arena_In;

    TEST_START(Test_String_Decode);

    {
        MARY_String_Stack(str8, 8, u8"aϑ☱𗷱");
        u8 *p8_front = MARY_Vector_Point_Front(&str8);
        u8 *p8_end = MARY_Vector_Point_End(&str8);
        Mary_UTF_8_t utf_8;

        MARY_UTF_Decode_8(utf_8, p8_front);
        MARY_Assert(utf_8.char_32 == CODE_1, 0);
        p8_front += utf_8.units;
        MARY_UTF_Decode_8(utf_8, p8_front);
        MARY_Assert(utf_8.char_32 == CODE_2, 0);
        p8_front += utf_8.units;
        MARY_UTF_Decode_8(utf_8, p8_front);
        MARY_Assert(utf_8.char_32 == CODE_3, 0);
        p8_front += utf_8.units;
        MARY_UTF_Decode_8(utf_8, p8_front);
        MARY_Assert(utf_8.char_32 == CODE_4, 0);
        p8_front += utf_8.units;
        MARY_UTF_Decode_8(utf_8, p8_front);
        MARY_Assert(utf_8.char_32 == CODE_5, 0);

        MARY_UTF_Decode_8_Reverse(utf_8, p8_end);
        MARY_Assert(utf_8.char_32 == CODE_5, 0);
        p8_end -= utf_8.units;
        MARY_UTF_Decode_8_Reverse(utf_8, p8_end);
        MARY_Assert(utf_8.char_32 == CODE_4, 0);
        p8_end -= utf_8.units;
        MARY_UTF_Decode_8_Reverse(utf_8, p8_end);
        MARY_Assert(utf_8.char_32 == CODE_3, 0);
        p8_end -= utf_8.units;
        MARY_UTF_Decode_8_Reverse(utf_8, p8_end);
        MARY_Assert(utf_8.char_32 == CODE_2, 0);
        p8_end -= utf_8.units;
        MARY_UTF_Decode_8_Reverse(utf_8, p8_end);
        MARY_Assert(utf_8.char_32 == CODE_1, 0);
    }

    {
        MARY_String_Stack(str16, 16, u"aϑ☱𗷱");
        u16 *p16_front = MARY_Vector_Point_Front(&str16);
        u16 *p16_end = MARY_Vector_Point_End(&str16);
        Mary_UTF_16_t utf_16;

        MARY_UTF_Decode_16(utf_16, p16_front);
        MARY_Assert(utf_16.char_32 == CODE_1, 0);
        p16_front += utf_16.units;
        MARY_UTF_Decode_16(utf_16, p16_front);
        MARY_Assert(utf_16.char_32 == CODE_2, 0);
        p16_front += utf_16.units;
        MARY_UTF_Decode_16(utf_16, p16_front);
        MARY_Assert(utf_16.char_32 == CODE_3, 0);
        p16_front += utf_16.units;
        MARY_UTF_Decode_16(utf_16, p16_front);
        MARY_Assert(utf_16.char_32 == CODE_4, 0);
        p16_front += utf_16.units;
        MARY_UTF_Decode_16(utf_16, p16_front);
        MARY_Assert(utf_16.char_32 == CODE_5, 0);

        MARY_UTF_Decode_16_Reverse(utf_16, p16_end);
        MARY_Assert(utf_16.char_32 == CODE_5, 0);
        p16_end -= utf_16.units;
        MARY_UTF_Decode_16_Reverse(utf_16, p16_end);
        MARY_Assert(utf_16.char_32 == CODE_4, 0);
        p16_end -= utf_16.units;
        MARY_UTF_Decode_16_Reverse(utf_16, p16_end);
        MARY_Assert(utf_16.char_32 == CODE_3, 0);
        p16_end -= utf_16.units;
        MARY_UTF_Decode_16_Reverse(utf_16, p16_end);
        MARY_Assert(utf_16.char_32 == CODE_2, 0);
        p16_end -= utf_16.units;
        MARY_UTF_Decode_16_Reverse(utf_16, p16_end);
        MARY_Assert(utf_16.char_32 == CODE_1, 0);
    }
    
    {
        MARY_String_Stack(str32, 32, U"aϑ☱𗷱");
        u32 *p32_front = MARY_Vector_Point_Front(&str32);
        u32 *p32_end = MARY_Vector_Point_End(&str32);
        Mary_UTF_32_t utf_32;

        MARY_UTF_Decode_32(utf_32, p32_front);
        MARY_Assert(utf_32.char_32 == CODE_1, 0);
        p32_front += utf_32.units;
        MARY_UTF_Decode_32(utf_32, p32_front);
        MARY_Assert(utf_32.char_32 == CODE_2, 0);
        p32_front += utf_32.units;
        MARY_UTF_Decode_32(utf_32, p32_front);
        MARY_Assert(utf_32.char_32 == CODE_3, 0);
        p32_front += utf_32.units;
        MARY_UTF_Decode_32(utf_32, p32_front);
        MARY_Assert(utf_32.char_32 == CODE_4, 0);
        p32_front += utf_32.units;
        MARY_UTF_Decode_32(utf_32, p32_front);
        MARY_Assert(utf_32.char_32 == CODE_5, 0);

        MARY_UTF_Decode_32_Reverse(utf_32, p32_end);
        MARY_Assert(utf_32.char_32 == CODE_5, 0);
        p32_end -= utf_32.units;
        MARY_UTF_Decode_32_Reverse(utf_32, p32_end);
        MARY_Assert(utf_32.char_32 == CODE_4, 0);
        p32_end -= utf_32.units;
        MARY_UTF_Decode_32_Reverse(utf_32, p32_end);
        MARY_Assert(utf_32.char_32 == CODE_3, 0);
        p32_end -= utf_32.units;
        MARY_UTF_Decode_32_Reverse(utf_32, p32_end);
        MARY_Assert(utf_32.char_32 == CODE_2, 0);
        p32_end -= utf_32.units;
        MARY_UTF_Decode_32_Reverse(utf_32, p32_end);
        MARY_Assert(utf_32.char_32 == CODE_1, 0);
    }

    TEST_STOP;

    MARY_Arena_Out;

    #undef CODE_0
    #undef CODE_1
    #undef CODE_2
    #undef CODE_3
    #undef CODE_4
}

static void Test_String_Seek_Unit()
{
    MARY_Arena_In;

    TEST_START(Test_String_Seek_Unit);

    {
        MARY_String_Stack(str, 8, u8"aϑ☱𗷱");
        MARY_Assert(Mary_String_Seek_Unit(&str, 0) == 0, 0);
        MARY_Assert(Mary_String_Seek_Unit(&str, 1) == 1, 0);
        MARY_Assert(Mary_String_Seek_Unit(&str, 2) == 3, 0);
        MARY_Assert(Mary_String_Seek_Unit(&str, 3) == 6, 0);
        MARY_Assert(Mary_String_Seek_Unit(&str, 4) == 10, 0);
    }

    {
        MARY_String_Stack(str, 16, u"aϑ☱𗷱");
        MARY_Assert(Mary_String_Seek_Unit(&str, 0) == 0, 0);
        MARY_Assert(Mary_String_Seek_Unit(&str, 1) == 1, 0);
        MARY_Assert(Mary_String_Seek_Unit(&str, 2) == 2, 0);
        MARY_Assert(Mary_String_Seek_Unit(&str, 3) == 3, 0);
        MARY_Assert(Mary_String_Seek_Unit(&str, 4) == 5, 0);
    }

    {
        MARY_String_Stack(str, 32, U"aϑ☱𗷱");
        MARY_Assert(Mary_String_Seek_Unit(&str, 0) == 0, 0);
        MARY_Assert(Mary_String_Seek_Unit(&str, 1) == 1, 0);
        MARY_Assert(Mary_String_Seek_Unit(&str, 2) == 2, 0);
        MARY_Assert(Mary_String_Seek_Unit(&str, 3) == 3, 0);
        MARY_Assert(Mary_String_Seek_Unit(&str, 4) == 4, 0);
    }

    TEST_STOP;

    MARY_Arena_Out;
}

static void Test_String_Each()
{
    #define CODE_0 97
    #define CODE_1 977
    #define CODE_2 9777
    #define CODE_3 97777
    #define CODE_4 0

    MARY_Arena_In;

    TEST_START(Test_String_Each);

    MARY_String_Frame(str, 8, u8"aϑ☱𗷱");

    Mary_String_Recode(&str, 8);

    MARY_String_Each(&str) {
        MARY_Assert(it.this == (void *)&str, 0);
        MARY_Assert(it.this->codes == 5, 0);
        MARY_Assert(it.this->units == 11, 0);
        MARY_Assert(it.utf == 8, 0);
        if (it.code_idx == 0) {
            MARY_Assert(it.unit_idx == 0, 0);
            MARY_Assert(it.ptr == (u8 *)it.this->data + 0, 0);
            MARY_Assert(it.char_32 == CODE_0, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 1) {
            MARY_Assert(it.unit_idx == 1, 0);
            MARY_Assert(it.ptr == (u8 *)it.this->data + 1, 0);
            MARY_Assert(it.char_32 == CODE_1, 0);
            MARY_Assert(it.units == 2, 0);
        } else if (it.code_idx == 2) {
            MARY_Assert(it.unit_idx == 3, 0);
            MARY_Assert(it.ptr == (u8 *)it.this->data + 3, 0);
            MARY_Assert(it.char_32 == CODE_2, 0);
            MARY_Assert(it.units == 3, 0);
        } else if (it.code_idx == 3) {
            MARY_Assert(it.unit_idx == 6, 0);
            MARY_Assert(it.ptr == (u8 *)it.this->data + 6, 0);
            MARY_Assert(it.char_32 == CODE_3, 0);
            MARY_Assert(it.units == 4, 0);
        } else if (it.code_idx == 4) {
            MARY_Assert(it.unit_idx == 10, 0);
            MARY_Assert(it.ptr == (u8 *)it.this->data + 10, 0);
            MARY_Assert(it.char_32 == CODE_4, 0);
            MARY_Assert(it.units == 1, 0);
        } else {
            MARY_Assert(it.code_idx >= 5, 0);
        }
    }

    MARY_String_Each_Reverse(&str) {
        MARY_Assert(it.this == (void *)&str, 0);
        MARY_Assert(it.this->codes == 5, 0);
        MARY_Assert(it.this->units == 11, 0);
        MARY_Assert(it.utf == 8, 0);
        if (it.code_idx == 0) {
            MARY_Assert(it.unit_idx == 0, 0);
            MARY_Assert(it.ptr == (u8 *)it.this->data + 0, 0);
            MARY_Assert(it.char_32 == CODE_0, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 1) {
            MARY_Assert(it.unit_idx == 1, 0);
            MARY_Assert(it.ptr == (u8 *)it.this->data + 1, 0);
            MARY_Assert(it.char_32 == CODE_1, 0);
            MARY_Assert(it.units == 2, 0);
        } else if (it.code_idx == 2) {
            MARY_Assert(it.unit_idx == 3, 0);
            MARY_Assert(it.ptr == (u8 *)it.this->data + 3, 0);
            MARY_Assert(it.char_32 == CODE_2, 0);
            MARY_Assert(it.units == 3, 0);
        } else if (it.code_idx == 3) {
            MARY_Assert(it.unit_idx == 6, 0);
            MARY_Assert(it.ptr == (u8 *)it.this->data + 6, 0);
            MARY_Assert(it.char_32 == CODE_3, 0);
            MARY_Assert(it.units == 4, 0);
        } else if (it.code_idx == 4) {
            MARY_Assert(it.unit_idx == 10, 0);
            MARY_Assert(it.ptr == (u8 *)it.this->data + 10, 0);
            MARY_Assert(it.char_32 == CODE_4, 0);
            MARY_Assert(it.units == 1, 0);
        } else {
            MARY_Assert(it.code_idx >= 5, 0);
        }
    }

    Mary_String_Recode(&str, 16);

    MARY_String_Each(&str) {
        MARY_Assert(it.this == (void *)&str, 0);
        MARY_Assert(it.this->codes == 5, 0);
        MARY_Assert(it.this->units == 6, 0);
        MARY_Assert(it.utf == 16, 0);
        if (it.code_idx == 0) {
            MARY_Assert(it.unit_idx == 0, 0);
            MARY_Assert(it.ptr == (u16 *)it.this->data + 0, 0);
            MARY_Assert(it.char_32 == CODE_0, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 1) {
            MARY_Assert(it.unit_idx == 1, 0);
            MARY_Assert(it.ptr == (u16 *)it.this->data + 1, 0);
            MARY_Assert(it.char_32 == CODE_1, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 2) {
            MARY_Assert(it.unit_idx == 2, 0);
            MARY_Assert(it.ptr == (u16 *)it.this->data + 2, 0);
            MARY_Assert(it.char_32 == CODE_2, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 3) {
            MARY_Assert(it.unit_idx == 3, 0);
            MARY_Assert(it.ptr == (u16 *)it.this->data + 3, 0);
            MARY_Assert(it.char_32 == CODE_3, 0);
            MARY_Assert(it.units == 2, 0);
        } else if (it.code_idx == 4) {
            MARY_Assert(it.unit_idx == 5, 0);
            MARY_Assert(it.ptr == (u16 *)it.this->data + 5, 0);
            MARY_Assert(it.char_32 == CODE_4, 0);
            MARY_Assert(it.units == 1, 0);
        } else {
            MARY_Assert(it.code_idx >= 5, 0);
        }
    }

    MARY_String_Each_Reverse(&str) {
        MARY_Assert(it.this == (void *)&str, 0);
        MARY_Assert(it.this->codes == 5, 0);
        MARY_Assert(it.this->units == 6, 0);
        MARY_Assert(it.utf == 16, 0);
        if (it.code_idx == 0) {
            MARY_Assert(it.unit_idx == 0, 0);
            MARY_Assert(it.ptr == (u16 *)it.this->data + 0, 0);
            MARY_Assert(it.char_32 == CODE_0, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 1) {
            MARY_Assert(it.unit_idx == 1, 0);
            MARY_Assert(it.ptr == (u16 *)it.this->data + 1, 0);
            MARY_Assert(it.char_32 == CODE_1, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 2) {
            MARY_Assert(it.unit_idx == 2, 0);
            MARY_Assert(it.ptr == (u16 *)it.this->data + 2, 0);
            MARY_Assert(it.char_32 == CODE_2, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 3) {
            MARY_Assert(it.unit_idx == 3, 0);
            MARY_Assert(it.ptr == (u16 *)it.this->data + 3, 0);
            MARY_Assert(it.char_32 == CODE_3, 0);
            MARY_Assert(it.units == 2, 0);
        } else if (it.code_idx == 4) {
            MARY_Assert(it.unit_idx == 5, 0);
            MARY_Assert(it.ptr == (u16 *)it.this->data + 5, 0);
            MARY_Assert(it.char_32 == CODE_4, 0);
            MARY_Assert(it.units == 1, 0);
        } else {
            MARY_Assert(it.code_idx >= 5, 0);
        }
    }

    Mary_String_Recode(&str, 32);

    MARY_String_Each(&str) {
        MARY_Assert(it.this == (void *)&str, 0);
        MARY_Assert(it.this->codes == 5, 0);
        MARY_Assert(it.this->units == 5, 0);
        MARY_Assert(it.utf == 32, 0);
        if (it.code_idx == 0) {
            MARY_Assert(it.unit_idx == 0, 0);
            MARY_Assert(it.ptr == (u32 *)it.this->data + 0, 0);
            MARY_Assert(it.char_32 == CODE_0, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 1) {
            MARY_Assert(it.unit_idx == 1, 0);
            MARY_Assert(it.ptr == (u32 *)it.this->data + 1, 0);
            MARY_Assert(it.char_32 == CODE_1, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 2) {
            MARY_Assert(it.unit_idx == 2, 0);
            MARY_Assert(it.ptr == (u32 *)it.this->data + 2, 0);
            MARY_Assert(it.char_32 == CODE_2, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 3) {
            MARY_Assert(it.unit_idx == 3, 0);
            MARY_Assert(it.ptr == (u32 *)it.this->data + 3, 0);
            MARY_Assert(it.char_32 == CODE_3, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 4) {
            MARY_Assert(it.unit_idx == 4, 0);
            MARY_Assert(it.ptr == (u32 *)it.this->data + 4, 0);
            MARY_Assert(it.char_32 == CODE_4, 0);
            MARY_Assert(it.units == 1, 0);
        } else {
            MARY_Assert(it.code_idx >= 5, 0);
        }
    }

    MARY_String_Each_Reverse(&str) {
        MARY_Assert(it.this == (void *)&str, 0);
        MARY_Assert(it.this->codes == 5, 0);
        MARY_Assert(it.this->units == 5, 0);
        MARY_Assert(it.utf == 32, 0);
        if (it.code_idx == 0) {
            MARY_Assert(it.unit_idx == 0, 0);
            MARY_Assert(it.ptr == (u32 *)it.this->data + 0, 0);
            MARY_Assert(it.char_32 == CODE_0, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 1) {
            MARY_Assert(it.unit_idx == 1, 0);
            MARY_Assert(it.ptr == (u32 *)it.this->data + 1, 0);
            MARY_Assert(it.char_32 == CODE_1, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 2) {
            MARY_Assert(it.unit_idx == 2, 0);
            MARY_Assert(it.ptr == (u32 *)it.this->data + 2, 0);
            MARY_Assert(it.char_32 == CODE_2, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 3) {
            MARY_Assert(it.unit_idx == 3, 0);
            MARY_Assert(it.ptr == (u32 *)it.this->data + 3, 0);
            MARY_Assert(it.char_32 == CODE_3, 0);
            MARY_Assert(it.units == 1, 0);
        } else if (it.code_idx == 4) {
            MARY_Assert(it.unit_idx == 4, 0);
            MARY_Assert(it.ptr == (u32 *)it.this->data + 4, 0);
            MARY_Assert(it.char_32 == CODE_4, 0);
            MARY_Assert(it.units == 1, 0);
        } else {
            MARY_Assert(it.code_idx >= 5, 0);
        }
    }

    TEST_STOP;

    MARY_Arena_Out;

    #undef CODE_0
    #undef CODE_1
    #undef CODE_2
    #undef CODE_3
    #undef CODE_4
}

static void Test_String_Range()
{
    MARY_Arena_In;

    TEST_START(Test_String_Range);

    {
        MARY_String_Stack(str, 8, u8"aϑ☱𗷱");

        MARY_String_Range(&str, 0, 5) {
            MARY_Assert(it.this == (void *)&str, 0);
            MARY_Assert(it.this->codes == 5, 0);
            MARY_Assert(it.this->units == 11, 0);
            MARY_Assert(it.utf == 8, 0);
            if (it.code_idx == 0) {
                MARY_Assert(it.unit_idx == 0, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 0, 0);
                MARY_Assert(it.char_32 == 97, 0);
                MARY_Assert(it.units == 1, 0);
            } else if (it.code_idx == 1) {
                MARY_Assert(it.unit_idx == 1, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 1, 0);
                MARY_Assert(it.char_32 == 977, 0);
                MARY_Assert(it.units == 2, 0);
            } else if (it.code_idx == 2) {
                MARY_Assert(it.unit_idx == 3, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 3, 0);
                MARY_Assert(it.char_32 == 9777, 0);
                MARY_Assert(it.units == 3, 0);
            } else if (it.code_idx == 3) {
                MARY_Assert(it.unit_idx == 6, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 6, 0);
                MARY_Assert(it.char_32 == 97777, 0);
                MARY_Assert(it.units == 4, 0);
            } else if (it.code_idx == 4) {
                MARY_Assert(it.unit_idx == 10, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 10, 0);
                MARY_Assert(it.char_32 == 0, 0);
                MARY_Assert(it.units == 1, 0);
            } else {
                MARY_Assert(it.code_idx >= 5, 0);
            }
        }

        MARY_String_Range_Reverse(&str, 0, 5) {
            MARY_Assert(it.this == (void *)&str, 0);
            MARY_Assert(it.this->codes == 5, 0);
            MARY_Assert(it.this->units == 11, 0);
            MARY_Assert(it.utf == 8, 0);
            if (it.code_idx == 0) {
                MARY_Assert(it.unit_idx == 0, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 0, 0);
                MARY_Assert(it.char_32 == 97, 0);
                MARY_Assert(it.units == 1, 0);
            } else if (it.code_idx == 1) {
                MARY_Assert(it.unit_idx == 1, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 1, 0);
                MARY_Assert(it.char_32 == 977, 0);
                MARY_Assert(it.units == 2, 0);
            } else if (it.code_idx == 2) {
                MARY_Assert(it.unit_idx == 3, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 3, 0);
                MARY_Assert(it.char_32 == 9777, 0);
                MARY_Assert(it.units == 3, 0);
            } else if (it.code_idx == 3) {
                MARY_Assert(it.unit_idx == 6, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 6, 0);
                MARY_Assert(it.char_32 == 97777, 0);
                MARY_Assert(it.units == 4, 0);
            } else if (it.code_idx == 4) {
                MARY_Assert(it.unit_idx == 10, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 10, 0);
                MARY_Assert(it.char_32 == 0, 0);
                MARY_Assert(it.units == 1, 0);
            } else {
                MARY_Assert(it.code_idx >= 5, 0);
            }
        }

        MARY_String_Range(&str, 1, 4) {
            MARY_Assert(it.this == (void *)&str, 0);
            MARY_Assert(it.this->codes == 5, 0);
            MARY_Assert(it.this->units == 11, 0);
            MARY_Assert(it.utf == 8, 0);
            if (it.code_idx == 0) {
                MARY_Assert(0, 0);
            } else if (it.code_idx == 1) {
                MARY_Assert(it.unit_idx == 1, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 1, 0);
                MARY_Assert(it.char_32 == 977, 0);
                MARY_Assert(it.units == 2, 0);
            } else if (it.code_idx == 2) {
                MARY_Assert(it.unit_idx == 3, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 3, 0);
                MARY_Assert(it.char_32 == 9777, 0);
                MARY_Assert(it.units == 3, 0);
            } else if (it.code_idx == 3) {
                MARY_Assert(it.unit_idx == 6, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 6, 0);
                MARY_Assert(it.char_32 == 97777, 0);
                MARY_Assert(it.units == 4, 0);
            } else if (it.code_idx == 4) {
                MARY_Assert(0, 0);
            } else {
                MARY_Assert(it.code_idx >= 5, 0);
            }
        }

        MARY_String_Range_Reverse(&str, 1, 4) {
            MARY_Assert(it.this == (void *)&str, 0);
            MARY_Assert(it.this->codes == 5, 0);
            MARY_Assert(it.this->units == 11, 0);
            MARY_Assert(it.utf == 8, 0);
            if (it.code_idx == 0) {
                MARY_Assert(0, 0);
            } else if (it.code_idx == 1) {
                MARY_Assert(it.unit_idx == 1, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 1, 0);
                MARY_Assert(it.char_32 == 977, 0);
                MARY_Assert(it.units == 2, 0);
            } else if (it.code_idx == 2) {
                MARY_Assert(it.unit_idx == 3, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 3, 0);
                MARY_Assert(it.char_32 == 9777, 0);
                MARY_Assert(it.units == 3, 0);
            } else if (it.code_idx == 3) {
                MARY_Assert(it.unit_idx == 6, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 6, 0);
                MARY_Assert(it.char_32 == 97777, 0);
                MARY_Assert(it.units == 4, 0);
            } else if (it.code_idx == 4) {
                MARY_Assert(0, 0);
            } else {
                MARY_Assert(it.code_idx >= 5, 0);
            }
        }

        MARY_String_Range(&str, 2, 3) {
            MARY_Assert(it.this == (void *)&str, 0);
            MARY_Assert(it.this->codes == 5, 0);
            MARY_Assert(it.this->units == 11, 0);
            MARY_Assert(it.utf == 8, 0);
            if (it.code_idx == 0) {
                MARY_Assert(0, 0);
            } else if (it.code_idx == 1) {
                MARY_Assert(0, 0);
            } else if (it.code_idx == 2) {
                MARY_Assert(it.unit_idx == 3, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 3, 0);
                MARY_Assert(it.char_32 == 9777, 0);
                MARY_Assert(it.units == 3, 0);
            } else if (it.code_idx == 3) {
                MARY_Assert(0, 0);
            } else if (it.code_idx == 4) {
                MARY_Assert(0, 0);
            } else {
                MARY_Assert(it.code_idx >= 5, 0);
            }
        }

        MARY_String_Range_Reverse(&str, 2, 3) {
            MARY_Assert(it.this == (void *)&str, 0);
            MARY_Assert(it.this->codes == 5, 0);
            MARY_Assert(it.this->units == 11, 0);
            MARY_Assert(it.utf == 8, 0);
            if (it.code_idx == 0) {
                MARY_Assert(0, 0);
            } else if (it.code_idx == 1) {
                MARY_Assert(0, 0);
            } else if (it.code_idx == 2) {
                MARY_Assert(it.unit_idx == 3, 0);
                MARY_Assert(it.ptr == (u8 *)it.this->data + 3, 0);
                MARY_Assert(it.char_32 == 9777, 0);
                MARY_Assert(it.units == 3, 0);
            } else if (it.code_idx == 3) {
                MARY_Assert(0, 0);
            } else if (it.code_idx == 4) {
                MARY_Assert(0, 0);
            } else {
                MARY_Assert(it.code_idx >= 5, 0);
            }
        }

        MARY_String_Range(&str, 3, 2) {
            MARY_Assert(0, 0);
        }

        MARY_String_Range_Reverse(&str, 3, 2) {
            MARY_Assert(0, 0);
        }
    }
    
    TEST_STOP;

    MARY_Arena_Out;
}
