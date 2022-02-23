#include <stdio.h>
#include <stdlib.h>
#include <Mary/Mary.h>
#include <Mary/Test/Test.h>
#include <Mary/Test/Test_Arena.h>
#include <Mary/App/GCC.h>
#include MARY_Vector_Namespace

MARY_Primitives; // will want to include this by default for utils namespace I think.

void mousewheel_pink(Mary_Event_t *event)
{
    printf("mousewheel on pink\n");
}

void mousewheel_blue(Mary_Event_t *event)
{
    printf("mousewheel on blue\n");
    Mary_Event_Stop_Propagation(event);
    //Mary_Event_Disable_Default(event);
}

void mousedown_orange(Mary_Event_t *event)
{
    printf("mousedown on orange.\n");
}

void mousedown_blue(Mary_Event_t *event)
{
    Mary_Event_Mousedown_t *e = MARY_Event_Mousedown(event);
    printf("mousedown on blue.\n");
}

void mouseclick_pink(Mary_Event_t *event)
{
    Mary_Event_Mouseclick_t *e = MARY_Event_Mouseclick(event);
    printf("mouseclick on pink.\n");
}

void mousein_purple(Mary_Event_t *event)
{
    printf("mousein on purple.\n");
}

void mouseout_green(Mary_Event_t *event)
{
    printf("mouseout on green.\n");
}

void keypress_pink(Mary_Event_Keypress_t *keypress)
{
    //MARY_Event_Declare_Keypress(event);
    printf("%c", (char)keypress->key);
}

typedef int64_t(*Mary_Heapsort_f)(void *one, void *two);

void Mary_Heapsort(void *data, size_t unit, size_t units, Mary_Heapsort_f callback)
{
    if (units < 2) return;

    #define TOP(I) ((I - 1) / 2)
    #define LEFT(I) (I * 2 + 1)
    #define RIGHT(I) (I * 2 + 2)

    // heapify
    u64 last_idx = units - 1;
    u64 branch_idx = TOP(last_idx);
    u64 top_idx, left_idx, right_idx;
    u8 *top_ptr, *left_ptr, *right_ptr, *temp_ptr = malloc(unit);

    while (branch_idx + 1 > 0)
    {
        top_idx = branch_idx;
        left_idx = LEFT(top_idx);
        right_idx = RIGHT(top_idx);
        top_ptr = MARY_Point_Unit(data, unit, top_idx);
        left_ptr = MARY_Point_Unit(data, unit, left_idx);
        right_ptr = MARY_Point_Unit(data, unit, right_idx);

        while ((left_idx <= last_idx && MARY_Truthy(callback(left_ptr, top_ptr))) ||
            (right_idx <= last_idx && MARY_Truthy(callback(right_ptr, top_ptr))))
        {
            if (right_idx <= last_idx)
            {
                if (MARY_Truthy(callback(left_ptr, right_ptr)))
                {
                    MARY_Swap_Unit(top_ptr, left_ptr, temp_ptr, unit);
                    top_idx = left_idx;
                } else
                {
                    MARY_Swap_Unit(top_ptr, right_ptr, temp_ptr, unit);
                    top_idx = right_idx;
                }
            } else
            {
                MARY_Swap_Unit(top_ptr, left_ptr, temp_ptr, unit);
                top_idx = left_idx;
            }
            left_idx = LEFT(top_idx);
            right_idx = RIGHT(top_idx);
            top_ptr = MARY_Point_Unit(data, unit, top_idx);
            left_ptr = MARY_Point_Unit(data, unit, left_idx);
            right_ptr = MARY_Point_Unit(data, unit, right_idx);
        }

        --branch_idx;
    }

    // sort
    u8 *root_ptr = MARY_Point_Unit(data, unit, 0);
    u8 *last_ptr = MARY_Point_Unit(data, unit, last_idx);
    while (last_idx > 0)
    {
        MARY_Swap_Unit(root_ptr, last_ptr, temp_ptr, unit);
        --last_idx, last_ptr -= unit;

        // need to sift down again.
        top_idx = 0;
        left_idx = LEFT(top_idx);
        right_idx = RIGHT(top_idx);
        top_ptr = MARY_Point_Unit(data, unit, top_idx);
        left_ptr = MARY_Point_Unit(data, unit, left_idx);
        right_ptr = MARY_Point_Unit(data, unit, right_idx);

        while ((left_idx <= last_idx && MARY_Truthy(callback(left_ptr, top_ptr))) ||
            (right_idx <= last_idx && MARY_Truthy(callback(right_ptr, top_ptr))))
        {
            if (right_idx <= last_idx)
            {
                if (MARY_Truthy(callback(left_ptr, right_ptr)))
                {
                    MARY_Swap_Unit(top_ptr, left_ptr, temp_ptr, unit);
                    top_idx = left_idx;
                } else
                {
                    MARY_Swap_Unit(top_ptr, right_ptr, temp_ptr, unit);
                    top_idx = right_idx;
                }
            } else
            {
                MARY_Swap_Unit(top_ptr, left_ptr, temp_ptr, unit);
                top_idx = left_idx;
            }
            left_idx = LEFT(top_idx);
            right_idx = RIGHT(top_idx);
            top_ptr = MARY_Point_Unit(data, unit, top_idx);
            left_ptr = MARY_Point_Unit(data, unit, left_idx);
            right_ptr = MARY_Point_Unit(data, unit, right_idx);
        }
    }

    free(temp_ptr);

    #undef TOP
    #undef LEFT
    #undef RIGHT
}

