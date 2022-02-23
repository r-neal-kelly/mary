#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Mary/Utils.h>
#include <Mary/String.h>
#include <Mary/OS.h>

MARY_Primitives;

void Mary_Exit_Success()
{
  printf("\npress enter to exit...\n");
  getc(stdin); exit(EXIT_SUCCESS);
}

void Mary_Exit_Success_Message(char *message)
{
  printf("\n%s\n", message);
  printf("\npress enter to exit...\n");
  getc(stdin); exit(EXIT_SUCCESS);
}

void Mary_Exit_Failure(char *error_string)
{
  printf("\nMARY FATAL ERROR: %s\n", error_string);
  printf("\npress enter to exit...\n");
  getc(stdin); exit(EXIT_FAILURE);
}

void Mary_Exit_Assert(char *assertion, char *message, char *func, char *file, int line)
{
  printf("\nMARY ASSERT: ( %s ) is not true!\n", assertion);
  if (message != 0) printf("    message: %s\n", message);
  printf("    in func: %s\n", func);
  printf("    in file: %s\n", file);
  printf("    on line: %i\n", line);
  printf("\npress enter to exit...\n");
  getc(stdin); exit(EXIT_FAILURE);
}

Mary_Bool_t Mary_Is_Little_Endian()
{
  volatile u16 i = 0x0001;
  return *(u8 *)&i == 1;
}

Mary_Bool_t Mary_Is_Big_Endian()
{
  volatile u16 i = 0x0001;
  return *(u8 *)&i == 0;
}

void Mary_Print_Bits(void *value, Mary_Size_t bytes, Mary_Bool_t little_endian)
{
  char *binary_digits[16] =
  {
    "0000", "0001", "0010", "0011",
    "0100", "0101", "0110", "0111",
    "1000", "1001", "1010", "1011",
    "1100", "1101", "1110", "1111"
  };
  u8 byte = 0, *p = little_endian ? (u8 *)value + bytes - 1 : value;
  for (int i = 0; i < 76; ++i) printf("-"); printf("\n");
  for (size_t i = 0; i < bytes; i += 8)
  {
    if (!i)
      printf("bin: ");
    else if (i % 8 == 0)
      printf("     ");
    for (int j = 0; j < 8; ++j)
    {
      if (i + j == bytes) break;
      byte = little_endian ? *(p - (i + j)) : *(p + i + j);
      printf("%s", binary_digits[(byte & 0xF0) >> 4]);
      printf("%s ", binary_digits[byte & 0x0F]);
    } printf("\n");

    if (!i)
      printf("hex: ");
    else if (i % 8 == 0)
      printf("     ");
    for (int j = 0; j < 8; ++j)
    {
      if (i + j == bytes) break;
      byte = little_endian ? *(p - (i + j)) : *(p + i + j);
      printf("%4X", (byte & 0xF0) >> 4);
      printf("%4X ", byte & 0x0F);
    } printf("\n");
  }
  for (int i = 0; i < 76; ++i) printf("-"); printf("\n");
}

uint64_t Mary_Random(uint64_t from, uint64_t to)
{
    uint64_t ms = (uint64_t)Mary_OS_Microseconds();
    uint64_t hash = (0xcbf29ce484222325 ^ ms) * 0x100000001b3;
    return hash % (to - from + 1) + from;
}

void *Mary_Alloc(Mary_Size_t bytes)
{
  void *data = malloc(bytes);
  MARY_Assert(data != 0, "Out of memory.");
  return data;
}

void *Mary_Calloc(Mary_Size_t unit, Mary_Size_t units)
{
  void *data = calloc(units, unit);
  MARY_Assert(data != 0, "Out of memory.");
  return data;
}

void *Mary_Realloc(void *data, Mary_Size_t bytes)
{
  void *new_data = realloc(data, bytes);
  MARY_Assert(new_data != 0, "Out of memory.");
  return new_data;
}

void Mary_Dealloc(void *data)
{
  free(data);
}

