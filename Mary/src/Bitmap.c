#include <stdlib.h>
#include <string.h>
#include <Mary/Bitmap.h>

MARY_Primitives;

// this will have to be like the string type, where it has separate funcs for bit depths.

void Mary_Bitmap_Create(Mary_Bitmap_t *bitmap, uint16_t width, uint16_t height, uint16_t bit_depth)
{
  MARY_Assert(bit_depth == 32 || bit_depth == 16 || bit_depth == 8, 0);
  bitmap->width = width;
  bitmap->height = height;
  bitmap->bit_depth = bit_depth;
  bitmap->unit = bit_depth >> 3;
  bitmap->bytes = width * height * bitmap->unit;
  bitmap->data = Mary_Alloc(bitmap->bytes); // these need to be paramitized.
  bitmap->allocator = MARY_Allocator_Heap; // param!
}

void Mary_Bitmap_Destroy(Mary_Bitmap_t *bitmap)
{
  free(bitmap->data);
}

char Mary_Bitmap_Fits_Width(Mary_Bitmap_t *bitmap, uint16_t x, uint16_t width)
{
  return x + width <= bitmap->width;
}

char Mary_Bitmap_Fits_Height(Mary_Bitmap_t *bitmap, uint16_t y, uint16_t height)
{
  return y + height <= bitmap->height;
}

char Mary_Bitmap_Fits(Mary_Bitmap_t *bitmap, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
  return x + width <= bitmap->width && y + height <= bitmap->height;
}

void Mary_Bitmap_Write_32bit(Mary_Bitmap_t *bitmap, uint16_t x, uint16_t y, Mary_Bitmap_t *in_bitmap)
{
  u32 *to_ptr = &MARY_Cast(bitmap->data, u32)[y * bitmap->width + x];
  u32 *from_ptr = in_bitmap->data;
  for (u64 i = 0; i < in_bitmap->height; ++i, to_ptr += bitmap->width)
  {
    for (u64 j = 0; j < in_bitmap->width; ++j, ++from_ptr)
    {
      *(to_ptr + j) = *from_ptr;
    }
  }
}
