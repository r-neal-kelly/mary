#pragma once

#include <Mary/Utils.h>
#include <Mary/Vector.h>
#include <Mary/Hashmap.h>
#include <Mary/Bitmap.h>
#include <Mary/OS.h>
#include <Mary/GL_Texture.h>

// we will have a global hashmap to wordmaps that the user and lib can use when available.
enum
{
  MARY_WORDMAP_ALPHABET_ALL,
  MARY_WORDMAP_ALPHABET_LATIN,
  MARY_WORDMAP_ALPHABET_HEBREW,
  MARY_WORDMAP_ALPHABET_GREEK
};

typedef struct Mary_Wordmap_t Mary_Wordmap_t;
typedef struct Mary_Wordmap_Page_t Mary_Wordmap_Page_t;
typedef struct Mary_Wordmap_Word_t Mary_Wordmap_Word_t;

typedef Mary_Vector_t Mary_Wordmap_v;
typedef Mary_Vector_t Mary_Wordmap_Page_v;
typedef Mary_Vector_t Mary_Wordmap_Word_v;

struct Mary_Wordmap_t
{
  Mary_Wordmap_Page_v pages;
  Mary_Hashmap_t words;
  Mary_OS_Font_Info_t font_info;
  uint16_t line_height;
  Mary_Wordmap_Page_t *free_page;
  uint16_t free_x, free_y;
};

struct Mary_Wordmap_Page_t
{
  Mary_Bitmap_t bitmap;
  Mary_GL_Texture_t gl_texture;
};

struct Mary_Wordmap_Word_t
{
  Mary_Wordmap_Page_t *page;
  float w_px, h_px;
  uint16_t x1_px, x2_px, y1_px, y2_px;
  float x1_norm, x2_norm, y1_norm, y2_norm;
};

void Mary_Wordmap_Create(Mary_Wordmap_t *wordmap, Mary_OS_Font_Info_t *font_info, uint32_t opt_pages);
void Mary_Wordmap_Destroy(Mary_Wordmap_t *wordmap);
void Mary_Wordmap_Add(Mary_Wordmap_t *wordmap, uint16_t *word_data, uint8_t opt_word_units);
Mary_Wordmap_Word_t *Mary_Wordmap_Get(Mary_Wordmap_t *wordmap, uint16_t *word_data, uint8_t opt_word_units);
void Mary_Wordmap_Empty(Mary_Wordmap_t *wordmap);
// will want to add a func that takes a text and automatically adds all the unique words in it.
// in other words, this unit will have to deal with languages, which will probably be in funcs in
// another unit.
