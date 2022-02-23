#include <Mary/Utils.h>
#include <Mary/Arena.h>
#include <Mary/String.h>
#include <Mary/Unicode.h>
#include <Mary/Test/Test.h> // this should prob. be one level up.

static void Test_Unicode_Case_Fold();

void Test_Unicode()
{
    Mary_Arena_Start();
    Mary_Unicode_Start();

    MARY_Arena_In;

    Test_Unicode_Case_Fold();

    MARY_Arena_Out;

    Mary_Unicode_Stop();
    Mary_Arena_Stop();
}

static void Test_Unicode_Case_Fold()
{
    MARY_Arena_In;

    TEST_START(Test_Unicode_Case_Fold);

    Mary_Char_32_t char_32 = U'\x32';
    Mary_Char_32_t folded = Mary_Unicode_Fold_Case_Simple(char_32);
    printf("\n\nbefore:%X, after:%X\n\n", char_32, folded);

    Mary_String_t folded_str;
    Mary_Unicode_Fold_Full(U'\x1F52', &folded_str);

    TEST_STOP;

    MARY_Arena_Out;
}
