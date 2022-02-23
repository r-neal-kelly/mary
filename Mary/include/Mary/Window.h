#pragma once

#include <stdint.h>
#include <Mary/OS.h>
#include <Mary/OpenGL.h>
#include <Mary/Hashmap.h>
#include <Mary/Element.h>
#include <Mary/Event.h>

enum
{
  MARY_WINDOW_STATE_OPENED,
  MARY_WINDOW_STATE_CLOSED
};

struct Mary_Window_t
{
  MARY_Element_t;
  Mary_OS_Window_t os;
  Mary_Vector_t messages;
  Mary_Pool_t *event_pool;
  uint64_t is_dirty;
  Mary_Matrix_4x4f projection;
  Mary_Element_t *active_elem;
  Mary_Element_t *last_mousedown_elem;
  Mary_Element_t *last_scroll_elem; double last_scroll_time;
  Mary_Element_t *last_mousemove_elem;
  int state; // I think this is currently unused...and should prob be renamed to opened
};

void Mary_Window_Start();
void Mary_Window_Finish();
char Mary_Window_Can_Render();
void Mary_Window_Render_All();
void Mary_Window_Create(Mary_Window_t *mary_window);
void Mary_Window_Destroy(Mary_Window_t *mary_window);
void Mary_Window_Open(Mary_Window_t *mary_window);
void Mary_Window_Close(Mary_Window_t *mary_window);
void Mary_Window_Refresh(Mary_Window_t *mary_window);

void Mary_Window_Update_Size(void *mary_window);
void Mary_Window_Update_Positions(void *mary_window);
void Mary_Window_Render(void *mary_window);
void Mary_Window_Flag_Dirty(Mary_Window_t *mary_window);
void Mary_Window_Flag_Last_Scroll_Elem(Mary_Window_t *mary_window, void *mary_element);

void Mary_Window_Show(Mary_Window_t *mary_window);
void Mary_Window_Hide(Mary_Window_t *mary_window);
void Mary_Window_Activate_Element(Mary_Window_t *mary_window, void *mary_element);
void Mary_Window_Back_Color(Mary_Window_t *mary_window, float r, float g, float b, float a);
void Mary_Window_Padding(Mary_Window_t *window, float l, float t, float r, float b);

#define MARY_Window(PTR) ((Mary_Window_t *)(PTR))