int64_t Heapsort_Sort(void *one, void *two)
{
    return *MARY_Cast(one, u64) > *MARY_Cast(two, u64);
}

typedef int64_t(*Mary_Quicksort_f)(void *low, void *high);

void Mary_Quicksort(void *data, size_t unit, size_t units, Mary_Quicksort_f callback)
{
    if (units < 2) return;

    MARY_Vector_Heap(stack, Mary_Slice_t, 64);
    Mary_Slice_t slice = { 0, units }; MARY_Vector_Push(&stack, Mary_Slice_t, slice);

    u64 from, to_exclusive, i, j;
    u8 *i_ptr, *j_ptr, *pivot_ptr, *swap_ptr = malloc(unit);

    while (stack.units > 0)
    {
        slice = MARY_Vector_Pop(&stack, Mary_Slice_t);
        from = slice.from, to_exclusive = slice.to_exclusive;
        i = slice.from, j = slice.to_exclusive - 1;

        if (i < j)
        {
            i_ptr = MARY_Point_Unit(data, unit, i);
            j_ptr = MARY_Point_Unit(data, unit, j);
            pivot_ptr = i_ptr;

            while (MARY_TRUE)
            {
                while (MARY_Truthy(callback(pivot_ptr, i_ptr))) // i < pivot
                {
                    ++i, i_ptr += unit;
                }

                while (j != 0 && MARY_Truthy(callback(j_ptr, pivot_ptr))) // j > pivot
                {
                    --j, j_ptr -= unit;
                }

                if (i < j)
                {
                    if (Mary_Is_Same(i_ptr, j_ptr, unit))
                    {
                        --j, j_ptr -= unit;
                    } else
                    {
                        MARY_Swap_Unit(i_ptr, j_ptr, swap_ptr, unit);
                    }
                } else
                {
                    slice.from = j + 1, slice.to_exclusive = to_exclusive;
                    MARY_Vector_Push(&stack, Mary_Slice_t, slice);
                    slice.from = from, slice.to_exclusive = j + 1;
                    MARY_Vector_Push(&stack, Mary_Slice_t, slice);
                    break;
                }
            }
        }
    }

    free(swap_ptr);
    Vector_Destroy(&stack);
}

u64 Mary_Quicksort_64_Partition(u64 *a, u64 from, u64 to_exclusive)
{
    u64 i = from - 1, j = to_exclusive, p = a[from], swap;
    while (MARY_TRUE)
    {
        do ++i; while (a[i] < p);
        do --j; while (a[j] > p);
        if (i < j)
        {
            swap = a[i], a[i] = a[j], a[j] = swap;
        } else
        {
            return j + 1;
        }
    }
}

