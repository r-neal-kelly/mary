#pragma once

#include <stdint.h>
#include <Mary/Pool.h>
#include <Mary/Vector.h>
#include <Mary/Bitmap.h>
#include <Mary/String.h>

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <Windows.h>
#undef ERROR // what a crappy name to put in global space!
#define MARY_CALL WINAPI

typedef struct
{
  HWND handle;
  HDC context;
  HBRUSH brush; // maybe not necessary...
}
Mary_OS_Window_t;

#elif defined(__linux__)

#define to_be_done

#endif

// this needs to be removed, see Mary_Random in utils.c
#define MARY_Random(LIMIT)\
  ( ( 0xcbf29ce484222325 ^ (uint64_t)Mary_OS_Microseconds() ) * 0x00000100000001b3 % ( (LIMIT) + 1 ) )

#define MARY_Benchmark(CODE, TRIALS)                                    \
MARY_M                                                                  \
  uint64_t trials = TRIALS;                                             \
  double us = Mary_OS_Microseconds(), s;                                \
  for (uint64_t i = 0; i < trials; ++i) { CODE; }                       \
  us = Mary_OS_Microseconds() - us;                                     \
  if (trials > 1) us /= trials;                                         \
  s = us / 1000000.0;                                                   \
  printf("%s\n", #CODE);                                                \
  printf("%10llu trials %10llu us %10f s\n", trials, (uint64_t)us, s);  \
MARY_W

typedef struct Mary_Window_t Mary_Window_t;

void Mary_OS_Start();
void Mary_OS_Finish();

void Mary_OS_Sleep(size_t milliseconds); // consider making float for param.
double Mary_OS_Microseconds();
double Mary_OS_Milliseconds();
double Mary_OS_Seconds();

// whenever we make an OS_Thread_t, whenever one is created, make sure
// to create the global Arena for that thread in OS func.
uint64_t Mary_OS_Thread_Current_ID();

void Mary_OS_Window_Create(Mary_Window_t *window);
void Mary_OS_Window_Destroy(Mary_Window_t *window);
void Mary_OS_Window_Show(Mary_Window_t *window);
void Mary_OS_Window_Hide(Mary_Window_t *window);
void Mary_OS_Window_Measure(Mary_Window_t *window);
void Mary_OS_Window_Handle_Messages(Mary_Window_t *window);
void Mary_OS_Window_Make_Current(Mary_Window_t *window);
void Mary_OS_Window_Swap_Buffers(Mary_Window_t *window);
void Mary_OS_Window_Back_Color(Mary_Window_t *window);

typedef struct Mary_OS_Directory_t Mary_OS_Directory_t;
typedef struct Mary_OS_Directory_Item_t Mary_OS_Directory_Item_t;

struct Mary_OS_Directory_t
{
  Mary_Vector_t items;
  void *os_data;
};

struct Mary_OS_Directory_Item_t
{
  Mary_Enum_t type;
  Mary_C_String_16_p name;
  // maybe add the relative path?
};

enum Mary_OS_Directory_Item_e
{
  MARY_OS_DIRECTORY_ITEM_DIRECTORY,
  MARY_OS_DIRECTORY_ITEM_FILE
};

void Mary_OS_Directory_Create(Mary_OS_Directory_t *directory, Mary_String_t *path);
void Mary_OS_Directory_Destroy(Mary_OS_Directory_t *directory);

void *Mary_OS_File_Create(Mary_String_t *path, Mary_Enum_t file_mode);



// in the future, we might want to combine these two function types below more, so that the user can have
// multiple dibs, one for each font, or whatever.

enum
{
  MARY_OS_TEXTMAP_ALIGN_LEFT,
  MARY_OS_TEXTMAP_ALIGN_RIGHT,
  MARY_OS_TEXTMAP_LEFT_TO_RIGHT,
  MARY_OS_TEXTMAP_RIGHT_TO_LEFT
};

typedef struct
{
  Mary_Bitmap_t bitmap;
  void *os_data;
}
Mary_OS_Textmap2_t; // eventually Mary_OS_Textmap_t

typedef struct
{
  uint16_t *text_data;
  uint32_t text_units;
  uint8_t enum_align;
  uint8_t enum_direction;
  uint8_t bool_alpha;
  uint8_t bool_rgba;
  uint8_t max_width;
}
Mary_OS_Textmap2_Info_t;

void Mary_OS_Textmap2_Create(Mary_OS_Textmap2_t *textmap, Mary_OS_Textmap2_Info_t *info);
void Mary_OS_Textmap2_Destroy(Mary_OS_Textmap2_t *textmap);

typedef struct
{
  uint16_t *face;
  uint16_t hint_px;
  uint8_t bold;
  uint8_t italic;
  uint8_t underline;
  uint8_t strikeout;
}
Mary_OS_Font_Info_t;

void Mary_OS_Font_Change(Mary_OS_Font_Info_t *font_info);
uint16_t *Mary_OS_Font_Get_Face();
uint16_t Mary_OS_Font_Get_Real_Px();

//// everything below here is to be deleted. ////

typedef struct Mary_OS_Textmap_t Mary_OS_Textmap_t;
typedef struct Mary_OS_Textmap_Line_t Mary_OS_Textmap_Line_t;
typedef struct Mary_OS_Textmap_Word_t Mary_OS_Textmap_Word_t;
typedef Mary_Vector_t Mary_OS_Textmap_Line_v;
typedef Mary_Vector_t Mary_OS_Textmap_Word_v;

struct Mary_OS_Textmap_t
{
  Mary_Pool_t *cache;
  Mary_Bitmap_t bitmap; // we might make a vector of these, if we need to break up horizontally, and we could put an index on the line objects.
  Mary_OS_Textmap_Line_v lines;
};

struct Mary_OS_Textmap_Line_t
{
  Mary_OS_Textmap_Word_v words;
  float w, h, user_w, user_h; // these might just be ints.
};

struct Mary_OS_Textmap_Word_t
{
  float w, h, x1, x2, y1, y2;
};

void Mary_OS_Textmap_Create(Mary_OS_Textmap_t *textmap, uint16_t *text, int text_units);
void Mary_OS_Textmap_Destroy(Mary_OS_Textmap_t *textmap);
