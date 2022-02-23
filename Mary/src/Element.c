#include <stdio.h>
#include <string.h>
#include <Mary/OpenGL.h>
#include <Mary/Element.h>
#include <Mary/Event.h>
#include <Mary/Window.h>
#include <Mary/Div.h>
#include <Mary/Text.h>

MARY_Primitives;

typedef void (*Type_f)(void *);

typedef struct
{
  size_t bytes;
  Type_f Update_Size;
  Type_f Update_Positions;
  Type_f Render;
  Type_f Destroy;
}
Type_Info;

typedef struct
{
  float w, h;
  int units;
}
Row;

static Mary_Vector_t type_infos; // we might want to break this up into smaller vectors, but it might be more manual to input the values.

void Mary_Element_Start()
{
  Type_Info type_info;
  Mary_Vector_Create(&type_infos, MARY_Allocator_Heap, sizeof(Type_Info), MARY_ELEMENT_TYPES);

  #define TYPE_INFO(TYPE)                           \
    type_info = (Type_Info)                         \
    {                                               \
      sizeof(Mary_##TYPE##_t),                      \
      Mary_##TYPE##_Update_Size,                    \
      Mary_##TYPE##_Update_Positions,               \
      Mary_##TYPE##_Render,                         \
      Mary_##TYPE##_Destroy                         \
    };                                              \
    MARY_Vector_Push(&type_infos, Type_Info, type_info)

  TYPE_INFO(Window);
  TYPE_INFO(Div);
  TYPE_INFO(Text);

  #undef TYPE_INFO
}

void Mary_Element_Finish()
{
  Mary_Vector_Destroy(&type_infos);
}

void Mary_Element_Create(void *mary_element, size_t type)
{
  Mary_Element_t *element = MARY_Element(mary_element);
  Type_Info *type_info = MARY_Vector_Point(&type_infos, type);
  memset(element, 0, type_info->bytes); // may just want to memset the element size here?
  element->type = type;
  if (type == MARY_ELEMENT_WINDOW) element->window = MARY_Window(element);
  Mary_Vector_Create(&element->children_s, MARY_Allocator_Heap, sizeof(Mary_Element_t *), 64);
  Mary_Vector_Create(&element->children_z, MARY_Allocator_Heap, sizeof(Mary_Element_t *), 64);
  Mary_Vector_Create(&element->rows, MARY_Allocator_Heap, sizeof(Row), 64);
  Mary_Event_Create_Listeners(&element->listeners);
  element->display = MARY_ELEMENT_DISPLAY_LINE;
  element->unit_w = MARY_ELEMENT_UNIT_FIT_PARENT;
  element->unit_h = MARY_ELEMENT_UNIT_FIT_CHILDREN;
  element->back_a = 1, element->fore_a = 1, element->border_a = 1;
  element->overflow = MARY_ELEMENT_OVERFLOW_Y; // not sure if this should be default yet.
  element->align_h = MARY_ELEMENT_ALIGN_LEFT;
  element->align_v = MARY_ELEMENT_ALIGN_TOP;
}

void Mary_Element_Destroy(void *mary_element)
{
  Mary_Element_t *element = MARY_Element(mary_element);
  Type_Info *type_info = MARY_Vector_Point(&type_infos, element->type);
  MARY_Vector_Each(&element->children_s, Mary_Element_t *)
  {
    Mary_Element_Remove(it.val);
  }
  Mary_Vector_Destroy(&element->children_s);
  Mary_Vector_Destroy(&element->children_z);
  Mary_Vector_Destroy(&element->rows);
  Mary_Event_Destroy_Listeners(&element->listeners);
  memset(element, 0, type_info->bytes); // may just want to memset the element size here?
}

void Mary_Element_Destroy_Family(void *mary_element)
{
  Mary_Element_t *parent = MARY_Element(mary_element);
  MARY_Vector_Stack(family, Mary_Element_t *, 256);
  MARY_Vector_Push(&family, Mary_Element_t *, parent);
  for (u64 i = 0; i < family.units; ++i)
  {
    parent = MARY_Vector_At(&family, Mary_Element_t *, i);
    MARY_Vector_Each(&parent->children_s, Mary_Element_t *)
    {
      MARY_Vector_Push(&family, Mary_Element_t *, it.val);
    }
  }
  MARY_Vector_Each_Reverse(&family, Mary_Element_t *)
  {
    Type_Info *info = MARY_Vector_Point(&type_infos, it.val->type);
    info->Destroy(it.val);
  }
}

static void Measure_Children_Size(Mary_Element_t *parent)
{
  float w_children = 0, h_children = 0; Row row = { 0, 0, 0 };
  MARY_Vector_Empty(&parent->rows);

  MARY_Vector_Each(&parent->children_s, Mary_Element_t *)
  {
    Mary_Element_t *child = it.val;

    if (child->display == MARY_ELEMENT_DISPLAY_NONE)
    {
      child->w_margin = 0;
      child->h_margin = 0;
      child->w = 0;
      child->h = 0;
      child->w_pad = 0;
      child->h_pad = 0;
      continue;
    }

    if (child->unit_w == MARY_ELEMENT_UNIT_FIT_PARENT || child->unit_w == MARY_ELEMENT_UNIT_FIT_CHILDREN)
    {
      child->w_margin = parent->w_pad;
      child->w = child->w_margin - child->margin_l - child->margin_r;
      child->w_pad = child->w - child->pad_l - child->pad_r;
    }
    else if (child->unit_w == MARY_ELEMENT_UNIT_PIXEL)
    {
      child->w_margin = child->w + child->margin_l + child->margin_r;
      child->w_pad = child->w - child->pad_l - child->pad_r;
    }

    if (child->unit_h == MARY_ELEMENT_UNIT_FIT_PARENT || child->unit_h == MARY_ELEMENT_UNIT_FIT_CHILDREN)
    {
      child->h_margin = parent->h_pad;
      child->h = child->h_margin - child->margin_t - child->margin_b;
      child->h_pad = child->h - child->pad_t - child->pad_b;
    }
    else if (child->unit_h == MARY_ELEMENT_UNIT_PIXEL)
    {
      child->h_margin = child->h + child->margin_t + child->margin_b;
      child->h_pad = child->h - child->pad_t - child->pad_b;
    }

    Measure_Children_Size(child);

    if (child->unit_w == MARY_ELEMENT_UNIT_FIT_CHILDREN)
    {
      child->w_pad = child->w_children;
      child->w = child->w_pad + child->pad_l + child->pad_r;
      child->w_margin = child->w + child->margin_l + child->margin_r;
    }

    if (child->unit_h == MARY_ELEMENT_UNIT_FIT_CHILDREN)
    {
      child->h_pad = child->h_children;
      child->h = child->h_pad + child->pad_t + child->pad_b;
      child->h_margin = child->h + child->margin_t + child->margin_b;
    }

    if (child->display == MARY_ELEMENT_DISPLAY_LINE)
    {
      if (row.units > 0)
      {
        if (w_children < row.w) w_children = row.w;
        h_children += row.h;
        MARY_Vector_Push(&parent->rows, Row, row);
      }
      if (w_children < child->w_margin) w_children = child->w_margin;
      h_children += child->h_margin;
      row.w = child->w_margin;
      row.h = child->h_margin;
      row.units = 1;
      MARY_Vector_Push(&parent->rows, Row, row);
      row.w = 0;
      row.h = 0;
      row.units = 0;
    }
    else if (child->display == MARY_ELEMENT_DISPLAY_INLINE_LOOSE)
    {
      if (row.w + child->w_margin > parent->w_pad)
      {
        if (w_children < row.w) w_children = row.w;
        h_children += row.h;
        MARY_Vector_Push(&parent->rows, Row, row);
        row.w = 0;
        row.h = 0;
        row.units = 0;
      }
      row.w += child->w_margin;
      if (row.h < child->h_margin) row.h = child->h_margin;
      ++row.units;
    }
    else if (child->display == MARY_ELEMENT_DISPLAY_INLINE_TIGHT)
    {
      // doesn't include margin_t or margin_b, perhaps also only ever fits children.
    }
  }

  if (row.units > 0)
  {
    if (w_children < row.w) w_children = row.w;
    h_children += row.h;
    MARY_Vector_Push(&parent->rows, Row, row);
  }

  // we want this here so it affects the widow object starting the recursion.
  parent->w_children = w_children;
  parent->h_children = h_children;
  parent->scroll_x_max = (parent->w_children <= parent->w_pad) ? 0 : parent->w_children - parent->w_pad;
  parent->scroll_y_max = (parent->h_children <= parent->h_pad) ? 0 : parent->h_children - parent->h_pad;
}

static u64 Clip(Mary_Element_t *elem)
{
  // would could prob cache these clip values in a struct in the func parameters, instead of on element?
  float left = elem->parent->clip_x1 + elem->parent->clip_pad_l;
  float right = elem->parent->clip_x2 - elem->parent->clip_pad_r;
  float top = elem->parent->clip_y1 + elem->parent->clip_pad_t;
  float bottom = elem->parent->clip_y2 - elem->parent->clip_pad_b;

  if (right <= left || elem->x1 >= right || elem->x2 <= left) return 1; // full clip
  if (bottom <= top || elem->y1 >= bottom || elem->y2 <= top) return 1; // full clip
  
  elem->clip_x1 = (elem->x1 < left) ? left : elem->x1;
  elem->clip_x2 = (elem->x2 > right) ? right : elem->x2;
  float clip_pad_l = elem->pad_l - (elem->clip_x1 - elem->x1);
  float clip_pad_r = elem->pad_r - (elem->x2 - elem->clip_x2);
  elem->clip_pad_l = (clip_pad_l < 0) ? 0 : clip_pad_l;
  elem->clip_pad_r = (clip_pad_r < 0) ? 0 : clip_pad_r;

  elem->clip_y1 = (elem->y1 < top) ? top : elem->y1;
  elem->clip_y2 = (elem->y2 > bottom) ? bottom : elem->y2;
  float clip_pad_t = elem->pad_t - (elem->clip_y1 - elem->y1);
  float clip_pad_b = elem->pad_b - (elem->y2 - elem->clip_y2);
  elem->clip_pad_t = (clip_pad_t < 0) ? 0 : clip_pad_t;
  elem->clip_pad_b = (clip_pad_b < 0) ? 0 : clip_pad_b;

  float parent_w = elem->parent->clip_x2 - elem->parent->clip_x1;
  float parent_h = elem->parent->clip_y2 - elem->parent->clip_y1;

  glScissor((GLint)(elem->parent->clip_x1 + elem->parent->clip_pad_l),
            (GLint)(elem->window->h - elem->parent->clip_y2 + elem->parent->clip_pad_b),
            (GLuint)(parent_w - elem->parent->clip_pad_l - elem->parent->clip_pad_r),
            (GLuint)(parent_h - elem->parent->clip_pad_t - elem->parent->clip_pad_b));
  return 0;
}

void *Mary_Element_XY_To_Element(Mary_Window_t *window, int x, int y)
{
  // we still need to get the one with the highest z
  // we need an option to return all elements at the z and filter them. that's the below func, or an option in this one
  // we can also clear the buffer periodically since we are not keeping it?
  MARY_Vector_Stack(net, Mary_Element_t *, 16);
  MARY_Vector_Stack(buffer, Mary_Element_t *, 256);
  MARY_Vector_Push(&buffer, Mary_Window_t *, window);

  for (u64 i = 0; i < buffer.units; ++i)
  {
    Mary_Element_t *element = *(Mary_Element_t **)MARY_Vector_Point(&buffer, i);
    MARY_Vector_Each(&element->children_s, Mary_Element_t *)
    {
      MARY_Vector_Push(&buffer, Mary_Element_t *, it.val);
    }
    if (MARY_Element_Hit_Test(element, x, y))
    {
      MARY_Vector_Push(&net, Mary_Element_t *, element);
    }
  }

  if (net.units == 0)
  {
    return 0;
  }
  else
  {
    return *(Mary_Element_t **)MARY_Vector_Point(&net, net.units - 1);
  }
}

void Mary_Element_XY_To_Elements(Mary_Window_t *window, int x, int y, Mary_Vector_t *elements)
{

}

void Mary_Element_Update_Size(void *mary_element)
{
  // this causes an element to update itself
  Mary_Element_t *elem = MARY_Element(mary_element);
  Type_Info *type_info = MARY_Vector_Point(&type_infos, elem->type);
  type_info->Update_Size(elem);
}

void Mary_Element_Update_Positions(void *mary_element)
{
  // but this one causes an element to update its children
  Mary_Element_t *elem = MARY_Element(mary_element);
  Type_Info *type_info = MARY_Vector_Point(&type_infos, elem->type);
  type_info->Update_Positions(elem);
}

void Mary_Element_Handle_Unit_Fit_Parent_W(void *mary_element)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  elem->w_margin = elem->parent->w_pad;
  elem->w = elem->w_margin - elem->margin_l - elem->margin_r;
  elem->w_pad = elem->w - elem->pad_l - elem->pad_r;
}