void Mary_Quicksort_64(u64 *a, u64 from, u64 to_exclusive)
{
    if (to_exclusive - from < 2) return;
    u64 p = Mary_Quicksort_64_Partition(a, from, to_exclusive);
    Mary_Quicksort_64(a, from, p);
    Mary_Quicksort_64(a, p, to_exclusive);
}

void Mary_Quicksort_64_2(u64 *a, u64 units)
{
    if (units < 2) return;

    // we can use the stack's units as an indicator to break off to heapsort
    MARY_Vector_Stack(stack, Mary_Slice_t, 64);
    Mary_Slice_t slice = { 0, units }; MARY_Vector_Push(&stack, Mary_Slice_t, slice);

    u64 from, to_exclusive, p, i, j, swap;

    while (stack.units > 0)
    {
        slice = MARY_Vector_Pop(&stack, Mary_Slice_t);
        from = slice.from, to_exclusive = slice.to_exclusive;
        p = a[from], i = from - 1, j = slice.to_exclusive;

        while (MARY_TRUE)
        {
            do ++i; while (a[i] < p);
            do --j; while (a[j] > p);
            if (i < j)
            {
                swap = a[i], a[i] = a[j], a[j] = swap;
            } else
            {
                slice.from = from, slice.to_exclusive = j + 1;
                if (slice.from < slice.to_exclusive - 1)
                {
                    MARY_Vector_Push(&stack, Mary_Slice_t, slice);
                }
                slice.from = j + 1, slice.to_exclusive = to_exclusive;
                if (slice.from < slice.to_exclusive - 1)
                {
                    MARY_Vector_Push(&stack, Mary_Slice_t, slice);
                }
                break;
            }
        }
    }
}

// only optimization left is pivot choosing. might as well use the ninther thing.
// but we need to add genericism to this to make it really useful. the fewer function calls, the better
// the plan is to bail out if it's taking too long, or if stack fails, and go to heapsort, i.e. introsort
// also if we do a macro for the function body, we can't have other macros in it

// I think it would be fine to put the the entire func in a macro that can be placed in other functions
// and so there can be multiple version of it (one per thread for instance.) that way maybe we can put a macro
// in there that has the actual code to compare, and it can be changed on use case. maybe. haven't thought it through.
u64 Mary_Quicksort_64_3(u64 *a, u64 units)
{
    #define stack_depth 64
    #define pop_slice (slice = &stack[stack_idx], --stack_units, --stack_idx)
    #define push_slice (++stack_units, ++stack_idx, slice = &stack[stack_idx])
    #define push_left (push_slice, slice->from = from, slice->to_exclusive = k)
    #define push_right (push_slice, slice->from = k, slice->to_exclusive = to_exclusive)
    #define has_left (from < j)
    #define has_right (k < to_exclusive - 1)
    #define left_is_smaller (k - from < to_exclusive - k)
    #define should_bail (stack_units == stack_depth)

    if (units < 2) {
        return 0;
    }

    Mary_Slice_t stack[stack_depth], *slice = stack;
    u64 stack_units = 1, stack_idx = 0, from, to_exclusive, p, i, j, k, swap;
    slice->from = 0, slice->to_exclusive = units;

    while (stack_units > 0) {
        pop_slice; from = slice->from, to_exclusive = slice->to_exclusive;
        p = a[from], i = from - 1, j = to_exclusive;

        while (1)
        {
            do ++i; while (a[i] < p);
            do --j; while (a[j] > p);
            if (i < j) {
                swap = a[i], a[i] = a[j], a[j] = swap;
            } else {
                k = j + 1;
                if (has_left) {
                    if (should_bail) {
                        return 1;
                    } else if (has_right) {
                        if (left_is_smaller) {
                            push_right, push_left;
                        } else {
                            push_left, push_right;
                        }
                    } else {
                        push_left;
                    }
                } else if (has_right) {
                    if (should_bail) {
                        return 1;
                    } else if (has_left) {
                        if (left_is_smaller) {
                            push_right, push_left;
                        } else {
                            push_left, push_right;
                        }
                    } else {
                        push_right;
                    }
                }
                break;
            }
        }
    }

    return 0;

    #undef stack_depth
    #undef pop_slice
    #undef push_slice
    #undef push_left
    #undef push_right
    #undef has_left
    #undef has_right
    #undef left_is_smaller
    #undef should_bail
}