void Mary_Zero(void *data, Mary_Size_t bytes)
{
  if (bytes >= sizeof(u64))
  {
    u64 u64s = bytes / sizeof(u64);
    u64 *data_64 = MARY_Cast(data, u64);
    MARY_Range(data_64, u64, 0, u64s) *it.ptr = 0;
    u64 u8s = bytes - u64s * sizeof(u64);
    u8 *data_8 = MARY_Cast(data_64 + u64s, u8);
    MARY_Range(data_8, u8, 0, u8s) *it.ptr = 0;
  }
  else
  {
    MARY_Range(data, u8, 0, bytes) *it.ptr = 0;
  }
}

void *Mary_Copy(void *from, void *to, Mary_Size_t bytes)
{
  if (from == to) return to;

  if (bytes >= sizeof(u64))
  {
    u64 u64s = bytes / sizeof(u64);
    u64 *to_64 = MARY_Cast(to, u64);
    u64 *from_64 = MARY_Cast(from, u64);
    MARY_Range(to_64, u64, 0, u64s) *it.ptr = *(from_64 + it.idx);
    u64 u8s = bytes - u64s * sizeof(u64);
    u8 *to_8 = MARY_Cast(to_64 + u64s, u8);
    u8 *from_8 = MARY_Cast(from_64 + u64s, u8);
    MARY_Range(to_8, u8, 0, u8s) *it.ptr = *(from_8 + it.idx);
  }
  else
  {
    u8 *to_8 = MARY_Cast(to, u8);
    u8 *from_8 = MARY_Cast(from, u8);
    MARY_Range(to_8, u8, 0, bytes) *it.ptr = *(from_8 + it.idx);
  }
  return to;
}

// do I need to buffer when moveing <--?
void *Mary_Move(void *from, void *to, Mary_Size_t bytes)
{
  if (from == to) return to;

  void *buffer = Mary_Alloc(bytes);
  Mary_Copy(from, buffer, bytes);
  Mary_Copy(buffer, to, bytes);
  Mary_Dealloc(buffer);
  return to;
}

void Mary_Swap(void *one, void *two, Mary_Size_t bytes)
{
  if (one == two) return;

  void *buffer = Mary_Alloc(bytes);
  Mary_Copy(one, buffer, bytes);
  Mary_Copy(two, one, bytes);
  Mary_Copy(buffer, two, bytes);
  Mary_Dealloc(buffer);
}

void Mary_Set(void *data, void *val, Mary_Size_t val_unit, Mary_Size_t val_units)
{
  u8 *data_8 = data;
  for (I i = 0; i < val_units; ++i, data_8 += val_unit)
  {
    Mary_Copy(val, data_8, val_unit);
  }
}

Mary_Bool_t Mary_Is_Same(void *one, void *two, Mary_Size_t bytes)
{
  if (one == two) return MARY_TRUE;

  if (bytes >= sizeof(u64))
  {
    u64 u64s = bytes / sizeof(u64);
    u64 *one_64 = MARY_Cast(one, u64);
    u64 *two_64 = MARY_Cast(two, u64);
    MARY_Range(one_64, u64, 0, u64s)
    {
      if (it.val != *(two_64 + it.idx))
      {
        return MARY_FALSE;
      }
    }
    u64 u8s = bytes - u64s * sizeof(u64);
    u8 *one_8 = MARY_Cast(one_64 + u64s, u8);
    u8 *two_8 = MARY_Cast(two_64 + u64s, u8);
    MARY_Range(one_8, u8, 0, u8s)
    {
      if (it.val != *(two_8 + it.idx))
      {
        return MARY_FALSE;
      }
    }
  }
  else
  {
    u8 *one_8 = MARY_Cast(one, u8);
    u8 *two_8 = MARY_Cast(two, u8);
    MARY_Range(one_8, u8, 0, bytes)
    {
      if (it.val != *(two_8 + it.idx))
      {
        return MARY_FALSE;
      }
    }
  }
  return MARY_TRUE;
}

int64_t Mary_Compare(void *one, void *two, Mary_Size_t bytes)
{
  return 0;
}
