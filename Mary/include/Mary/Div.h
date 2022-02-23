#pragma once

#include <Mary/Element.h>
#include <Mary/Event.h>

struct Mary_Div_t
{
  MARY_Element_t;
};

void Mary_Div_Start();
void Mary_Div_Finish();
void Mary_Div_Create(Mary_Div_t *mary_div);
void Mary_Div_Destroy(Mary_Div_t *mary_div);

void Mary_Div_Update_Size(void *mary_div);
void Mary_Div_Update_Positions(void *mary_div);
void Mary_Div_Render(void *mary_div);

#define MARY_Div(PTR) ((Mary_Div_t *)(PTR))
