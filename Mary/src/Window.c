#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Mary/Utils.h>
#include <Mary/Bitbool.h>
#include <Mary/Vector.h>
#include <Mary/String.h>
#include <Mary/Window.h>
#include <Mary/Text.h>

MARY_Primitives;

static void Mary_Window_Handle_Messages(Mary_Window_t *window);

static Mary_Vector_t windows_v;

void Mary_Window_Start()
{
  Mary_Vector_Create(&windows_v, MARY_Allocator_Heap, sizeof(Mary_Window_t *), 0);
}

void Mary_Window_Finish()
{
  Mary_Vector_Destroy(&windows_v);
}

char Mary_Window_Can_Render()
{
  return !MARY_Vector_Is_Empty(&windows_v);
}

void Mary_Window_Render_All()
{
  r64 start = Mary_OS_Milliseconds();
  MARY_Vector_Each(&windows_v, Mary_Window_t *)
  {
    Mary_OS_Window_Handle_Messages(it.val);
    if (it.val->parent == 0 && it.val->is_dirty)
    {
      Mary_Window_Update_Size(it.val);
      Mary_Window_Update_Positions(it.val);
      Mary_Window_Render(it.val);
    }
    Mary_Window_Handle_Messages(it.val);
  }
  r64 finish = Mary_OS_Milliseconds();
  r64 sleep = 16.0 - (finish - start);
  Mary_OS_Sleep((u64)(sleep < 0 ? 0 : sleep));
}

static void Mary_Window_Handle_Messages(Mary_Window_t *window)
{
  // the macro stops any looping after the original end of the vector
  // so anything added in the loop will not be dispatched again.
  MARY_Vector_Each(&window->messages, Mary_Event_t *)
  {
    if (it.val->type == MARY_EVENT_KEYDOWN)
    {
      // we might want to cache last key on window, to see if it's repeating.
      Mary_Event_Keydown_t *keydown = (Mary_Event_Keydown_t *)it.val;
      Mary_Event_Dispatch(keydown, window->active_elem);
      if (MARY_Event_Keydown_Is_Keypress(keydown))
      {
        Mary_Event_Keypress_t *keypress = Mary_Event_Create(window, MARY_EVENT_KEYPRESS);
        switch (keydown->key)
        {
          case MARY_EVENT_KEY_ENTER:
            keypress->key = '\n'; break;
          case MARY_EVENT_KEY_TAB:
            keypress->key = '\t'; break;
          default:
            keypress->key = keydown->key;
        }
        keypress->buttons = keydown->buttons;
        keypress->control = keydown->control;
        keypress->shift = keydown->shift;
        keypress->alt = keydown->alt;
        Mary_Event_Dispatch(keypress, window->active_elem);
      }
    }
    else if (it.val->type == MARY_EVENT_KEYUP)
    {
      Mary_Event_Keyup_t *keyup = (Mary_Event_Keyup_t *)it.val;
      Mary_Event_Dispatch(keyup, window->active_elem);
    }
    else if (it.val->type == MARY_EVENT_MOUSEDOWN)
    {
      Mary_Event_Mousedown_t *event = (Mary_Event_Mousedown_t *)it.val;
      window->last_mousedown_elem = Mary_Element_XY_To_Element(window, event->x, event->y);
      Mary_Event_Dispatch(event, window->last_mousedown_elem);
    }
    else if (it.val->type == MARY_EVENT_MOUSEUP)
    {
      Mary_Event_Mousedown_t *event = (Mary_Event_Mousedown_t *)it.val;
      Mary_Element_t *mouseup_elem = Mary_Element_XY_To_Element(window, event->x, event->y);
      Mary_Event_Dispatch(event, mouseup_elem);
      if (window->last_mousedown_elem == mouseup_elem)
      {
        Mary_Event_Mouseclick_t *click = Mary_Event_Create(window, MARY_EVENT_MOUSECLICK);
        window->active_elem = mouseup_elem;
        click->x = event->x;
        click->y = event->y;
        click->button = event->button;
        click->buttons = event->buttons;
        click->control = event->control;
        click->shift = event->shift;
        click->alt = event->alt;
        Mary_Event_Dispatch(click, mouseup_elem);
      }
    }
    else if (it.val->type == MARY_EVENT_MOUSEWHEEL)
    {
      Mary_Event_Mousewheel_t *event = (Mary_Event_Mousewheel_t *)it.val;
      double now = Mary_OS_Milliseconds();
      if (now - window->last_scroll_time < 250)
      {
        window->last_scroll_time = now;
        Mary_Event_Dispatch(event, window->last_scroll_elem);
      }
      else
      {
        window->last_scroll_elem = Mary_Element_XY_To_Element(window, event->x, event->y);
        window->last_scroll_time = now;
        if (window->last_scroll_elem != 0)
        {
          Mary_Event_Dispatch(event, window->last_scroll_elem);
        }
      }
    }
    else if (it.val->type == MARY_EVENT_MOUSEMOVE)
    {
      Mary_Event_Mousemove_t *mousemove = (Mary_Event_Mousemove_t *)it.val;
      Mary_Element_t *mousemove_elem = Mary_Element_XY_To_Element(window, mousemove->x, mousemove->y);
      Mary_Event_Dispatch(mousemove, mousemove_elem);
      if (window->last_mousemove_elem != mousemove_elem)
      {
        Mary_Event_Mousein_t *mousein = Mary_Event_Create(window, MARY_EVENT_MOUSEIN);
        mousein->x = mousemove->x;
        mousein->y = mousemove->y;
        mousein->buttons = mousemove->buttons;
        mousein->control = mousemove->control;
        mousein->shift = mousemove->shift;
        mousein->alt = mousemove->alt;
        mousein->from = window->last_mousemove_elem;
        Mary_Event_Mouseout_t *mouseout = Mary_Event_Create(window, MARY_EVENT_MOUSEOUT);
        mouseout->x = mousemove->x;
        mouseout->y = mousemove->y;
        mousein->buttons = mousemove->buttons;
        mousein->control = mousemove->control;
        mousein->shift = mousemove->shift;
        mousein->alt = mousemove->alt;
        mouseout->to = mousemove_elem;
        window->last_mousemove_elem = mousemove_elem;
        Mary_Event_Dispatch(mousein, mouseout->to);
        Mary_Event_Dispatch(mouseout, mousein->from);
      }
    }
  }
  MARY_Vector_Empty(&window->messages);
  Mary_Pool_Empty(window->event_pool);
}

