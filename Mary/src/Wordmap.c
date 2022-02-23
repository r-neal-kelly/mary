#include <stdlib.h>
#include <Mary/Wordmap.h>

MARY_Primitives;

#define MAX_PAGE_WIDTH MARY_GL_MAX_TEXTURE_SIZE
#define MAX_PAGE_HEIGHT 1024

static void Add_Page(Mary_Wordmap_t * wordmap);
static void Expand_Page(Mary_Wordmap_t * wordmap);

void Mary_Wordmap_Create(Mary_Wordmap_t *wordmap, Mary_OS_Font_Info_t *font_info, uint32_t opt_pages)
{
  u32 pages = opt_pages > 0 ? opt_pages : 1;
  Mary_Vector_Create(&wordmap->pages, MARY_Allocator_Heap, sizeof(Mary_Wordmap_Page_t), pages); wordmap->pages.units = pages;
  Mary_Hashmap_Create(&wordmap->words, sizeof(u16) * 32, sizeof(Mary_Wordmap_Word_t));

  memcpy(&wordmap->font_info, font_info, sizeof(Mary_OS_Font_Info_t));
  Mary_OS_Font_Change(&wordmap->font_info);
  wordmap->line_height = Mary_OS_Font_Get_Real_Px();

  MARY_Vector_Each(&wordmap->pages, Mary_Wordmap_Page_t)
  {
    Mary_Wordmap_Page_t *page = it.ptr;
    if (it.idx != it.this->units - 1)
    {
      Mary_Bitmap_Create(&page->bitmap, MAX_PAGE_WIDTH, MAX_PAGE_HEIGHT, 32);
    }
    else
    {
      Mary_Bitmap_Create(&page->bitmap, MAX_PAGE_WIDTH, wordmap->line_height, 32);
    }
    Mary_GL_Texture_Create(&page->gl_texture, GL_TEXTURE_2D);
    // should we do more here to get things rolling?
  }

  wordmap->free_page = wordmap->pages.data;
  wordmap->free_x = 0;
  wordmap->free_y = 0;
}

void Mary_Wordmap_Destroy(Mary_Wordmap_t *wordmap)
{
  MARY_Vector_Each(&wordmap->pages, Mary_Wordmap_Page_t)
  {
    Mary_Wordmap_Page_t *page = it.ptr;
    Mary_Bitmap_Destroy(&page->bitmap);
    Mary_GL_Texture_Destroy(&page->gl_texture);
  }
  Mary_Hashmap_Destroy(&wordmap->words);
  Mary_Vector_Destroy(&wordmap->pages);
}

static void Add_Page(Mary_Wordmap_t *wordmap)
{
  u64 idx;
  MARY_Vector_Each(&wordmap->pages, Mary_Wordmap_Page_t)
  {
    if (wordmap->free_page == it.ptr)
    {
      idx = it.idx; break;
    }
  }

  if (idx == wordmap->pages.units -1)
  {
    Mary_Wordmap_Page_t page;
    Mary_Bitmap_Create(&page.bitmap, MAX_PAGE_WIDTH, wordmap->line_height, 32);
    Mary_GL_Texture_Create(&page.gl_texture, GL_TEXTURE_2D);
    MARY_Vector_Push(&wordmap->pages, Mary_Wordmap_Page_t, page);
  }

  wordmap->free_page = MARY_Vector_Point(&wordmap->pages, idx + 1);
  wordmap->free_x = 0;
  wordmap->free_y = 0;
}

static void Expand_Page(Mary_Wordmap_t * wordmap)
{
  Mary_Bitmap_t *old_bitmap = &wordmap->free_page->bitmap, new_bitmap;
  Mary_Bitmap_Create(&new_bitmap, MAX_PAGE_WIDTH, old_bitmap->height + wordmap->line_height, 32);
  Mary_Bitmap_Destroy(old_bitmap);
  *old_bitmap = new_bitmap;

  Mary_Vector_t words; Mary_Hashmap_Values(&wordmap->words, &words, 0);
  MARY_Vector_Each(&words, Mary_Wordmap_Word_t *)
  {
    Mary_Wordmap_Word_t *word = it.val;
    Mary_Wordmap_Page_t *page = word->page;
    word->y1_norm = (float)word->y1_px / page->bitmap.height;
    word->y2_norm = (float)word->y2_px / page->bitmap.height;
  }
}

void Mary_Wordmap_Add(Mary_Wordmap_t *wordmap, uint16_t *word_data, uint8_t opt_word_units)
{
  // and easy way to check if we already have the words or not.
  //if (Mary_Hashmap_Contains_Key)

  u64 word_units = MARY_Truthy(opt_word_units) ?
    opt_word_units : Mary_C_String_Count_Units(word_data, 16, 0);

  Mary_OS_Font_Change(&wordmap->font_info);

  Mary_OS_Textmap2_Info_t textmap_info;
  Mary_Zero(&textmap_info, sizeof(textmap_info));
  textmap_info.text_data = word_data;
  textmap_info.text_units = (u32)word_units;
  textmap_info.enum_align = MARY_OS_TEXTMAP_ALIGN_LEFT;
  textmap_info.bool_alpha = MARY_TRUE;

  Mary_OS_Textmap2_t text;
  Mary_OS_Textmap2_Create(&text, &textmap_info);

  // check that there is enough room in page for word
  if (wordmap->free_x + text.bitmap.width > wordmap->free_page->bitmap.width)
  {
    wordmap->free_x = 0;
    wordmap->free_y += wordmap->line_height;
    if (wordmap->free_page->bitmap.height + wordmap->line_height > MAX_PAGE_HEIGHT)
    {
      Add_Page(wordmap);
    }
    else if (wordmap->free_y + wordmap->line_height > wordmap->free_page->bitmap.height)
    {
      Expand_Page(wordmap);
    }
  }

  // add word to hashmap. has to have normalized values.
  {
    Mary_Wordmap_Word_t word;
    word.page = wordmap->free_page;
    word.w_px = text.bitmap.width;
    word.h_px = text.bitmap.height;
    word.x1_px = wordmap->free_x;
    word.x2_px = word.x1_px + (u16)word.w_px;
    word.y1_px = wordmap->free_y;
    word.y2_px = word.y1_px + (u16)word.h_px;
    word.x1_norm = (float)word.x1_px / MAX_PAGE_WIDTH;
    word.x2_norm = (float)word.x2_px / MAX_PAGE_WIDTH;
    word.y1_norm = (float)word.y1_px / word.page->bitmap.height;
    word.y2_norm = (float)word.y2_px / word.page->bitmap.height;
    u16 u16s[32]; Mary_Zero(u16s, sizeof(u16s));
    memcpy(u16s, word_data, word_units * sizeof(u16));
    Mary_Hashmap_Assign(&wordmap->words, &u16s, &word);
  }

  // copy word into page at free location
  Mary_Bitmap_Write_32bit(&wordmap->free_page->bitmap, wordmap->free_x, wordmap->free_y, &text.bitmap);
  wordmap->free_x += text.bitmap.width;

  Mary_OS_Textmap2_Destroy(&text);
}

Mary_Wordmap_Word_t *Mary_Wordmap_Get(Mary_Wordmap_t *wordmap, uint16_t *word_data, uint8_t opt_word_units)
{
  return 0;
}

void Mary_Wordmap_Empty(Mary_Wordmap_t *wordmap)
{
  Mary_Hashmap_Empty(&wordmap->words);
  wordmap->free_page = MARY_Vector_Point_Front(&wordmap->pages);
  wordmap->free_x = 0;
  wordmap->free_y = 0;
}