void Mary_Element_Handle_Unit_Fit_Parent_H(void *mary_element)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  elem->h_margin = elem->parent->h_pad;
  elem->h = elem->h_margin - elem->margin_t - elem->margin_b;
  elem->h_pad = elem->h - elem->pad_t - elem->pad_b;
}

void Mary_Element_Handle_Unit_Pixel_W(void *mary_element)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  elem->w_margin = elem->w + elem->margin_l + elem->margin_r;
  elem->w_pad = elem->w - elem->pad_l - elem->pad_r;
}

void Mary_Element_Handle_Unit_Pixel_H(void *mary_element)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  elem->h_margin = elem->h + elem->margin_t + elem->margin_b;
  elem->h_pad = elem->h - elem->pad_t - elem->pad_b;
}

void Mary_Element_Handle_Children_Display(void *mary_element)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  float w_children = 0, h_children = 0;
  Row row = { 0, 0, 0 }; MARY_Vector_Empty(&elem->rows);

  MARY_Vector_Each(&elem->children_s, Mary_Element_t *)
  {
    Mary_Element_t *child = it.val;

    Mary_Element_Update_Size(child);

    if (child->display == MARY_ELEMENT_DISPLAY_LINE)
    {
      if (row.units > 0)
      {
        if (w_children < row.w) w_children = row.w;
        h_children += row.h;
        MARY_Vector_Push(&elem->rows, Row, row);
      }
      if (w_children < child->w_margin) w_children = child->w_margin;
      h_children += child->h_margin;
      row.w = child->w_margin;
      row.h = child->h_margin;
      row.units = 1;
      MARY_Vector_Push(&elem->rows, Row, row);
      row.w = 0;
      row.h = 0;
      row.units = 0;
    }
    else if (child->display == MARY_ELEMENT_DISPLAY_INLINE_LOOSE)
    {
      if (row.w + child->w_margin > elem->w_pad)
      {
        if (w_children < row.w) w_children = row.w;
        h_children += row.h;
        MARY_Vector_Push(&elem->rows, Row, row);
        row.w = 0;
        row.h = 0;
        row.units = 0;
      }
      row.w += child->w_margin;
      if (row.h < child->h_margin) row.h = child->h_margin;
      ++row.units;
    }
    else if (child->display == MARY_ELEMENT_DISPLAY_INLINE_TIGHT)
    {
      // doesn't include margin_t or margin_b, perhaps also only ever fits children.
    }
  }

  if (row.units > 0)
  {
    if (w_children < row.w) w_children = row.w;
    h_children += row.h;
    MARY_Vector_Push(&elem->rows, Row, row);
  }

  elem->w_children = w_children;
  elem->h_children = h_children;
}