int64_t Quicksort_Sort(void *low, void *high)
{
    return *MARY_Cast(low, u64) > *MARY_Cast(high, u64);
}

int Qsort_Sort(const void *a_ptr, const void *b_ptr)
{
    return *MARY_Cast(a_ptr, u16) - *MARY_Cast(b_ptr, u16);
}

int main()
{
#if 1
    Test_Vector();
    Test_String();
    Test_Unicode();
    Test_Regex();
    Mary_Exit_Success();
#endif

#if 0
    // because Arena_Start depends on Pool_Start, I'm thinking about having global init
    // variables that each start function can check to ensure that things are done in the
    // right order, and not done multiple times.
    // we should even put asserts in functions that rely on the initialization, so that they'll
    // help us in debug, but go away in release.

    Mary_Arena_Start();

    MARY_Arena_In;

    VECTOR(vec1, FRAME, u8, 32); vec1.units = 32;
    VECTOR_Each(&vec1, u8) *it.ptr = MARY_Random(255);
    VECTOR_Each(&vec1, u8) printf("%4u", it.val); printf("\n");

    VECTOR_Stack(vec2, void *, 24);
    VECTOR_Push(&vec2, void *, (void *)0);
    VECTOR_Push(&vec2, void *, (void *)1);
    //VECTOR_At(&vec2, void *, 2) = (void *)5; // asserts
    VECTOR_At_Front(&vec2, void *) = (void *)0xFFFF;
    VECTOR_At_Back(&vec2, void *) = (void *)0x10000;
    VECTOR_Each(&vec2, void *) printf("%p ", it.val); printf("\n");
    void *test1, *test2;
    test1 = VECTOR_Pop(&vec2, void *), printf("%p\n", test1);
    test2 = VECTOR_Pop(&vec2, void *), printf("%p\n", test2);
    //VECTOR_Pop(&vec2, void *); // asserts

    VECTOR_Stack(vec3, Mary_Size_2D_t, 24);
    Mary_Size_2D_t *size1 = VECTOR_Point_Push(&vec3);
    size1->width = 100, size1->height = 200;
    VECTOR_Each(&vec3, Mary_Size_2D_t)
        printf("w:%llu, h:%llu\n", it.val.width, it.val.height);
    Mary_Size_2D_t *size2 = VECTOR_Point_Pop(&vec3);
    printf("w:%llu, h:%llu\n", size2->width, size2->height);

    MARY_Arena_Out;

    Mary_Arena_Stop();

    Mary_Exit_Success();
#endif

#if 0
    Mary_GCC_Create_Bat();
    Mary_Exit_Success();
#endif

#if 0
    #define size 8
    #define type u64
    #define test(EXPRESSION, TIMES)                                    \
  for (u64 i = 0; i < TIMES; ++i)                                  \
  {                                                                \
    MARY_Benchmark(EXPRESSION, 1);                                 \
    MARY_Vector_Each(&vec, type) *it.ptr = (type)MARY_Random(99);  \
  }

    Vector_t vec;
    Vector_Create(&vec, sizeof(type), size);
    type random;
    for (type i = 0; i < size; ++i)
    {
        random = (type)MARY_Random(99);
        //random = 1;
        Vector_Push_Back(&vec, &random);
    }

    //type array[8] = { 0, 6, 8, 2, 5, 5, 4, 6 };
    //Mary_Vector_Create_With(&vec, sizeof(type), (Mary_p) { array, sizeof(array) });

    //VECTOR_Each(&vec, type) printf("%4llu ", it.val); printf("\n");

    //MARY_Benchmark(Mary_Selection_Sort(vec.data, vec.unit, vec.units, Mary_Greater_Than, vec), 1);
    //MARY_Benchmark(Mary_Heapsort(vec.data, vec.unit, vec.units, Heapsort_Sort), 1);
    //MARY_Benchmark(Mary_Heapsort(vec.data, vec.unit, vec.units, Heapsort_Sort), 1);
    //MARY_Benchmark(Mary_Quicksort(vec.data, vec.unit, vec.units, Quicksort_Sort), 1);
    //MARY_Benchmark(qsort(vec.data, vec.units, vec.unit, Qsort_Sort), 1);
    //MARY_Benchmark(Mary_Quicksort_64_3(vec.data, vec.units), 1);
    //MARY_Benchmark(Mary_Heapsort(vec.data, vec.unit, vec.units, Heapsort_Sort), 1);
    //test(qsort(vec.data, vec.units, vec.unit, Qsort_Sort), 8); printf("\n");
    //test(Mary_Quicksort_64(vec.data, 0, vec.units), 8); printf("\n");
    //test(Mary_Quicksort_64_2(vec.data, vec.units), 8); printf("\n");
    //test(Mary_Quicksort_64_3(vec.data, vec.units), 8); printf("\n");

    Mary_Quicksort_64_3(vec.data, vec.units);
    VECTOR_Each(&vec, type) printf("%4llu ", it.val); printf("\n");

    //Vector_Destroy(&vec);

    Mary_Exit_Success();
#endif

#if 0
    Mary_Start();

    Mary_OS_Font_Info_t font_info;
    Mary_Zero(&font_info, sizeof(font_info));
    font_info.face = MARY_C_String(u16, "Times New Roman");
    font_info.hint_px = 36;
    Mary_OS_Font_Change(&font_info);

    Mary_Wordmap_t wordmap;
    Mary_Wordmap_Create(&wordmap, &font_info, 0);
    Mary_Wordmap_Add(&wordmap, MARY_C_String(u16, "love"), 0);
    Mary_Wordmap_Add(&wordmap, MARY_C_String(u16, " "), 0);
    Mary_Wordmap_Add(&wordmap, MARY_C_String(u16, "Yahweh!"), 0);
    for (u64 i = 0; i < 4800; ++i)
    {
        Mary_Wordmap_Add(&wordmap, MARY_C_String(u16, "testing"), 0);
    }
    Mary_Wordmap_Add(&wordmap, MARY_C_String(u16, "testing"), 0);
    Mary_Wordmap_Destroy(&wordmap);

    Mary_Finish();

    Mary_Exit_Success();
#endif

#if 0
    Mary_Start();

    // I'm wonder about how to make this interface easier, but still fast.

    Mary_Window_t pink; Mary_Window_Create(&pink);
    Mary_Window_Back_Color(&pink, 1.0f, 0.4f, 0.8f, 1.0f);
    Mary_Window_Padding(&pink, 84.0f, 24.0f, 84.0f, 24.0f);

    Mary_Div_t blue; Mary_Div_Create(&blue);
    Mary_Element_Append_To(&blue, &pink);
    Mary_Element_Back_Color(&blue, 0.0f, 0.0f, 1.0f, 1.0f);
    Mary_Element_Unit_Size(&blue, MARY_ELEMENT_UNIT_FIT_PARENT, MARY_ELEMENT_UNIT_PIXEL); // MARY_ELEMENT_UNIT_FIT_CHILDREN
    Mary_Element_Padding(&blue, 24.0f, 24.0f, 24.0f, 24.0f);
    Mary_Element_Size(&blue, 600, 250);

    Mary_Div_t purple; Mary_Div_Create(&purple);
    Mary_Element_Append_To(&purple, &blue);
    Mary_Element_Back_Color(&purple, 0.4f, 0.3f, 0.8f, 1.0f);
    Mary_Element_Unit_Size(&purple, MARY_ELEMENT_UNIT_PIXEL, MARY_ELEMENT_UNIT_PIXEL);
    Mary_Element_Size(&purple, 50, 100);
    Mary_Element_Display(&purple, MARY_ELEMENT_DISPLAY_INLINE_LOOSE);

    Mary_Div_t green; Mary_Div_Create(&green);
    Mary_Element_Append_To(&green, &blue);
    Mary_Element_Back_Color(&green, 0.4f, 0.7f, 0.3f, 1.0f);
    Mary_Element_Unit_Size(&green, MARY_ELEMENT_UNIT_PIXEL, MARY_ELEMENT_UNIT_PIXEL);
    Mary_Element_Size(&green, 50, 50);
    Mary_Element_Margin(&green, 12.0f, 12.0f, 0.0f, 12.0f);
    Mary_Element_Display(&green, MARY_ELEMENT_DISPLAY_INLINE_LOOSE);

    Mary_Div_t orange; Mary_Div_Create(&orange);
    Mary_Element_Append_To(&orange, &blue);
    Mary_Element_Back_Color(&orange, 0.8f, 0.3f, 0.3f, 1.0f);
    Mary_Element_Unit_Size(&orange, MARY_ELEMENT_UNIT_PIXEL, MARY_ELEMENT_UNIT_PIXEL);
    Mary_Element_Size(&orange, 500, 150);
    Mary_Element_Margin(&orange, 0.0f, 0.0f, 0.0f, 24.0f);
    Mary_Element_Display(&orange, MARY_ELEMENT_DISPLAY_INLINE_LOOSE);

    #pragma warning (disable: 4028)
    //Mary_Event_Add_Listener(&pink, MARY_EVENT_MOUSEWHEEL, mousewheel_pink, 0, 0);
    //Mary_Event_Add_Listener(&blue, MARY_EVENT_MOUSEWHEEL, mousewheel_blue, 0, 0);
    //Mary_Event_Add_Listener(&blue, MARY_EVENT_MOUSEDOWN, mousedown_blue, 1, 0);
    //Mary_Event_Add_Listener(&orange, MARY_EVENT_MOUSEDOWN, mousedown_orange, 0, 0);
    //Mary_Event_Add_Listener(&pink, MARY_EVENT_MOUSECLICK, mouseclick_pink, 0, 0);
    Mary_Event_Add_Listener(&purple, MARY_EVENT_MOUSEIN, mousein_purple, 0, 0);
    Mary_Event_Add_Listener(&green, MARY_EVENT_MOUSEOUT, mouseout_green, 0, 0);
    Mary_Event_Add_Listener(&pink, MARY_EVENT_KEYPRESS, keypress_pink, 0, 0);
    #pragma warning (default: 4028)

    Mary_Window_Open(&pink);

    while (Mary_Window_Can_Render())
    {
        Mary_Window_Render_All();
    }

    Mary_Element_Destroy_Family(&pink);

    Mary_Stop();
#endif

#if 0
    // broken at the moment. but we need to change system anyway.
    Mary_Start();

    Mary_OS_Font_Info_t font_info; Mary_Zero(&font_info, sizeof(font_info));
    font_info.face = u"Segoe UI";
    font_info.hint_px = 16;
    Mary_OS_Font_Change(&font_info);

    Mary_Window_t window; Mary_Window_Create(&window);
    Mary_Window_Back_Color(&window, 1.0f, 1.0f, 1.0f, 1.0f);

    Mary_Div_t wrap1; Mary_Div_Create(&wrap1);
    Mary_Element_Append_To(&wrap1, &window);
    Mary_Element_Back_Color(&wrap1, 1.0f, 1.0f, 1.0f, 1.0f);
    Mary_Element_Unit_Size(&wrap1, MARY_ELEMENT_UNIT_FIT_PARENT, MARY_ELEMENT_UNIT_FIT_PARENT);
    Mary_Element_Padding(&wrap1, 24.0f, 24.0f, 24.0f, 24.0f);
    Mary_Element_Size(&wrap1, 0, 0);
    Mary_Element_Overflow(&wrap1, MARY_ELEMENT_OVERFLOW_Y);

    Mary_Text_t text; Mary_Text_Create_With_File(&text, 8, "test3.txt");
    Mary_Element_Append_To(&text, &wrap1);
    Mary_Element_Fore_Color(&text, 0.0f, 0.0f, 0.0f, 1.0f);

    Mary_Window_Open(&window); // should this be done automatically on create? I wonder...

    while (Mary_Window_Can_Render())
    {
        Mary_Window_Render_All();
    }

    Mary_Element_Destroy_Family(&window);

    Mary_Stop();
#endif

#if 0
    Mary_Pool_Start();
    Mary_Regex_Start();

    #define PRINT_MATCHES(MATCHES)                                                         \
    MARY_Range(MATCHES.data, Mary_Slice_t, 0, MATCHES.units)                             \
      printf("from: %llu to_exclusive: %llu\n", it.val.from, it.val.to_exclusive);

    if (1)
    {
        Mary_Regex_t regex; Mary_Vector_t matches;

        //MARY_Benchmark(Mary_Regex_Create(&regex, 32, U"(?<[1-9]+z+)a", "g"), 1);
        //MARY_Benchmark(matches = Mary_Regex_Execute(&regex, 32, U"a12zzzza 5zza a"), 12);

        //MARY_Benchmark(Mary_Regex_Create(&regex, 32, U"b(?>12+3?)", "g"), 1);
        //MARY_Benchmark(matches = Mary_Regex_Execute(&regex, 32, U"b122222bdb123ba"), 12);

        //MARY_Benchmark(Mary_Regex_Create(&regex, 32, U"~:Hebrew Char:", "g"), 1);
        //MARY_Benchmark(matches = Mary_Regex_Execute(&regex, 32, U"Ἄνθρωπος בְּרֵאשִׁ֖ית בָּרָ֣א πρὸς πάντα"), 12);

        //MARY_Benchmark(Mary_Regex_Create(&regex, 32, U"^a$", "gm"), 1);
        //MARY_Benchmark(matches = Mary_Regex_Execute(&regex, 32, U"a\na"), 12);

        //MARY_Benchmark(Mary_Regex_Create(&regex, 32, U"a{3,2}", "g"), 1);
        //MARY_Benchmark(matches = Mary_Regex_Execute(&regex, 32, U"aaaaa"), 12);

        //MARY_Benchmark(Mary_Regex_Create(&regex, 32, U"~:Latin:+", "g"), 1);
        //MARY_Benchmark(matches = Mary_Regex_Execute(&regex, 32, U"abc"), 12);

        MARY_Benchmark(Mary_Regex_Create(&regex, 32, U"[1-3]", "g"), 1);
        MARY_Benchmark(matches = Mary_Regex_Execute(&regex, 32, U"123"), 12);

        PRINT_MATCHES(matches);

        Mary_Vector_Destroy(&matches); Mary_Regex_Destroy(&regex);
    }

    if (0)
    {
        Mary_Regex_t _1, _2, _3, _4; Mary_Vector_t v_1, v_2, v_3, v_4;
        u32 *bound_test = U"This is a misty island.";
        "01234567891111111111222";
        "          0123456789012";
        MARY_Benchmark(Mary_Regex_Create(&_1, 32, U"~bis~b", "g"), 1);
        MARY_Benchmark(Mary_Regex_Create(&_2, 32, U"~bis~B", "g"), 1);
        MARY_Benchmark(Mary_Regex_Create(&_3, 32, U"~Bis~b", "g"), 1);
        MARY_Benchmark(Mary_Regex_Create(&_4, 32, U"~Bis~B", "g"), 1); printf("\n");
        MARY_Benchmark(v_1 = Mary_Regex_Execute(&_1, 32, bound_test), 12);
        MARY_Benchmark(v_2 = Mary_Regex_Execute(&_2, 32, bound_test), 12);
        MARY_Benchmark(v_3 = Mary_Regex_Execute(&_3, 32, bound_test), 12);
        MARY_Benchmark(v_4 = Mary_Regex_Execute(&_4, 32, bound_test), 12); printf("\n");
        PRINT_MATCHES(v_1); PRINT_MATCHES(v_2); PRINT_MATCHES(v_3); PRINT_MATCHES(v_4);
        Mary_Vector_Destroy(&v_1); Mary_Vector_Destroy(&v_2); Mary_Vector_Destroy(&v_3); Mary_Vector_Destroy(&v_4);
        Mary_Regex_Destroy(&_1); Mary_Regex_Destroy(&_2); Mary_Regex_Destroy(&_3); Mary_Regex_Destroy(&_4);
    }

    #undef PRINT_MATCHES

    Mary_Regex_Finish();
    Mary_Pool_Stop();

    Mary_Exit_Success();
#endif
    return 0;
}
