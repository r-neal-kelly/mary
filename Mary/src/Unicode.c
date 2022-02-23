#include <Mary/Allocator.h>
#include <Mary/Arena.h>
#include <Mary/Vector.h>
#include <Mary/C_String.h>
#include <Mary/File.h>
#include <Mary/Unicode.h>

MARY_Primitives;

typedef struct Mary_Unicode_Case_Fold_sys Mary_Unicode_Case_Fold_sys;

struct Mary_Unicode_Case_Fold_sys {
    Mary_Vector_t from;
    Mary_Vector_t to;
};

static Mary_Unicode_Case_Fold_sys case_fold_common;
static Mary_Unicode_Case_Fold_sys case_fold_simple;
static Mary_Unicode_Case_Fold_sys case_fold_full;
static Mary_Bool_t is_started = MARY_FALSE;

void Mary_Unicode_Start()
{
    if (is_started == MARY_TRUE) {
        return;
    } else {
        Mary_Arena_Start();
        is_started = MARY_TRUE;
    }

    MARY_Arena_In;

    ////// Case Folding //////
    MARY_Vector_Heap_p(&case_fold_common.from, Mary_Char_32_t, 1376);
    MARY_Vector_Heap_p(&case_fold_common.to, Mary_Char_32_t, 1376);
    MARY_Vector_Heap_p(&case_fold_simple.from, Mary_Char_32_t, 28);
    MARY_Vector_Heap_p(&case_fold_simple.to, Mary_Char_32_t, 28);
    MARY_Vector_Heap_p(&case_fold_full.from, Mary_Char_32_t, 104);
    Mary_Vector_Create(&case_fold_full.to, MARY_Allocator_Heap, sizeof(u32) * 5, 104);

    Mary_File_t case_folding_file;
    MARY_String_Stack(case_folding_path, 16, u"Data/Unicode/CaseFolding.txt");
    Mary_File_String_Create(&case_folding_file, &case_folding_path, MARY_FILE_READ, 8);
    Mary_String_t case_folding_line;
    Mary_String_Create(&case_folding_line, MARY_Allocator_Frame, 8, 256);

    Mary_C_String_8_t str;
    Mary_Char_8_t chr;
    Mary_u64_t hex;
    Mary_Char_32_t *full_val;

    while (Mary_File_String_Read_Line(&case_folding_file, &case_folding_line)) {
        str = case_folding_line.data;
        chr = str[0];

        if (chr == '#' || chr == '\n' || chr == '\r') {
            continue;
        }

        hex = Mary_C_String_To_Hexadecimal(str, 8, &str);
        str += 2;
        chr = *str;
        str += 3;

        if (chr == 'C') {
            MARY_Vector_Push(&case_fold_common.from, u32, (u32)hex);
            hex = Mary_C_String_To_Hexadecimal(str, 8, 0);
            MARY_Vector_Push(&case_fold_common.to, u32, (u32)hex);
        } else if (chr == 'S') {
            MARY_Vector_Push(&case_fold_simple.from, u32, (u32)hex);
            hex = Mary_C_String_To_Hexadecimal(str, 8, 0);
            MARY_Vector_Push(&case_fold_simple.to, u32, (u32)hex);
        } else if (chr == 'F') {
            MARY_Vector_Push(&case_fold_full.from, u32, (u32)hex);
            full_val = MARY_Vector_Point_Push(&case_fold_full.to);
            full_val[0] = 1;
            while (MARY_TRUE) {
                full_val[full_val[0]] =
                    (u32)Mary_C_String_To_Hexadecimal(str, 8, &str);
                full_val[0] += 1;
                if (*str == ';') {
                    full_val[full_val[0]] = 0;
                    break;
                } else {
                    str += 1;
                }
            }
        }
    }

    Mary_File_Destroy(&case_folding_file);

    MARY_Arena_Out;
}

void Mary_Unicode_Stop()
{
    if (is_started == MARY_FALSE) {
        return;
    }

    Mary_Vector_Destroy(&case_fold_common.from);
    Mary_Vector_Destroy(&case_fold_common.to);
    Mary_Vector_Destroy(&case_fold_simple.from);
    Mary_Vector_Destroy(&case_fold_simple.to);
    Mary_Vector_Destroy(&case_fold_full.from);
    Mary_Vector_Destroy(&case_fold_full.to);

    is_started = MARY_FALSE;
}

Mary_Char_32_t Mary_Unicode_Fold_Case_Simple(Mary_Char_32_t char_32)
{
    MARY_Assert(is_started == MARY_TRUE,
                "Mary_Unicode has not been started.");

    Mary_Unicode_Case_Fold_sys *fold_case = &case_fold_simple;
    Mary_Index_t idx;
    Mary_Char_32_t *ptr;
    Mary_Bool_t was_found;

    MARY_Vector_Binary_Search(&fold_case->from, idx, ptr,
                              char_32, *ptr,
                              { was_found = MARY_TRUE; },
                              { was_found = MARY_FALSE; });
    if (was_found) {
        return MARY_Vector_At(&fold_case->to, Mary_Char_32_t, idx);
    } else {
        fold_case = &case_fold_common;
        MARY_Vector_Binary_Search(&fold_case->from, idx, ptr,
                                  char_32, *ptr,
                                  { was_found = MARY_TRUE; },
                                  { was_found = MARY_FALSE; });
    }

    if (was_found) {
        return MARY_Vector_At(&fold_case->to, Mary_Char_32_t, idx);
    } else {
        return char_32;
    }
}

void Mary_Unicode_Fold_Full(Mary_Char_32_t char_32, Mary_String_t *out_str)
{
    MARY_Assert(MARY_String_Is_Created(out_str),
                "'out_str' has not been created.");


}