void Mary_Element_Handle_Scroll_Max(void *mary_element)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  elem->scroll_x_max = (elem->w_children <= elem->w_pad) ? 0 : elem->w_children - elem->w_pad;
  elem->scroll_y_max = (elem->h_children <= elem->h_pad) ? 0 : elem->h_children - elem->h_pad;
  if (elem->scroll_x > elem->scroll_x_max) elem->scroll_x = elem->scroll_x_max;
  if (elem->scroll_y > elem->scroll_y_max) elem->scroll_y = elem->scroll_y_max;
}

void Mary_Element_Handle_Children_Positions(void *mary_element)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  float x1 = elem->x1 + elem->pad_l;
  float y1 = elem->y1 + elem->pad_t;
  float x_origin = x1;

  if (elem->overflow == MARY_ELEMENT_OVERFLOW_Y)
  {
    y1 -= elem->scroll_y;
  }
  else if (elem->overflow == MARY_ELEMENT_OVERFLOW_X)
  {
    x1 -= elem->scroll_x;
  }
  else if (elem->overflow == MARY_ELEMENT_OVERFLOW_XY)
  {
    y1 -= elem->scroll_y, x1 -= elem->scroll_x;
  }

  Row *row; int row_idx = 0;
  row = MARY_Vector_Point(&elem->rows, row_idx);

  MARY_Vector_Each(&elem->children_s, Mary_Element_t *)
  {
    Mary_Element_t *child = it.val;

    if (child->display == MARY_ELEMENT_DISPLAY_NONE)
    {
      child->x1 = 0;
      child->x2 = 0;
      child->y1 = 0;
      child->y2 = 0;
      continue;
    }
    else
    {
      child->x1 = x1 + child->margin_l;
      child->x2 = child->x1 + child->w;
      x1 += child->w_margin;
      child->y1 = y1 + child->margin_t;
      child->y2 = child->y1 + child->h;
    }

    if (x1 - x_origin == row->w)
    {
      x1 = x_origin;
      y1 += row->h;
      ++row_idx; row = MARY_Vector_Point(&elem->rows, row_idx);
    }

    Mary_Element_Update_Positions(child);
  }
}

