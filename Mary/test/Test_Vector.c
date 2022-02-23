#include <Mary/Utils.h>
#include <Mary/Vector.h>
#include <Mary/Arena.h>
#include <Mary/Test/Test.h>
#include MARY_Vector_Namespace

MARY_Primitives;

static void Test_Vector_Create();
static void Test_Vector_Loops();
static void Test_Vector_Linear_Search();
static void Test_Vector_Binary_Search();

void Test_Vector()
{
    Mary_Arena_Start();

    MARY_Arena_In;

    Test_Vector_Loops();
    Test_Vector_Linear_Search();
    Test_Vector_Binary_Search();

    MARY_Arena_Out;

    Mary_Arena_Stop();
}

static void Test_Vector_Create()
{
    MARY_Arena_In;

    TEST_START(Test_Vector_Create);

    // to be done...

    TEST_STOP;

    MARY_Arena_Out;
}

static void Test_Vector_Loops()
{
    MARY_Arena_In;

    TEST_START(Test_Vector_Loops);

    VECTOR_Frame(vec, u64, 8);

    for (I i = 1; i <= 8; ++i) {
        VECTOR_Push(&vec, u64, i);
    }

    Mary_Size_t iterations;
    u64 s[8];

    ////// Each
    {
        iterations = 0;
        Mary_Zero(s, sizeof(s));
        VECTOR_Each(&vec, u64) {
            MARY_Assert(it.this == &vec, 0);
            MARY_Assert(sizeof(it.val) == sizeof(u64), 0);
            s[iterations] = it.val;
            if (it.idx == 0) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 0), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 0), 0);
                ++iterations;
            } else if (it.idx == 1) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 1), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 1), 0);
                ++iterations;
            } else if (it.idx == 2) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 2), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 2), 0);
                ++iterations;
            } else if (it.idx == 3) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 3), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 3), 0);
                ++iterations;
            } else if (it.idx == 4) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 4), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 4), 0);
                ++iterations;
            } else if (it.idx == 5) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 5), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 5), 0);
                ++iterations;
            } else if (it.idx == 6) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 6), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 6), 0);
                ++iterations;
            } else if (it.idx == 7) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 7), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 7), 0);
                ++iterations;
            } else {
                MARY_Assert(MARY_FALSE, 0);
            }
        }
        MARY_Assert(iterations == 8, 0);
        MARY_Assert(s[0] == 1 && s[1] == 2 && s[2] == 3 && s[3] == 4 &&
                    s[4] == 5 && s[5] == 6 && s[6] == 7 && s[7] == 8, 0);
    }

    ////// Each_Reverse
    {
        iterations = 0;
        Mary_Zero(s, sizeof(s));
        VECTOR_Each_Reverse(&vec, u64) {
            MARY_Assert(it.this == &vec, 0);
            MARY_Assert(sizeof(it.val) == sizeof(u64), 0);
            s[iterations] = it.val;
            if (it.idx == 0) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 0), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 0), 0);
                ++iterations;
            } else if (it.idx == 1) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 1), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 1), 0);
                ++iterations;
            } else if (it.idx == 2) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 2), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 2), 0);
                ++iterations;
            } else if (it.idx == 3) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 3), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 3), 0);
                ++iterations;
            } else if (it.idx == 4) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 4), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 4), 0);
                ++iterations;
            } else if (it.idx == 5) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 5), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 5), 0);
                ++iterations;
            } else if (it.idx == 6) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 6), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 6), 0);
                ++iterations;
            } else if (it.idx == 7) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 7), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 7), 0);
                ++iterations;
            } else {
                MARY_Assert(MARY_FALSE, 0);
            }
        }
        MARY_Assert(iterations == 8, 0);
        MARY_Assert(s[0] == 8 && s[1] == 7 && s[2] == 6 && s[3] == 5 &&
                    s[4] == 4 && s[5] == 3 && s[6] == 2 && s[7] == 1, 0);
    }

    ////// Range over all
    {
        iterations = 0;
        Mary_Zero(s, sizeof(s));
        VECTOR_Range(&vec, u64, 0, vec.units) {
            MARY_Assert(it.this == &vec, 0);
            MARY_Assert(it.from == 0, 0);
            MARY_Assert(it.to_exclusive == vec.units, 0);
            MARY_Assert(sizeof(it.val) == sizeof(u64), 0);
            s[iterations] = it.val;
            if (it.idx == 0) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 0), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 0), 0);
                ++iterations;
            } else if (it.idx == 1) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 1), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 1), 0);
                ++iterations;
            } else if (it.idx == 2) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 2), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 2), 0);
                ++iterations;
            } else if (it.idx == 3) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 3), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 3), 0);
                ++iterations;
            } else if (it.idx == 4) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 4), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 4), 0);
                ++iterations;
            } else if (it.idx == 5) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 5), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 5), 0);
                ++iterations;
            } else if (it.idx == 6) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 6), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 6), 0);
                ++iterations;
            } else if (it.idx == 7) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 7), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 7), 0);
                ++iterations;
            } else {
                MARY_Assert(MARY_FALSE, 0);
            }
        }
        MARY_Assert(iterations == 8, 0);
        MARY_Assert(s[0] == 1 && s[1] == 2 && s[2] == 3 && s[3] == 4 &&
                    s[4] == 5 && s[5] == 6 && s[6] == 7 && s[7] == 8, 0);
    }

    ////// Range_Reverse over all
    {
        iterations = 0;
        Mary_Zero(s, sizeof(s));
        VECTOR_Range_Reverse(&vec, u64, 0, vec.units) {
            MARY_Assert(it.this == &vec, 0);
            MARY_Assert(it.from == 0, 0);
            MARY_Assert(it.to_exclusive == vec.units, 0);
            MARY_Assert(sizeof(it.val) == sizeof(u64), 0);
            s[iterations] = it.val;
            if (it.idx == 0) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 0), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 0), 0);
                ++iterations;
            } else if (it.idx == 1) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 1), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 1), 0);
                ++iterations;
            } else if (it.idx == 2) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 2), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 2), 0);
                ++iterations;
            } else if (it.idx == 3) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 3), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 3), 0);
                ++iterations;
            } else if (it.idx == 4) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 4), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 4), 0);
                ++iterations;
            } else if (it.idx == 5) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 5), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 5), 0);
                ++iterations;
            } else if (it.idx == 6) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 6), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 6), 0);
                ++iterations;
            } else if (it.idx == 7) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 7), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 7), 0);
                ++iterations;
            } else {
                MARY_Assert(MARY_FALSE, 0);
            }
        }
        MARY_Assert(iterations == 8, 0);
        MARY_Assert(s[0] == 8 && s[1] == 7 && s[2] == 6 && s[3] == 5 &&
                    s[4] == 4 && s[5] == 3 && s[6] == 2 && s[7] == 1, 0);
    }

    ////// Range over some
    {
        iterations = 0;
        Mary_Zero(s, sizeof(s));
        VECTOR_Range(&vec, u64, 1, 7) {
            MARY_Assert(it.this == &vec, 0);
            MARY_Assert(it.from == 1, 0);
            MARY_Assert(it.to_exclusive == 7, 0);
            MARY_Assert(sizeof(it.val) == sizeof(u64), 0);
            s[iterations] = it.val;
            if (it.idx == 0) {
                MARY_Assert(MARY_FALSE, 0);
            } else if (it.idx == 1) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 1), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 1), 0);
                ++iterations;
            } else if (it.idx == 2) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 2), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 2), 0);
                ++iterations;
            } else if (it.idx == 3) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 3), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 3), 0);
                ++iterations;
            } else if (it.idx == 4) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 4), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 4), 0);
                ++iterations;
            } else if (it.idx == 5) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 5), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 5), 0);
                ++iterations;
            } else if (it.idx == 6) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 6), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 6), 0);
                ++iterations;
            } else if (it.idx == 7) {
                MARY_Assert(MARY_FALSE, 0);
            } else {
                MARY_Assert(MARY_FALSE, 0);
            }
        }
        MARY_Assert(iterations == 6, 0);
        MARY_Assert(s[0] == 2 && s[1] == 3 && s[2] == 4 && s[3] == 5 &&
                    s[4] == 6 && s[5] == 7 && s[6] == 0 && s[7] == 0, 0);
    }

    ////// Range_Reverse over some
    {
        iterations = 0;
        Mary_Zero(s, sizeof(s));
        VECTOR_Range_Reverse(&vec, u64, 1, 7) {
            MARY_Assert(it.this == &vec, 0);
            MARY_Assert(it.from == 1, 0);
            MARY_Assert(it.to_exclusive == 7, 0);
            MARY_Assert(sizeof(it.val) == sizeof(u64), 0);
            s[iterations] = it.val;
            if (it.idx == 0) {
                MARY_Assert(MARY_FALSE, 0);
            } else if (it.idx == 1) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 1), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 1), 0);
                ++iterations;
            } else if (it.idx == 2) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 2), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 2), 0);
                ++iterations;
            } else if (it.idx == 3) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 3), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 3), 0);
                ++iterations;
            } else if (it.idx == 4) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 4), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 4), 0);
                ++iterations;
            } else if (it.idx == 5) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 5), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 5), 0);
                ++iterations;
            } else if (it.idx == 6) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 6), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 6), 0);
                ++iterations;
            } else if (it.idx == 7) {
                MARY_Assert(MARY_FALSE, 0);
            } else {
                MARY_Assert(MARY_FALSE, 0);
            }
        }
        MARY_Assert(iterations == 6, 0);
        MARY_Assert(s[0] == 7 && s[1] == 6 && s[2] == 5 && s[3] == 4 &&
                    s[4] == 3 && s[5] == 2 && s[6] == 0 && s[7] == 0, 0);
    }

    ////// Out of range
    {
        Mary_Bool_t hit_idx_7;
        
        hit_idx_7 = MARY_FALSE;
        VECTOR_Range(&vec, u64, 7, 100) {
            MARY_Assert(it.from == 7, 0);
            MARY_Assert(it.to_exclusive == vec.units, 0);
            if (it.idx == 7) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 7), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 7), 0);
                hit_idx_7 = MARY_TRUE;
            } else {
                MARY_Assert(MARY_FALSE, 0);
            }
        }
        MARY_Assert(hit_idx_7 == MARY_TRUE, 0);

        hit_idx_7 = MARY_FALSE;
        VECTOR_Range_Reverse(&vec, u64, 7, 100) {
            MARY_Assert(it.from == 7, 0);
            MARY_Assert(it.to_exclusive == vec.units, 0);
            if (it.idx == 7) {
                MARY_Assert(it.ptr == VECTOR_Point(&vec, 7), 0);
                MARY_Assert(it.val == VECTOR_At(&vec, u64, 7), 0);
                hit_idx_7 = MARY_TRUE;
            } else {
                MARY_Assert(MARY_FALSE, 0);
            }
        }
        MARY_Assert(hit_idx_7 == MARY_TRUE, 0);

        VECTOR_Range(&vec, u64, 8, 100) {
            MARY_Assert(MARY_FALSE, 0);
        }

        VECTOR_Range_Reverse(&vec, u64, 1241431, ~0) {
            MARY_Assert(MARY_FALSE, 0);
        }
    }

    ////// With empty
    {
        VECTOR_Empty(&vec);

        VECTOR_Each(&vec, u64) {
            MARY_Assert(MARY_FALSE, 0);
        }

        VECTOR_Each_Reverse(&vec, u64) {
            MARY_Assert(MARY_FALSE, 0);
        }

        VECTOR_Range(&vec, u64, 0, 0) {
            MARY_Assert(MARY_FALSE, 0);
        }

        VECTOR_Range_Reverse(&vec, u64, 0, 0) {
            MARY_Assert(MARY_FALSE, 0);
        }

        VECTOR_Range(&vec, u64, 0, 100) {
            MARY_Assert(MARY_FALSE, 0);
        }

        VECTOR_Range_Reverse(&vec, u64, 0, 100) {
            MARY_Assert(MARY_FALSE, 0);
        }

        VECTOR_Range(&vec, u64, 21, ~0) {
            MARY_Assert(MARY_FALSE, 0);
        }

        VECTOR_Range_Reverse(&vec, u64, 21, ~0) {
            MARY_Assert(MARY_FALSE, 0);
        }
    }

    ////// With variable type
    {
        typedef struct { u32 known_type; U unknown_type; } Test_t;
        Test_t test;

        Vector_Repurpose(&vec, sizeof(test), 8);

        for (u32 i = 1; i <= 8; ++i) {
            test.known_type = i;
            VECTOR_Push(&vec, Test_t, test);
        }

        iterations = 0;
        Mary_Zero(s, sizeof(s));
        VECTOR_Each(&vec, u32) {
            s[iterations] = it.val;
            ++iterations;
        }
        MARY_Assert(s[0] == 1 && s[1] == 2 && s[2] == 3 && s[3] == 4 &&
                    s[4] == 5 && s[5] == 6 && s[6] == 7 && s[7] == 8, 0);

        iterations = 0;
        Mary_Zero(s, sizeof(s));
        VECTOR_Each_Reverse(&vec, u32) {
            s[iterations] = it.val;
            ++iterations;
        }
        MARY_Assert(s[0] == 8 && s[1] == 7 && s[2] == 6 && s[3] == 5 &&
                    s[4] == 4 && s[5] == 3 && s[6] == 2 && s[7] == 1, 0);

        iterations = 0;
        Mary_Zero(s, sizeof(s));
        VECTOR_Range(&vec, u32, 1, 7) {
            s[iterations] = it.val;
            ++iterations;
        }
        MARY_Assert(s[0] == 2 && s[1] == 3 && s[2] == 4 && s[3] == 5 &&
                    s[4] == 6 && s[5] == 7 && s[6] == 0 && s[7] == 0, 0);

        iterations = 0;
        Mary_Zero(s, sizeof(s));
        VECTOR_Range_Reverse(&vec, u32, 1, 7) {
            s[iterations] = it.val;
            ++iterations;
        }
        MARY_Assert(s[0] == 7 && s[1] == 6 && s[2] == 5 && s[3] == 4 &&
                    s[4] == 3 && s[5] == 2 && s[6] == 0 && s[7] == 0, 0);
    }

    TEST_STOP;

    MARY_Arena_Out;
}

