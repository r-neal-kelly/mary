#pragma once

#include <stdio.h>

#define TEST_START(FUNC_NAME)\
  printf("Running %-24s...", "'" #FUNC_NAME "'")
#define TEST_STOP\
  printf(" Ok!\n")

void Test_Vector();
void Test_String();
void Test_Unicode();
void Test_Regex();




// the rest of this should be assimilated into separate files without a header.
void Test_Mary_Pool_Reallocate();

void Test_MARY_String();
void Test_String_Create();
void Test_String_Recode();
void Test_String_Loops();
void Test_String_Recode_Speed();
void Test_String_Copy_To();
void Test_String_Decode_Speed();

void Test_Big_Return();
void Test_Bit_Flags();