static void Render_Children(Mary_Element_t *parent)
{
  MARY_Vector_Each(&parent->children_z, Mary_Element_t *)
  {
    if (!Clip(it.val))
    {
      Type_Info *type_info = MARY_Vector_Point(&type_infos, it.val->type);
      type_info->Render(it.val);
      Render_Children(it.val);
    }
  }
}

void Mary_Element_Render_Children(Mary_Window_t *window)
{
  Render_Children(MARY_Element(window));
}

static void Mary_Element_Window(Mary_Element_t *element, Mary_Window_t *window)
{
  element->window = window;
  MARY_Vector_Each(&element->children_s, Mary_Element_t *)
  {
    Mary_Element_Window(it.val, window);
  }
}

void Mary_Element_Append_To(void *mary_element, void *mary_element_parent)
{
  #define LAST_CHILD_Z(PARENT_PTR)\
    (*(Mary_Element_t **)MARY_Vector_Point(&PARENT_PTR->children_z, PARENT_PTR->children_z.units - 1))

  Mary_Element_t *element = MARY_Element(mary_element);
  Mary_Element_t *parent = MARY_Element(mary_element_parent);

  ////// remove from old parent
  if (element->parent != 0)
  {
    MARY_Vector_Each(&element->parent->children_s, Mary_Element_t *)
    {
      if (it.val == element)
      {
        Mary_Vector_Erase_At(&element->parent->children_s, it.idx); break;
      }
    }
    MARY_Vector_Each(&element->parent->children_z, Mary_Element_t *)
    {
      if (it.val == element)
      {
        Mary_Vector_Erase_At(&element->parent->children_z, it.idx); break;
      }
    }
    Mary_Window_Flag_Dirty(element->window);
  }

  ////// add to new parent
  MARY_Vector_Push(&parent->children_s, Mary_Element_t *, element);
  if (parent->children_z.units == 0)
  {
    MARY_Vector_Push(&parent->children_z, Mary_Element_t *, element);
  }
  else if (LAST_CHILD_Z(parent)->z <= element->z)
  {
    MARY_Vector_Push(&parent->children_z, Mary_Element_t *, element);
  }
  else
  {
    MARY_Vector_Each(&parent->children_z, Mary_Element_t *)
    {
      if (it.val->z > element->z)
      {
        Mary_Vector_Push_At(&parent->children_z, it.idx, &element);
      }
    }
  }
  Mary_Element_Window(element, parent->window);
  Mary_Window_Flag_Dirty(element->window);
  element->parent = parent;

  #undef LAST_CHILD_Z
}