void Mary_Window_Create(Mary_Window_t *window)
{
  Mary_Element_Create(window, MARY_ELEMENT_WINDOW);
  Mary_Vector_Create(&window->messages, MARY_Allocator_Heap, sizeof(Mary_Event_t *), 8);
  window->event_pool = Mary_Pool_Create(1024); // this has to be a pool, variable event sizes. 1024 is sound?
  window->active_elem = MARY_Element(window);
  window->last_mousedown_elem = MARY_Element(window);
  window->last_scroll_elem = MARY_Element(window);
  window->last_mousemove_elem = MARY_Element(window);
}

void Mary_Window_Destroy(Mary_Window_t *window)
{
  Mary_Pool_Destroy(window->event_pool);
  Mary_Vector_Destroy(&window->messages);
  Mary_Element_Destroy(window);
}

void Mary_Window_Open(Mary_Window_t *window)
{
  MARY_Vector_Push(&windows_v, Mary_Window_t *, window);
  Mary_OS_Window_Create(window);
  Mary_Window_Show(window);
  window->is_dirty = 1;
}

void Mary_Window_Close(Mary_Window_t *window)
{
  // we can add a confirm func handler call in here. or at least send run handlers.
  window->is_dirty = 0;
  Mary_OS_Window_Destroy(window);
  Mary_Vector_Erase(&windows_v, &window);
}

void Mary_Window_Refresh(Mary_Window_t *window)
{
  Mary_Window_Update_Size(window);
  Mary_Window_Update_Positions(window);
  Mary_Window_Render(window);
}

void Mary_Window_Update_Size(void *mary_window)
{
  Mary_Window_t *window = MARY_Window(mary_window);
  Mary_Element_Handle_Unit_Pixel_W(window);
  Mary_Element_Handle_Unit_Pixel_H(window);
  Mary_Element_Handle_Children_Display(window);
  Mary_Element_Handle_Scroll_Max(window);
}

void Mary_Window_Update_Positions(void *mary_window)
{
  Mary_Window_t *window = MARY_Window(mary_window);
  Mary_Element_Handle_Children_Positions(window);
}

void Mary_Window_Render(void *mary_window)
{
  Mary_Window_t *window = MARY_Window(mary_window);
  window->projection = Mary_OpenGL_Ortho(0.0f, window->w, window->h, 0.0f, 0.0f, 1.0f);
  Mary_OS_Window_Make_Current(window); // we may want to be able to turn anti-aliasing off for text render.
  glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glViewport(0, 0, (u32)window->w, (u32)window->h);
  glEnable(GL_SCISSOR_TEST); glScissor(0, 0, (u32)window->w, (u32)window->h);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glClearColor(window->back_r, window->back_g, window->back_b, window->back_a);
  glClear(GL_COLOR_BUFFER_BIT);
  Mary_Element_Render_Children(window);
  Mary_OS_Window_Swap_Buffers(window);
  window->is_dirty = 0;
}

inline void Mary_Window_Show(Mary_Window_t *window)
{
  Mary_OS_Window_Show(window);
}

inline void Mary_Window_Hide(Mary_Window_t *window)
{
  Mary_OS_Window_Hide(window);
}

void Mary_Window_Flag_Dirty(Mary_Window_t *window) // Mary_Window_Updata?
{
  window->is_dirty = 1;
}

void Mary_Window_Flag_Last_Scroll_Elem(Mary_Window_t *window, void *mary_element)
{
  window->last_scroll_elem = mary_element;
}

void Mary_Window_Activate_Element(Mary_Window_t *window, void *mary_element) // Focus element?
{
  // prob will want to set render flag, or something to notify the system
  // also check to make sure the element is attached to this window!!!
  window->active_elem = mary_element;
}

void Mary_Window_Back_Color(Mary_Window_t *window, float r, float g, float b, float a)
{
  Mary_Element_Back_Color(window, r, g, b, a); // might want this as a macro.
  Mary_OS_Window_Back_Color(window);
  window->is_dirty = 1;
}

void Mary_Window_Padding(Mary_Window_t *win, float l, float t, float r, float b)
{
  win->pad_l = l, win->pad_t = t, win->pad_r = r, win->pad_b = b; // might want to have this part as a macro, so that it can act like a super.
  win->clip_pad_l = l, win->clip_pad_t = t, win->clip_pad_r = r, win->clip_pad_b = b;
  // maybe we can put this in OS_Window_Measure?
}
