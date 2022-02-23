#pragma once

#include <stdio.h>
#include <Mary/Utils.h>
#include <Mary/Vector.h>
#include <Mary/String.h>

enum Mary_File_Mode_e
{
  MARY_FILE_READ,
  MARY_FILE_READ_WRITE,
  MARY_FILE_WRITE,
  MARY_FILE_WRITE_READ,
  MARY_FILE_APPEND,
  MARY_FILE_APPEND_READ,
};

enum Mary_File_Flag_e
{
  MARY_FILE_STRING,
  MARY_FILE_BYTES,
  MARY_FILE_ERROR,
  MARY_FILE_ENDED
};

typedef struct
{
  FILE *stream;
  Mary_Size_t unit;
  Mary_Size_t units;
  uint64_t flags;
}
Mary_File_t;

// maybe the Create function should be combined with an extra param. I tend to not want to go past four params though.
void Mary_File_String_Create(Mary_File_t *mary_file, Mary_String_t *path, Mary_Enum_t mode, Mary_UTF_t utf);
void Mary_File_Bytes_Create(Mary_File_t *mary_file, Mary_String_t *path, Mary_Enum_t mode, Mary_Size_t unit);
void Mary_File_Destroy(Mary_File_t *mary_file);
Mary_Bool_t Mary_File_Has_String(Mary_File_t *mary_file);
Mary_Bool_t Mary_File_Has_Bytes(Mary_File_t *mary_file);
Mary_Bool_t Mary_File_Has_Error(Mary_File_t *mary_file);
Mary_Bool_t Mary_File_Has_Ended(Mary_File_t *mary_file);
Mary_Bool_t Mary_File_String_Read_Unit(Mary_File_t *mary_file, void *out_unit);
Mary_Bool_t Mary_File_Bytes_Read_Unit(Mary_File_t *mary_file, void *out_unit);
Mary_Bool_t Mary_File_String_Read_Line(Mary_File_t *file, Mary_String_t *out_line);
void Mary_File_String_Read_All(Mary_File_t *mary_file, Mary_String_t *out_string, Mary_Bool_t is_created);
void Mary_File_Bytes_Read_All(Mary_File_t *mary_file, Mary_Vector_t *out_buffer, Mary_Bool_t is_created);

void Mary_File_Write_Unit(Mary_File_t *mary_file, void *in_unit);
// maybe we could have a Write_Line that takes text? maybe. I think we should make it a String_t though?
void Mary_File_String_Write_All(Mary_File_t *mary_file, Mary_String_t *mary_string);
void Mary_File_Bytes_Write_All(Mary_File_t *mary_file, Mary_Vector_t *in_buffer);
void Mary_File_Point_Front(Mary_File_t *mary_file);