void Mary_Element_Remove(void *mary_element)
{
  Mary_Element_t *element = MARY_Element(mary_element);
  if (element->parent != 0)
  {
    MARY_Vector_Each(&element->parent->children_s, Mary_Element_t *)
    {
      if (it.val == element)
      {
        Mary_Vector_Erase_At(&element->parent->children_s, it.idx); break;
      }
    }
    MARY_Vector_Each(&element->parent->children_z, Mary_Element_t *)
    {
      if (it.val == element)
      {
        Mary_Vector_Erase_At(&element->parent->children_z, it.idx); break;
      }
    }
    Mary_Window_Flag_Dirty(element->window);
    Mary_Element_Window(element, 0);
    element->parent = 0;
  }
}

void Mary_Element_Size(void *mary_element, float w, float h)
{
  Mary_Element_t *element = MARY_Element(mary_element);
  element->w = w, element->h = h;
}

void Mary_Element_Unit_Size(void *mary_element, int unit_w, int unit_h)
{
  Mary_Element_t *element = MARY_Element(mary_element);
  element->unit_w = unit_w, element->unit_h = unit_h;
  // we shouldn't allow this to change window_t...
  // makes me think we should have these funcs in each of their own types...
}

void Mary_Element_Back_Color(void *mary_element, float r, float g, float b, float a)
{
  Mary_Element_t *element = MARY_Element(mary_element);
  element->back_r = r, element->back_g = g, element->back_b = b, element->back_a = a;
}

