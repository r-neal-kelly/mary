#pragma once

#include <Mary/String.h>
#include <Mary/GL_Texture.h>
#include <Mary/Element.h>
#include <Mary/Event.h>

struct Mary_Text_t
{
  MARY_Element_t;
  Mary_String_t string;
  Mary_OS_Textmap_t textmap;
  Mary_GL_Texture_t texture;
};

void Mary_Text_Start();
void Mary_Text_Finish();
void Mary_Text_Create(Mary_Text_t *mary_text, char bit_format, void *text, size_t opt_units);
void Mary_Text_Create_With_File(Mary_Text_t *elem, char bit_format, char *file_path);
//void Mary_Text_Create_With_String(Mary_Text_t *mary_text, Mary_String_t *mary_string);
void Mary_Text_Destroy(Mary_Text_t *mary_text);
//void Mary_Text_Change(Mary_Text_t *mary_text, uint16_t *text);

void Mary_Text_Update_Size(void *mary_text);
void Mary_Text_Update_Positions(void *mary_text);
void Mary_Text_Render(void *mary_text);

#define MARY_Text(PTR) ((Mary_Text_t *)(PTR))
