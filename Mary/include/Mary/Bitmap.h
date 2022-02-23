#pragma once

#include <Mary/Utils.h>
#include <Mary/Allocator.h>

typedef struct
{
  MARY_Pointer_t;
  uint16_t width;
  uint16_t height;
  uint16_t bit_depth;
  uint16_t unit;
}
Mary_Bitmap_t;

void Mary_Bitmap_Create(Mary_Bitmap_t *bitmap, uint16_t width, uint16_t height, uint16_t bit_depth);
void Mary_Bitmap_Destroy(Mary_Bitmap_t *bitmap);
char Mary_Bitmap_Fits_Width(Mary_Bitmap_t *bitmap, uint16_t x, uint16_t width);
char Mary_Bitmap_Fits_Height(Mary_Bitmap_t *bitmap, uint16_t y, uint16_t height);
char Mary_Bitmap_Fits(Mary_Bitmap_t *bitmap, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void Mary_Bitmap_Write(Mary_Bitmap_t *bitmap, uint16_t x, uint16_t y, Mary_Bitmap_t *in_bitmap);
void Mary_Bitmap_Write_32bit(Mary_Bitmap_t *bitmap, uint16_t x, uint16_t y, Mary_Bitmap_t *in_bitmap);
void Mary_Bitmap_Copy(Mary_Bitmap_t *bitmap, uint16_t x, uint16_t y, uint16_t width, uint16_t height, Mary_Bitmap_t *out_bitmap);
void Mary_Bitmap_Convert_Bit_Depth(Mary_Bitmap_t *bitmap, uint16_t bit_depth);
void Mary_Bitmap_Convert_To_RGBA(Mary_Bitmap_t *bitmap);
void Mary_Bitmap_Convert_To_BGRA(Mary_Bitmap_t *bitmap);
Mary_p *Mary_Bitmap_Point(Mary_Bitmap_t *bitmap, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

#define MARY_Bitmap_Fits_Width(PTR, X, WIDTH)\
  (X + WIDTH <= (PTR)->width)

#define MARY_Bitmap_Fits_Height(PTR, Y, HEIGHT)\
  (Y + HEIGHT <= (PTR)->height)

#define MARY_Bitmap_Fits(PTR, X, Y, WIDTH, HEIGHT)\
  (X + WIDTH <= (PTR)->width && Y + HEIGHT <= (PTR)->height)

#define MARY_Bitmap_Read(PTR, TYPE, X, Y, W, H) // loop?