void Mary_Element_Fore_Color(void *mary_element, float r, float g, float b, float a)
{
  Mary_Element_t *element = MARY_Element(mary_element);
  element->fore_r = r, element->fore_g = g, element->fore_b = b, element->fore_a = a;
}

void Mary_Element_Border_Color(void *mary_element, float r, float g, float b, float a)
{
  Mary_Element_t *element = MARY_Element(mary_element);
  element->border_r = r, element->border_g = g, element->border_b = b, element->border_a = a;
}

void Mary_Element_Margin(void *mary_element, float l, float t, float r, float b)
{
  Mary_Element_t *element = MARY_Element(mary_element);
  element->margin_l = l, element->margin_t = t, element->margin_r = r, element->margin_b = b;
}

void Mary_Element_Padding(void *mary_element, float l, float t, float r, float b)
{
  Mary_Element_t *element = MARY_Element(mary_element);
  element->pad_l = l, element->pad_t = t, element->pad_r = r, element->pad_b = b;
}

void Mary_Element_Scroll_Y_By(void *mary_element, float scroll_y_by)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  float scroll_y_to = elem->scroll_y + scroll_y_by;
  scroll_y_to = MARY_Clip_Number(scroll_y_to, 0, elem->scroll_y_max);

  if (elem->scroll_y != scroll_y_to)
  {
    elem->scroll_y = scroll_y_to;
    Mary_Window_Flag_Dirty(elem->window);
  }
}

void Mary_Element_Scroll_Y_To(void *mary_element, float scroll_y_to)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  scroll_y_to = MARY_Clip_Number(scroll_y_to, 0, elem->scroll_y_max);

  if (elem->scroll_y != scroll_y_to)
  {
    elem->scroll_y = scroll_y_to;
    Mary_Window_Flag_Dirty(elem->window);
  }
}

void Mary_Element_Scroll_X_By(void *mary_element, float scroll_x_by)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  float scroll_x_to = elem->scroll_x + scroll_x_by;
  scroll_x_to = MARY_Clip_Number(scroll_x_to, 0, elem->scroll_x_max);

  if (elem->scroll_x != scroll_x_to)
  {
    elem->scroll_x = scroll_x_to;
    Mary_Window_Flag_Dirty(elem->window);
  }
}

void Mary_Element_Scroll_X_To(void *mary_element, float scroll_x_to)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  scroll_x_to = MARY_Clip_Number(scroll_x_to, 0, elem->scroll_x_max);

  if (elem->scroll_x != scroll_x_to)
  {
    elem->scroll_x = scroll_x_to;
    Mary_Window_Flag_Dirty(elem->window);
  }
}

void Mary_Element_Display(void *mary_element, int display)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  elem->display = display;
}

void Mary_Element_Overflow(void *mary_element, int enum_overflow)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  elem->overflow = enum_overflow;
}
