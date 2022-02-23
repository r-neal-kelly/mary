#pragma once

#include <stdint.h>
#include <Mary/Vector.h>

#define MARY_ELEMENT_TYPES 3

enum
{
  MARY_ELEMENT_WINDOW,
  MARY_ELEMENT_DIV,
  MARY_ELEMENT_TEXT
};

enum
{
  MARY_ELEMENT_UNIT_PIXEL,
  MARY_ELEMENT_UNIT_PERCENT,
  MARY_ELEMENT_UNIT_FIT_PARENT,
  MARY_ELEMENT_UNIT_FIT_CHILDREN
};

enum
{
  MARY_ELEMENT_DISPLAY_LINE,
  MARY_ELEMENT_DISPLAY_INLINE_LOOSE,
  MARY_ELEMENT_DISPLAY_INLINE_TIGHT,
  MARY_ELEMENT_DISPLAY_FLEX,
  MARY_ELEMENT_DISPLAY_GRID,
  MARY_ELEMENT_DISPLAY_NONE
};

enum
{
  MARY_ELEMENT_OVERFLOW_NONE,
  MARY_ELEMENT_OVERFLOW_X,
  MARY_ELEMENT_OVERFLOW_Y,
  MARY_ELEMENT_OVERFLOW_XY
};

enum
{
  MARY_ELEMENT_ALIGN_LEFT,
  MARY_ELEMENT_ALIGN_RIGHT,
  MARY_ELEMENT_ALIGN_TOP,
  MARY_ELEMENT_ALIGN_BOTTOM,
  MARY_ELEMENT_ALIGN_CENTER
};

typedef struct Mary_Element_t Mary_Element_t;
typedef struct Mary_Window_t Mary_Window_t;
typedef struct Mary_Div_t Mary_Div_t;
typedef struct Mary_Text_t Mary_Text_t;

#define MARY_Element_t\
  size_t type;\
  Mary_Window_t *window;\
  Mary_Element_t *parent;\
  Mary_Vector_t children_s; /* sorted by sibling */\
  Mary_Vector_t children_z; /* sorted by z-index */\
  Mary_Vector_t rows;\
  Mary_Vector_t listeners;\
  int display;\
  float x1, x2, y1, y2; int z;\
  float w, h; int unit_w, unit_h;\
  float w_margin, h_margin, w_pad, h_pad;\
  float w_children, h_children;\
  int align_h, align_v;\
  float back_r, back_g, back_b, back_a;\
  float fore_r, fore_g, fore_b, fore_a;\
  float border_r, border_g, border_b, border_a;\
  float margin_l, margin_t, margin_r, margin_b;\
  float pad_l, pad_t, pad_r, pad_b;\
  float clip_x1, clip_x2, clip_y1, clip_y2;\
  float clip_pad_l, clip_pad_t, clip_pad_r, clip_pad_b;\
  float scroll_x, scroll_x_max, scroll_y, scroll_y_max; int overflow

struct Mary_Element_t
{
  MARY_Element_t;
};

typedef Mary_Vector_t Mary_Element_v;

void Mary_Element_Start();
void Mary_Element_Finish();
void Mary_Element_Create(void *mary_element, size_t type);
void Mary_Element_Destroy(void *mary_element);
void Mary_Element_Destroy_Family(void *mary_element); // I want this to be used to destroy an element and its children. will have to cache destroy funcs from types.

void *Mary_Element_XY_To_Element(Mary_Window_t *window, int x, int y);
void Mary_Element_XY_To_Elements(Mary_Window_t *window, int x, int y, Mary_Vector_t *elements);
void Mary_Element_Update_Size(void *mary_element); // Self_Size
void Mary_Element_Update_Positions(void *mary_element); // Children_Positions
void Mary_Element_Handle_Unit_Fit_Parent_W(void *mary_element);
void Mary_Element_Handle_Unit_Fit_Parent_H(void *mary_element);
void Mary_Element_Handle_Unit_Pixel_W(void *mary_element);
void Mary_Element_Handle_Unit_Pixel_H(void *mary_element);
void Mary_Element_Handle_Children_Display(void *mary_element);
void Mary_Element_Handle_Scroll_Max(void *mary_element);
void Mary_Element_Handle_Children_Positions(void *mary_element);
void Mary_Element_Render_Children(Mary_Window_t *window);

void Mary_Element_Append_To(void *mary_element, void *mary_element_parent);
void Mary_Element_Remove(void *mary_element);

void Mary_Element_Size(void *mary_element, float w, float h);
void Mary_Element_Unit_Size(void *mary_element, int unit_w, int unit_h); // Size_Unit?
void Mary_Element_Back_Color(void *mary_element, float r, float g, float b, float a);
void Mary_Element_Fore_Color(void *mary_element, float r, float g, float b, float a);
void Mary_Element_Border_Color(void *mary_element, float r, float g, float b, float a);
void Mary_Element_Margin(void *mary_element, float l, float t, float r, float b);
void Mary_Element_Padding(void *mary_element, float l, float t, float r, float b);
void Mary_Element_Z_Index(void *mary_element, int z);
void Mary_Element_Scroll_Y_By(void *mary_element, float scroll_y_by);
void Mary_Element_Scroll_Y_To(void *mary_element, float scroll_y_to);
void Mary_Element_Scroll_X_By(void *mary_element, float scroll_x_by);
void Mary_Element_Scroll_X_To(void *mary_element, float scroll_x_to);
void Mary_Element_Display(void *mary_element, int display);
void Mary_Element_Overflow(void *mary_element, int enum_overflow);

#define MARY_Element(PTR) ((Mary_Element_t *)(PTR))

#define MARY_Element_Hit_Test(ELEMENT_PTR, X, Y) \
(                                                \
    X >= ELEMENT_PTR->x1 &&                      \
    X <= ELEMENT_PTR->x2 &&                      \
    Y >= ELEMENT_PTR->y1 &&                      \
    Y <= ELEMENT_PTR->y2                         \
)
