#include <stdio.h>
#include <Mary/Event.h>
#include <Mary/Window.h>
#include <Mary/Div.h>
#include <Mary/Text.h>

MARY_Primitives;

static void Default_On_Keydown(Mary_Event_Keydown_t *event);
static void Default_On_Keyup(Mary_Event_Keyup_t *event);
static void Default_On_Keypress(Mary_Event_Keypress_t *event);
static void Default_On_Mousedown(Mary_Event_Mousedown_t *event);
static void Default_On_Mouseup(Mary_Event_Mouseup_t *event);
static void Default_On_Mouseclick(Mary_Event_Mouseclick_t *event);
static void Default_On_Mousewheel(Mary_Event_Mousewheel_t *event);
static void Default_On_Mousemove(Mary_Event_Mousewheel_t *event);
static void Default_On_Mousein(Mary_Event_Mousein_t *event);
static void Default_On_Mouseout(Mary_Event_Mouseout_t *event);

static const size_t BYTES[MARY_EVENT_TYPES] =
{
  sizeof(Mary_Event_Keydown_t),
  sizeof(Mary_Event_Keyup_t),
  sizeof(Mary_Event_Keypress_t),
  sizeof(Mary_Event_Mousedown_t),
  sizeof(Mary_Event_Mouseup_t),
  sizeof(Mary_Event_Mouseclick_t),
  sizeof(Mary_Event_Mousewheel_t),
  sizeof(Mary_Event_Mousemove_t),
  sizeof(Mary_Event_Mousein_t),
  sizeof(Mary_Event_Mouseout_t)
};

#pragma warning (disable: 4028)
// I'm not sure I need a default for each message. possibly, we should check in window if
// there is anyone listening before we dispatch an event.
static const Mary_Event_f DEFAULT_LISTENERS[MARY_EVENT_TYPES] =
{
  Default_On_Keydown,
  Default_On_Keyup,
  Default_On_Keypress,
  Default_On_Mousedown,
  Default_On_Mouseup,
  Default_On_Mouseclick,
  Default_On_Mousewheel,
  Default_On_Mousemove,
  Default_On_Mousein,
  Default_On_Mouseout
};
#pragma warning (default: 4028)

static void Default_On_Keydown(Mary_Event_Keydown_t *event)
{
  Mary_Event_Stop_Propagation(event);
}

static void Default_On_Keyup(Mary_Event_Keyup_t *event)
{
  Mary_Event_Stop_Propagation(event);
}

static void Default_On_Keypress(Mary_Event_Keypress_t *event)
{
  Mary_Event_Stop_Propagation(event);
}

static void Default_On_Mousedown(Mary_Event_Mousedown_t *event)
{
  // maybe want to set active element. but might want to wait until full click?
}

static void Default_On_Mouseup(Mary_Event_Mouseup_t *event)
{

}

static void Default_On_Mouseclick(Mary_Event_Mouseclick_t *event)
{
  // set as active element on window I think.
}

static void Default_On_Mousewheel(Mary_Event_Mousewheel_t *event)
{
  Mary_Element_t *elem = MARY_Element(event->current);
  if (elem->overflow == MARY_ELEMENT_OVERFLOW_Y || elem->overflow == MARY_ELEMENT_OVERFLOW_XY)
  {
    if ((event->delta < 0 && elem->scroll_y != 0) || (event->delta > 0 && elem->scroll_y != elem->scroll_y_max))
    {
      Mary_Element_Scroll_Y_By(elem, 64.0f * event->delta); // we should be calc height to scroll better
      Mary_Window_Flag_Last_Scroll_Elem(elem->window, elem);
      Mary_Event_Disable_Default(event);
    }
  }
  else if (elem->overflow == MARY_ELEMENT_OVERFLOW_X)
  {
    if ((event->delta < 0 && elem->scroll_x != 0) || (event->delta > 0 && elem->scroll_x != elem->scroll_x_max))
    {
      Mary_Element_Scroll_X_By(elem, 64.0f * event->delta); // we should be calc height to scroll better
      Mary_Window_Flag_Last_Scroll_Elem(elem->window, elem);
      Mary_Event_Disable_Default(event);
    }
  }
}

static void Default_On_Mousemove(Mary_Event_Mousewheel_t *event)
{
  // we could implement a css type hover thing in here.
  // or in the mouse enter and leave events.
}

static void Default_On_Mousein(Mary_Event_Mousein_t *event)
{
  Mary_Event_Stop_Propagation(event);
}

static void Default_On_Mouseout(Mary_Event_Mouseout_t *event)
{
  Mary_Event_Stop_Propagation(event);
}

void *Mary_Event_Create(Mary_Window_t *window, int enum_mary_event)
{
  Mary_Event_t *event = Mary_Pool_Alloc(window->event_pool, BYTES[enum_mary_event]);
  event->type = enum_mary_event;
  event->target = 0;
  event->current = 0;
  event->canceled = 0;
  event->skipped = 0;
  event->stopped = 0;
  event->defaulted = 1;
  MARY_Vector_Push(&window->messages, Mary_Event_t *, event);
  return event;
}

