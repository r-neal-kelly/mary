#pragma once

#include <Mary/Element.h>

#define MARY_EVENT_TYPES 10

enum
{
  MARY_EVENT_KEYDOWN,
  MARY_EVENT_KEYUP,
  MARY_EVENT_KEYPRESS,
  MARY_EVENT_MOUSEDOWN,
  MARY_EVENT_MOUSEUP,
  MARY_EVENT_MOUSECLICK,
  MARY_EVENT_MOUSEWHEEL,
  MARY_EVENT_MOUSEMOVE,
  MARY_EVENT_MOUSEIN,
  MARY_EVENT_MOUSEOUT
};

enum
{
  MARY_EVENT_MOUSE_NONE = 0x00,
  MARY_EVENT_MOUSE_LEFT = 0x01,
  MARY_EVENT_MOUSE_MIDDLE = 0x02,
  MARY_EVENT_MOUSE_RIGHT = 0x04
};

enum
{
  MARY_EVENT_KEY_ENTER = 0x10000, // keypresses
  MARY_EVENT_KEY_TAB,
  MARY_EVENT_KEY_UNKNOWN = 0x20000, // non-keypresses
  MARY_EVENT_KEY_DEADKEY,
  MARY_EVENT_KEY_CONTROL_LEFT,
  MARY_EVENT_KEY_CONTROL_RIGHT,
  MARY_EVENT_KEY_SHIFT_LEFT,
  MARY_EVENT_KEY_SHIFT_RIGHT,
  MARY_EVENT_KEY_CAPSLOCK,
  MARY_EVENT_KEY_BACKSPACE,
  MARY_EVENT_KEY_ESCAPE,
  MARY_EVENT_KEY_ARROW_LEFT,
  MARY_EVENT_KEY_ARROW_UP,
  MARY_EVENT_KEY_ARROW_RIGHT,
  MARY_EVENT_KEY_ARROW_DOWN,
  MARY_EVENT_KEY_INSERT,
  MARY_EVENT_KEY_DELETE,
  MARY_EVENT_KEY_HOME,
  MARY_EVENT_KEY_END,
  MARY_EVENT_KEY_PAGE_UP,
  MARY_EVENT_KEY_PAGE_DOWN
};

typedef struct Mary_Event_t Mary_Event_t;
typedef struct Mary_Event_Keydown_t Mary_Event_Keydown_t;
typedef struct Mary_Event_Keyup_t Mary_Event_Keyup_t;
typedef struct Mary_Event_Keypress_t Mary_Event_Keypress_t;
typedef struct Mary_Event_Mousedown_t Mary_Event_Mousedown_t;
typedef struct Mary_Event_Mouseup_t Mary_Event_Mouseup_t;
typedef struct Mary_Event_Mouseclick_t Mary_Event_Mouseclick_t;
typedef struct Mary_Event_Mousewheel_t Mary_Event_Mousewheel_t;
typedef struct Mary_Event_Mousemove_t Mary_Event_Mousemove_t;
typedef struct Mary_Event_Mousein_t Mary_Event_Mousein_t;
typedef struct Mary_Event_Mouseout_t Mary_Event_Mouseout_t;

#define MARY_Event_t\
  uint32_t type;\
  Mary_Element_t *target, *current;\
  uint8_t canceled, skipped, stopped, defaulted

#define MARY_Event_Modifiers_t\
  int8_t buttons, control, shift, alt

#define MARY_Event_Coordinates_t\
  int16_t x, y

struct Mary_Event_t
{
  MARY_Event_t;
};

struct Mary_Event_Keydown_t
{
  MARY_Event_t;
  MARY_Event_Modifiers_t;
  uint32_t key;
};

struct Mary_Event_Keyup_t
{
  MARY_Event_t;
  MARY_Event_Modifiers_t;
  uint32_t key;
};

struct Mary_Event_Keypress_t
{
  MARY_Event_t;
  MARY_Event_Modifiers_t;
  uint32_t key;
};

struct Mary_Event_Mousedown_t
{
  MARY_Event_t;
  MARY_Event_Modifiers_t;
  MARY_Event_Coordinates_t;
  int8_t button;
};

struct Mary_Event_Mouseup_t
{
  MARY_Event_t;
  MARY_Event_Modifiers_t;
  MARY_Event_Coordinates_t;
  int8_t button;
};

struct Mary_Event_Mouseclick_t
{
  MARY_Event_t;
  MARY_Event_Modifiers_t;
  MARY_Event_Coordinates_t;
  int8_t button;
};

struct Mary_Event_Mousewheel_t
{
  MARY_Event_t;
  MARY_Event_Modifiers_t;
  MARY_Event_Coordinates_t;
  int8_t delta;
};

struct Mary_Event_Mousemove_t
{
  MARY_Event_t;
  MARY_Event_Modifiers_t;
  MARY_Event_Coordinates_t;
};

struct Mary_Event_Mousein_t
{
  MARY_Event_t;
  MARY_Event_Modifiers_t;
  MARY_Event_Coordinates_t;
  Mary_Element_t *from;
};

struct Mary_Event_Mouseout_t
{
  MARY_Event_t;
  MARY_Event_Modifiers_t;
  MARY_Event_Coordinates_t;
  Mary_Element_t *to;
};

typedef void (*Mary_Event_f)(Mary_Event_t *);
typedef struct Mary_Event_Listener_t Mary_Event_Listener_t;

struct Mary_Event_Listener_t
{
  Mary_Event_f function;
  uint8_t capture, once;
};

void *Mary_Event_Create(Mary_Window_t *window, int enum_mary_event);
void Mary_Event_Create_Listeners(Mary_Vector_t *listeners);
void Mary_Event_Destroy_Listeners(Mary_Vector_t *listeners);
void Mary_Event_Dispatch(void *mary_event, void *mary_element_target);
void Mary_Event_Cancel(void *mary_event); // cancels event and any further listeners completely
void Mary_Event_Skip_Current(void *mary_event); // skips events on the same element, but only after itself
void Mary_Event_Stop_Propagation(void *mary_event); // stops capture phase and bubbling phase
void Mary_Event_Disable_Default(void *mary_event); // disables any default behavior, if present
void Mary_Event_Add_Listener(void *mary_element, int enum_mary_event, Mary_Event_f func, char capture, char once);
void Mary_Event_Del_Listener(void *mary_element, int enum_mary_event, Mary_Event_f func, char capture, char once);
void Mary_Event_Empty_Listeners(void *mary_element, int enum_mary_event);

#define MARY_Event(PTR) ((Mary_Event_t *)(PTR))
#define MARY_Event_Keypress(PTR) ((Mary_Event_Keypress_t *)(PTR))
#define MARY_Event_Mousedown(PTR) ((Mary_Event_Mousedown_t *)(PTR))
#define MARY_Event_Mouseclick(PTR) ((Mary_Event_Mouseclick_t *)(PTR))

#define MARY_Event_Declare_Keypress(PTR) Mary_Event_Keypress_t *keypress = (Mary_Event_Keypress_t *)(PTR)

#define MARY_Event_Keydown_Is_Keypress(EVENT_PTR) (EVENT_PTR->key < 0x20000)
