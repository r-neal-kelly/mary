#include <string.h>
#include <stdio.h>
#include <Mary/Utils.h>
#include <Mary/Bitbool.h>

MARY_Primitives;

void Mary_Bitbool_Assign(void *bitbool, size_t bitbool_size, size_t bit_index, char boolean)
{
  size_t byte_index = bit_index / 8;
  if (byte_index <= bitbool_size)
  {
    u8 *p = (u8 *)bitbool + byte_index;
    bit_index %= 8;
    boolean = (boolean > 0) ? 1 : 0;
    u8 byte = (boolean) ? (*p | (1 << bit_index)) : (*p & ~(1 << bit_index));
    memmove(p, &byte, 1);
  }
}

char Mary_Bitbool_At(void *bitbool, size_t bitbool_size, size_t bit_index)
{
  char result = 0;
  size_t byte_index = bit_index / 8;
  if (byte_index <= bitbool_size)
  {
    u8 *p = (u8 *)bitbool + byte_index;
    bit_index %= 8;
    result = (*p & (1 << bit_index)) ? 1 : 0;
  }
  return result;
}

void Mary_Bitbool_Fill(void *bitbool, size_t bitbool_size, char boolean)
{
  u8 *p = (u8 *)bitbool;
  u8 value = (boolean > 0) ? ~0 : 0;
  for (size_t z = 0; z < bitbool_size; ++z, p += 1)
  {
    memset(p, value, 1);
  }
}

void Mary_Bitbool_Print(void *bitbool, size_t bitbool_size)
{
  size_t total_bits = bitbool_size * 8;
  for (size_t i = 0; i < total_bits; ++i)
  {
    printf("%zu: %i\n", i, Mary_Bitbool_At(bitbool, bitbool_size, i));
  }
}
