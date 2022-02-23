#include <Mary/Utils.h>
#include <Mary/Allocator.h>
#include <Mary/Arena.h>
#include <Mary/String.h>
#include <Mary/Regex.h>
#include <Mary/Test/Test.h> // this should prob. be one level up.

MARY_Primitives;

static void Test_Regex_Create();
static void Test_Regex_Execute();

void Test_Regex()
{
    Mary_Regex_Start();

    MARY_Arena_In;

    //Test_Regex_Create();
    Test_Regex_Execute();

    MARY_Arena_Out;

    Mary_Regex_Stop();
}

static void Test_Regex_Create()
{
    TEST_START(Test_Regex_Create);

    MARY_Arena_In;

    Mary_Regex_t regex;
    MARY_String_Static(expression, 8, u8"abc");
    Mary_Regex_Create(&regex, MARY_Allocator_Heap, &expression, "");
    Mary_Regex_Destroy(&regex);

    MARY_Arena_Out;

    TEST_STOP;
}

static void Test_Regex_Execute()
{
    TEST_START(Test_Regex_Execute);

    MARY_Arena_In;

    MARY_String_Frame(exp, 8, u8"");
    Mary_Regex_t regex;
    MARY_Vector_Frame(matches, Mary_Regex_Match_t, 16);
    MARY_String_Frame(str, 8, u8"");

    #define EXECUTE_AND_PRINT                                   \
    MARY_M                                                      \
        Mary_Regex_Execute(&regex, &str, &matches);             \
        printf("\n\n");                                         \
        if (matches.units > 0) {                                \
            MARY_Vector_Each(&matches, Mary_Regex_Match_t) {    \
                printf("from: %llu, to_exclusive: %llu\n",      \
                       it.val.from, it.val.to_exclusive);       \
            }                                                   \
        } else {                                                \
            printf("no matches...\n");                          \
        }                                                       \
    MARY_W

    #define CLEAN_UP                    \
    MARY_M                              \
        Mary_Regex_Destroy(&regex);     \
        MARY_Vector_Empty(&matches);    \
    MARY_W

    // should ~b and ~B respect \n as a boundary? perhaps only in multiline mode?

    // make sure to check for loose regular parens by keeping a balance in parser

    // consider making ':' a special character that has to be escaped.

    // also, we need to implement "i" flag. we will need to have functions that
    // take a char and return its capital. Latin is easy I think, because we just
    // need to add how ever many unicodes away the captial set is from the lower set.

    {
        MARY_String_Assign(&exp, 8, u8"abc");
        //Mary_Regex_Create(&regex, MARY_Allocator_Heap, &exp, "g");
        MARY_String_Assign(&str, 8, u8"111abc111aabc");
        //EXECUTE_AND_PRINT;
        //CLEAN_UP;
    }

    {
        MARY_String_Assign(&exp, 8, u8"^a");
        //Mary_Regex_Create(&regex, MARY_Allocator_Heap, &exp, "gm");
        MARY_String_Assign(&str, 8, u8"abbbbbbb\nabbbbbb");
        //EXECUTE_AND_PRINT;
        //CLEAN_UP;
    }

    {
        MARY_String_Assign(&exp, 8, u8"(?<a)bc");
        //Mary_Regex_Create(&regex, MARY_Allocator_Heap, &exp, "g");
        MARY_String_Assign(&str, 8, u8"ababcab");
        //EXECUTE_AND_PRINT;
        //CLEAN_UP;
    }

    {
        MARY_String_Assign(&exp, 8, u8"(?<^d)(α|β|א)+(?>c$)");
        Mary_Regex_Create(&regex, MARY_Allocator_Heap, &exp, "gm");
        MARY_String_Assign(&str, 8, u8"dαβαβc\n"
                                    u8"dαβββββαααββαβαβאαβαβαβαβαβαβαβαβαβαβc\n"
                                    u8"dαβαβc");
        EXECUTE_AND_PRINT;
        CLEAN_UP;
    }

    printf("\n\n");

    #undef EXECUTE_AND_PRINT
    #undef CLEAN_UP

    MARY_Arena_Out;

    TEST_STOP;
}