void Mary_Event_Create_Listeners(Mary_Vector_t *listeners)
{
  Mary_Vector_Create(listeners, MARY_Allocator_Heap, sizeof(Mary_Vector_t), MARY_EVENT_TYPES);
  listeners->units = MARY_EVENT_TYPES;
  MARY_Vector_Each(listeners, Mary_Vector_t)
  {
    Mary_Vector_Create(it.ptr, MARY_Allocator_Heap, sizeof(Mary_Event_Listener_t), 8);
  }
}

void Mary_Event_Destroy_Listeners(Mary_Vector_t *listeners)
{
  MARY_Vector_Each(listeners, Mary_Vector_t)
  {
    Mary_Vector_Destroy(it.ptr);
  }
  Mary_Vector_Destroy(listeners);
}

void Mary_Event_Dispatch(void *mary_event, void *mary_element_target)
{
  Mary_Event_t *event = MARY_Event(mary_event);
  Mary_Element_t *target = MARY_Element(mary_element_target);
  event->target = target; // this doesn't change.

  ////// Capture Phase
  {
    MARY_Vector_Stack(parents, Mary_Element_t *, 256);
    for (Mary_Element_t *elem = target->parent; elem != 0; elem = elem->parent)
    {
      MARY_Vector_Push(&parents, Mary_Element_t *, elem);
    }

    MARY_Vector_Each_Reverse(&parents, Mary_Element_t *)
    {
      Mary_Element_t *elem = it.val;
      Mary_Vector_t *listeners = MARY_Vector_Point(&elem->listeners, event->type);
      event->current = elem;
      MARY_Vector_Each(listeners, Mary_Event_Listener_t)
      {
        Mary_Event_Listener_t *listener = it.ptr;
        if (listener->capture)
        {
          listener->function(event);
          if (listener->once)
          {
            Mary_Vector_t *listeners = MARY_Vector_Point(&elem->listeners, event->type);
            Mary_Vector_Erase(listeners, listener);
          }
          if (event->canceled)
          {
            return;
          }
          if (event->skipped)
          {
            event->skipped = 0;
            break;
          }
        }
      }
      if (event->stopped)
      {
        break;
      }
    }
  }

  ////// Target Phase
  {
    Mary_Vector_t *listeners = MARY_Vector_Point(&target->listeners, event->type);
    event->current = target;
    MARY_Vector_Each(listeners, Mary_Event_Listener_t)
    {
      Mary_Event_Listener_t *listener = it.ptr;
      listener->function(event);
      if (listener->once)
      {
        Mary_Vector_Erase(listeners, listener);
      }
      if (event->canceled)
      {
        return;
      }
      if (event->skipped)
      {
        event->skipped = 0;
        break;
      }
    }
    if (event->defaulted)
    {
      DEFAULT_LISTENERS[event->type](event);
    }
    if (event->stopped)
    {
      return;
    }
  }

  ////// Bubble Phase
  {
    for (Mary_Element_t *elem = target->parent; elem != 0; elem = elem->parent)
    {
      Mary_Vector_t *listeners = MARY_Vector_Point(&elem->listeners, event->type);
      event->current = elem;
      MARY_Vector_Each(listeners, Mary_Event_Listener_t)
      {
        Mary_Event_Listener_t *listener = it.ptr;
        if (!listener->capture)
        {
          listener->function(event);
          if (listener->once)
          {
            Mary_Vector_t *listeners = MARY_Vector_Point(&elem->listeners, event->type);
            Mary_Vector_Erase(listeners, listener);
          }
          if (event->canceled)
          {
            return;
          }
          if (event->skipped)
          {
            event->skipped = 0;
            break;
          }
        }
      }
      if (event->defaulted)
      {
        DEFAULT_LISTENERS[event->type](event);
      }
      if (event->stopped)
      {
        return;
      }
    }
  }
}

void Mary_Event_Cancel(void *mary_event)
{
  MARY_Event(mary_event)->canceled = 1;
}

void Mary_Event_Skip_Current(void *mary_event)
{
  MARY_Event(mary_event)->skipped = 1;
}

void Mary_Event_Stop_Propagation(void *mary_event)
{
  MARY_Event(mary_event)->stopped = 1;
}

void Mary_Event_Disable_Default(void *mary_event)
{
  MARY_Event(mary_event)->defaulted = 0;
}

void Mary_Event_Add_Listener(void *mary_element,
                             int enum_mary_event,
                             Mary_Event_f func,
                             char capture,
                             char once)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  Mary_Vector_t *listeners = MARY_Vector_Point(&elem->listeners, enum_mary_event);
  Mary_Event_Listener_t listener = { func, capture, once };
  Mary_Vector_Erase(listeners, &listener);
  MARY_Vector_Push(listeners, Mary_Event_Listener_t, listener);
}

void Mary_Event_Del_Listener(void *mary_element,
                             int enum_mary_event,
                             Mary_Event_f func,
                             char capture,
                             char once)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  Mary_Vector_t *listeners = MARY_Vector_Point(&elem->listeners, enum_mary_event);
  Mary_Event_Listener_t listener = { func, capture, once };
  Mary_Vector_Erase(listeners, &listener);
}

void Mary_Event_Empty_Listeners(void *mary_element, int enum_mary_event)
{
  Mary_Element_t *elem = MARY_Element(mary_element);
  Mary_Vector_t *listeners = MARY_Vector_Point(&elem->listeners, enum_mary_event);
  MARY_Vector_Empty(listeners);
}
