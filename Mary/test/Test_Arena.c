#include <Mary/Mary.h>
#include <Mary/Test/Test_Arena.h>

MARY_Primitives;

static void *Level_1();
static void *Level_2();
static void *Level_3();

void Mary_Test_Arena()
{
    Mary_Arena_Start();

    MARY_Arena_In;

    u8 *alloc = Level_1();
    alloc[3] = '\0';
    printf("%s\n", alloc);

    getchar();
    MARY_Arena_Out;
    getchar();

    Mary_Arena_Stop();
}

static void *Level_1()
{
    MARY_Arena_In;

    u8 *temp1 = MARY_Arena_Alloc(MARY_ARENA_FRAME, 512);
    u8 *alloc = Level_2();
    u8 *temp2 = MARY_Arena_Alloc(MARY_ARENA_FRAME, 1024);
    alloc[2] = 'c';

    MARY_Arena_Chain_Return(alloc);
}

static void *Level_2()
{
    u8 *alloc = Level_3();
    alloc[1] = 'b';
    return alloc;
}

static void *Level_3()
{
    MARY_Arena_In;

    u8 *alloc = MARY_Arena_Alloc(MARY_ARENA_CHAIN, 0x300001);
    alloc[0] = 'a';

    MARY_Arena_Return(alloc);
}
