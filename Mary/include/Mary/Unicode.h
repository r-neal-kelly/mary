#pragma once

#include <Mary/Utils.h>
#include <Mary/String.h>

#define MARY_Unicode_Max 0x10FFFF

// I think we can probably make a sparse array for folding, or a hashmap.
// the sparse array would probably just be binary searched.
// I wonder if I can make a macro binary search. see pool.c implemetation.

void Mary_Unicode_Start();
void Mary_Unicode_Stop();

void Mary_Unicode_Fold_Case_Full();
Mary_Char_32_t Mary_Unicode_Fold_Case_Simple(Mary_Char_32_t char_32);
void Mary_Unicode_Fold_Full(Mary_Char_32_t char_32, Mary_String_t *out_str);

// Mary_Unicode_To_Lowercase();
// Mary_Unicode_To_Uppercase();
// Mary_Unicode_To_Titlecase();
// Mary_Unicode_Is_Lowercase();
// Mary_Unicode_Is_Uppercase();
// Mary_Unicode_Is_Titlecase();