static void Test_Vector_Linear_Search()
{
    MARY_Arena_In;

    TEST_START(Test_Vector_Linear_Search);

    MARY_Vector_Frame(vec, u64, 1024);

    for (u64 i = 0; i < 1024; ++i) {
        MARY_Vector_Push(&vec, u64, i);
    }

    Mary_Index_t idx;
    u64 *ptr;

    for (I i = 0; i < 1024; ++i) {
        MARY_Vector_Linear_Search(&vec, idx, ptr, i, *ptr,
                                  MARY_Assert(idx == i && *ptr == i, 0),
                                  MARY_Assert(0, 0));
    }

    MARY_Vector_Linear_Search(&vec, idx, ptr, 1024, *ptr,
                              MARY_Assert(0, 0),
                              MARY_Assert(1, 0));

    TEST_STOP;

    MARY_Arena_Out;
}

static void Test_Vector_Binary_Search()
{
    MARY_Arena_In;

    TEST_START(Test_Vector_Binary_Search);

    MARY_Vector_Frame(vec, u64, 1024);

    for (u64 i = 0; i < 1024; ++i) {
        MARY_Vector_Push(&vec, u64, i);
    }

    Mary_Index_t idx;
    u64 *ptr;

    for (I i = 0; i < 1024; ++i) {
        MARY_Vector_Binary_Search(&vec, idx, ptr, i, *ptr,
                                  MARY_Assert(idx == i && *ptr == i, 0),
                                  MARY_Assert(0, 0));
    }

    MARY_Vector_Binary_Search(&vec, idx, ptr, 1024, *ptr,
                              MARY_Assert(0, 0),
                              MARY_Assert(1, 0));

    TEST_STOP;

    MARY_Arena_Out;
}
