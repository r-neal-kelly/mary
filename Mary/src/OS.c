#include <stdlib.h>
#include <stdio.h>
#include <Mary/Utils.h>
#include <Mary/Hashmap.h>
#include <Mary/File.h>
#include <Mary/OS.h>
#include <Mary/OpenGL.h>
#include <Mary/Window.h>

MARY_Primitives;

static void Mary_OpenGL_Start();
static void Mary_OpenGL_Finish();

#if defined(_WIN32)
#pragma comment (lib, "opengl32.lib")
//#pragma comment (lib, "glu32.lib")

#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_TYPE_RGBA_ARB 0x202B
#define WGL_SAMPLE_BUFFERS_ARB 0x2041
#define WGL_SAMPLES_ARB 0x2042
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x0001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x0002

static HWND g_hwnd = 0;
static HDC g_hdc = 0;
static HGLRC g_hglrc = 0;
static int g_pixel_format = 0;

static const uint16_t g_class_name[] = u"Mary_Win32";
static Mary_Hashmap_t windows;

static u8 g_keyboard_state[265];
static u16 g_keyboard_codes[4];

static HDC dib_context;
static BITMAPINFO dib_bmi;

static HFONT g_font_handle = 0;
static u16 g_font_face[32];
static u16 g_font_real_px = 0;
static Mary_OS_Font_Info_t g_font_info;

static BOOL(WINAPI *wglChoosePixelFormatARB)(HDC hdc, const int *attribs_i, const FLOAT *attribs_f, UINT max_count, int *out_format, UINT *out_count);
static HGLRC(WINAPI *wglCreateContextAttribsARB)(HDC hdc, HGLRC share_context, const int *attribs_i);
static int64_t WINAPI Win32_Wnd_Proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

static void Mary_OS_Start_OpenGL()
{
  WNDCLASSEX wndclass;
  memset(&wndclass, 0, sizeof(wndclass));
  wndclass.cbSize = sizeof(wndclass);
  wndclass.lpfnWndProc = DefWindowProc;
  wndclass.lpszClassName = u"Mary_OpenGL";
  wndclass.style = CS_OWNDC;
  RegisterClassEx(&wndclass);

  g_hwnd = CreateWindowEx(0,
                          u"Mary_OpenGL",
                          0,
                          0, 0,
                          0, 0,
                          0, 0, 0, 0, 0);
  g_hdc = GetDC(g_hwnd);

  PIXELFORMATDESCRIPTOR pfd;
  memset(&pfd, 0, sizeof(pfd));
  SetPixelFormat(g_hdc, ChoosePixelFormat(g_hdc, &pfd), &pfd);

  g_hglrc = wglCreateContext(g_hdc);
  wglMakeCurrent(g_hdc, g_hglrc);

  HMODULE opengl_dll = LoadLibrary(u"opengl32.dll");

  // Buffer Objects
  glBindBuffer = (void *)wglGetProcAddress("glBindBuffer");
  glBufferData = (void *)wglGetProcAddress("glBufferData");
  glDeleteBuffers = (void *)wglGetProcAddress("glDeleteBuffers");
  glDisableVertexAttribArray = (void *)wglGetProcAddress("glDisableVertexAttribArray");
  glDrawArrays = (void *)GetProcAddress(opengl_dll, "glDrawArrays");
  glDrawElements = (void *)GetProcAddress(opengl_dll, "glDrawElements");
  glEnableVertexAttribArray = (void *)wglGetProcAddress("glEnableVertexAttribArray");
  glGenBuffers = (void *)wglGetProcAddress("glGenBuffers");
  glVertexAttribIPointer = (void *)wglGetProcAddress("glVertexAttribIPointer");
  glVertexAttribPointer = (void *)wglGetProcAddress("glVertexAttribPointer");

  // Rendering
  glClear = (void *)GetProcAddress(opengl_dll, "glClear");
  glClearColor = (void *)GetProcAddress(opengl_dll, "glClearColor");

  // Shaders
  glAttachShader = (void *)wglGetProcAddress("glAttachShader");
  glCompileShader = (void *)wglGetProcAddress("glCompileShader");
  glCreateProgram = (void *)wglGetProcAddress("glCreateProgram");
  glCreateShader = (void *)wglGetProcAddress("glCreateShader");
  glDeleteProgram = (void *)wglGetProcAddress("glDeleteProgram");
  glDeleteShader = (void *)wglGetProcAddress("glDeleteShader");
  glDetachShader = (void *)wglGetProcAddress("glDetachShader");
  glGetShaderiv = (void *)wglGetProcAddress("glGetShaderiv");
  glGetShaderInfoLog = (void *)wglGetProcAddress("glGetShaderInfoLog");
  glGetUniformLocation = (void *)wglGetProcAddress("glGetUniformLocation");
  glLinkProgram = (void *)wglGetProcAddress("glLinkProgram");
  glShaderSource = (void *)wglGetProcAddress("glShaderSource");
  glUniform1i = (void *)wglGetProcAddress("glUniform1i");
  glUniform4f = (void *)wglGetProcAddress("glUniform4f");
  glUniformMatrix4fv = (void *)wglGetProcAddress("glUniformMatrix4fv");
  glUseProgram = (void *)wglGetProcAddress("glUseProgram");
  glValidateProgram = (void *)wglGetProcAddress("glValidateProgram");

  // State Management
  glBlendFunc = (void *)GetProcAddress(opengl_dll, "glBlendFunc");
  glEnable = (void *)GetProcAddress(opengl_dll, "glEnable");
  glGetIntegerv = (void *)GetProcAddress(opengl_dll, "glGetIntegerv");
  glGetError = (void *)GetProcAddress(opengl_dll, "glGetError");
  glPixelStorei = (void *)GetProcAddress(opengl_dll, "glPixelStorei");
  glPolygonMode = (void *)GetProcAddress(opengl_dll, "glPolygonMode");
  glScissor = (void *)GetProcAddress(opengl_dll, "glScissor");
  glViewport = (void *)GetProcAddress(opengl_dll, "glViewport");

  // Textures
  glActiveTexture = (void *)wglGetProcAddress("glActiveTexture");
  glBindTexture = (void *)GetProcAddress(opengl_dll, "glBindTexture");
  glDeleteTextures = (void *)GetProcAddress(opengl_dll, "glDeleteTextures");
  glGenTextures = (void *)GetProcAddress(opengl_dll, "glGenTextures");
  glTexImage2D = (void *)GetProcAddress(opengl_dll, "glTexImage2D");
  glTexParameteri = (void *)GetProcAddress(opengl_dll, "glTexParameteri");

  // Utility
  glGetString = (void *)GetProcAddress(opengl_dll, "glGetString");

  // Vertex Array Objects
  glBindVertexArray = (void *)wglGetProcAddress("glBindVertexArray");
  glDeleteVertexArrays = (void *)wglGetProcAddress("glDeleteVertexArrays");
  glGenVertexArrays = (void *)wglGetProcAddress("glGenVertexArrays");

  // Windows Extensions
  wglChoosePixelFormatARB = (void *)wglGetProcAddress("wglChoosePixelFormatARB");
  wglCreateContextAttribsARB = (void *)wglGetProcAddress("wglCreateContextAttribsARB");

  FreeLibrary(opengl_dll);

  const int pf_attribs[] =
  {
    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_COLOR_BITS_ARB, 32,
    WGL_DEPTH_BITS_ARB, 24,
    WGL_STENCIL_BITS_ARB, 8,
    //WGL_SAMPLE_BUFFERS_ARB, 1, // we may want to have a separate context just for anti-aliasing?
    //WGL_SAMPLES_ARB, 4,
    0
  };
  unsigned int num_formats = 0;
  wglChoosePixelFormatARB(g_hdc, pf_attribs, 0, 1, &g_pixel_format, &num_formats);

  wglMakeCurrent(g_hdc, 0);
  wglDeleteContext(g_hglrc);
  ReleaseDC(g_hwnd, g_hdc);
  DestroyWindow(g_hwnd);

  g_hwnd = CreateWindowEx(0,
                          u"Mary_OpenGL",
                          0,
                          0, 0,
                          0, 0,
                          0, 0, 0, 0, 0);
  g_hdc = GetDC(g_hwnd);

  memset(&pfd, 0, sizeof(pfd));
  SetPixelFormat(g_hdc, g_pixel_format, &pfd);

  const int context_attribs[] =
  {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
    WGL_CONTEXT_MINOR_VERSION_ARB, 3,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    0
  };
  g_hglrc = wglCreateContextAttribsARB(g_hdc, 0, context_attribs);
  wglMakeCurrent(g_hdc, g_hglrc);

  puts((char *)glGetString(GL_VERSION)); // temp
}

static void Mary_OS_Start_Window()
{
  WNDCLASSEX win32_class;
  memset(&win32_class, 0, sizeof(win32_class));
  win32_class.cbSize = sizeof(win32_class);
  win32_class.lpfnWndProc = Win32_Wnd_Proc;
  win32_class.hCursor = LoadCursor(0, IDC_ARROW);
  win32_class.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
  win32_class.lpszClassName = g_class_name;
  win32_class.style = CS_OWNDC;
  RegisterClassEx(&win32_class);
  Mary_Hashmap_Create(&windows, sizeof(HWND), sizeof(Mary_Window_t *));
}

static void Mary_OS_Start_Text()
{
  dib_context = CreateCompatibleDC(g_hdc);
  SetBkColor(dib_context, RGB(0, 0, 0));
  SetTextColor(dib_context, RGB(255, 255, 255));
  memset(&dib_bmi, 0, sizeof(dib_bmi));
  dib_bmi.bmiHeader.biSize = sizeof(dib_bmi.bmiHeader);
  dib_bmi.bmiHeader.biWidth = 0;
  dib_bmi.bmiHeader.biHeight = 0;
  dib_bmi.bmiHeader.biPlanes = 1;
  dib_bmi.bmiHeader.biBitCount = 32;
  dib_bmi.bmiHeader.biCompression = BI_RGB;
  dib_bmi.bmiHeader.biSizeImage = 0;
}

static void Mary_OS_Start_Font()
{
  g_font_handle = (void *)GetStockObject(SYSTEM_FONT);

  Mary_Zero(g_font_face, sizeof(g_font_face));
  LOGFONT log_font; GetObject(g_font_handle, sizeof(LOGFONT), &log_font);
  memcpy(g_font_face, log_font.lfFaceName, sizeof(log_font.lfFaceName));

  RECT rect = { 0, 0, 0, 0 };
  DrawTextEx(dib_context, u" ", 1, &rect, DT_CALCRECT, 0);
  g_font_real_px = (u16)rect.bottom;

  Mary_Zero(&g_font_info, sizeof(g_font_info));
}

void Mary_OS_Start()
{
  Mary_OS_Start_OpenGL();
  Mary_OS_Start_Window();
  Mary_OS_Start_Text();
  Mary_OS_Start_Font();
}

void Mary_OS_Finish()
{
  ////// Font //////
  DeleteObject(g_font_handle);

  ////// Text //////
  DeleteDC(dib_context);

  ////// Window //////
  Mary_Hashmap_Destroy(&windows);

  ////// OpenGL //////
  wglMakeCurrent(g_hdc, 0);
  wglDeleteContext(g_hglrc);
  ReleaseDC(g_hwnd, g_hdc);
  DestroyWindow(g_hwnd);
}

uint64_t Mary_OS_Thread_Current_ID()
{
  return GetCurrentThreadId();
}

void Mary_OS_Window_Create(Mary_Window_t *window)
{
  HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,
                             g_class_name,
                             L"Praise Yahweh!", // these need to be set to user values.
                             WS_OVERLAPPEDWINDOW,
                             100, 100, //
                             800, 600, //
                             0, 0, 0, 0);
  window->os.handle = hwnd;
  window->os.context = GetDC(hwnd);
  PIXELFORMATDESCRIPTOR pfd; memset(&pfd, 0, sizeof(pfd));
  SetPixelFormat(window->os.context, g_pixel_format, &pfd);
  Mary_Hashmap_Assign(&windows, &hwnd, &window);
}

void Mary_OS_Window_Destroy(Mary_Window_t *window)
{
  Mary_Hashmap_Erase(&windows, &window->os.handle);
  DeleteObject(window->os.brush);
  ReleaseDC(window->os.handle, window->os.context);
  DestroyWindow(window->os.handle);
}

static int64_t WINAPI Win32_Wnd_Proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
  #define GET_HI(PARAM) (PARAM >> 16)
  #define GET_LO(PARAM) (PARAM & 0x0000FFFF)
  #define IS_DOWN(VIRTUAL_KEY) (GetKeyState(VIRTUAL_KEY) < 0 ? 1 : 0)
  #define IS_TOGGLED(VIRTUAL_KEY) (GetKeyState(VIRTUAL_KEY) > 0 ? 1 : 0)

  Mary_Window_t *window = 0; Mary_Hashmap_At(&windows, &hwnd, &window);
  if (!window)
  {
    return DefWindowProc(hwnd, msg, wp, lp);
  }
  else if (msg == WM_CLOSE)
  {
    Mary_Window_Close(window);
    return 0;
  }
  else if (msg == WM_SIZE)
  {
    Mary_OS_Window_Measure(window);
    Mary_Window_Refresh(window);
    return 0;
  }
  else if (msg == WM_MOVE)
  {
    Mary_Window_Refresh(window);
    return 0;
  }
  else if (msg == WM_ERASEBKGND)
  {
    PAINTSTRUCT ps;
    BeginPaint(hwnd, &ps);
    FillRect(window->os.context, &ps.rcPaint, window->os.brush); // I think instead of brush, we could give it a RGB macro. but this is faster.
    EndPaint(hwnd, &ps);
    return 1;
  }
  else if (msg == WM_KEYDOWN)
  {
    Mary_Event_Keydown_t *keydown = Mary_Event_Create(window, MARY_EVENT_KEYDOWN);
    switch (wp)
    {
      case VK_RETURN:
        keydown->key = MARY_EVENT_KEY_ENTER; break;
      case VK_BACK:
        keydown->key = MARY_EVENT_KEY_BACKSPACE; break;
      case VK_TAB:
        keydown->key = MARY_EVENT_KEY_TAB; break;
      case VK_LCONTROL:
        keydown->key = MARY_EVENT_KEY_CONTROL_LEFT; break;
      case VK_RCONTROL:
        keydown->key = MARY_EVENT_KEY_CONTROL_RIGHT; break;
      case VK_LSHIFT:
        keydown->key = MARY_EVENT_KEY_SHIFT_LEFT; break;
      case VK_RSHIFT:
        keydown->key = MARY_EVENT_KEY_SHIFT_RIGHT; break;
      case VK_CAPITAL:
        keydown->key = MARY_EVENT_KEY_CAPSLOCK; break;
      case VK_ESCAPE:
        keydown->key = MARY_EVENT_KEY_ESCAPE; break;
      case VK_LEFT:
        keydown->key = MARY_EVENT_KEY_ARROW_LEFT; break;
      case VK_UP:
        keydown->key = MARY_EVENT_KEY_ARROW_UP; break;
      case VK_RIGHT:
        keydown->key = MARY_EVENT_KEY_ARROW_RIGHT; break;
      case VK_DOWN:
        keydown->key = MARY_EVENT_KEY_ARROW_DOWN; break;
      case VK_INSERT:
        keydown->key = MARY_EVENT_KEY_INSERT; break;
      case VK_DELETE:
        keydown->key = MARY_EVENT_KEY_DELETE; break;
      case VK_HOME:
        keydown->key = MARY_EVENT_KEY_HOME; break;
      case VK_END:
        keydown->key = MARY_EVENT_KEY_END; break;
      case VK_PRIOR:
        keydown->key = MARY_EVENT_KEY_PAGE_UP; break;
      case VK_NEXT:
        keydown->key = MARY_EVENT_KEY_PAGE_DOWN; break;
      default:
      {
        int units = ToUnicode((u32)wp, (u16)(lp >> 16), g_keyboard_state, g_keyboard_codes, 4, 0);
        switch (units)
        {
          case -1:
            keydown->key = MARY_EVENT_KEY_DEADKEY; break;
          case 0:
            keydown->key = MARY_EVENT_KEY_UNKNOWN; break;
          default:
            keydown->key = keydown->key = g_keyboard_codes[0];
        }
      }
    }
    keydown->buttons = MARY_EVENT_MOUSE_NONE;
    if (IS_DOWN(VK_LBUTTON)) keydown->buttons |= MARY_EVENT_MOUSE_LEFT;
    if (IS_DOWN(VK_MBUTTON)) keydown->buttons |= MARY_EVENT_MOUSE_MIDDLE;
    if (IS_DOWN(VK_RBUTTON)) keydown->buttons |= MARY_EVENT_MOUSE_RIGHT;
    keydown->control = IS_DOWN(VK_CONTROL);
    keydown->shift = IS_DOWN(VK_SHIFT);
    keydown->alt = IS_DOWN(VK_MENU);
    return 0;
  }
  else if (msg == WM_KEYUP)
  {
    Mary_Event_Keyup_t *keyup = Mary_Event_Create(window, MARY_EVENT_KEYUP);
    switch (wp)
    {
      case VK_RETURN:
        keyup->key = MARY_EVENT_KEY_ENTER; break;
      case VK_BACK:
        keyup->key = MARY_EVENT_KEY_BACKSPACE; break;
      case VK_TAB:
        keyup->key = MARY_EVENT_KEY_TAB; break;
      case VK_LCONTROL:
        keyup->key = MARY_EVENT_KEY_CONTROL_LEFT; break;
      case VK_RCONTROL:
        keyup->key = MARY_EVENT_KEY_CONTROL_RIGHT; break;
      case VK_LSHIFT:
        keyup->key = MARY_EVENT_KEY_SHIFT_LEFT; break;
      case VK_RSHIFT:
        keyup->key = MARY_EVENT_KEY_SHIFT_RIGHT; break;
      case VK_CAPITAL:
        keyup->key = MARY_EVENT_KEY_CAPSLOCK; break;
      case VK_ESCAPE:
        keyup->key = MARY_EVENT_KEY_ESCAPE; break;
      case VK_LEFT:
        keyup->key = MARY_EVENT_KEY_ARROW_LEFT; break;
      case VK_UP:
        keyup->key = MARY_EVENT_KEY_ARROW_UP; break;
      case VK_RIGHT:
        keyup->key = MARY_EVENT_KEY_ARROW_RIGHT; break;
      case VK_DOWN:
        keyup->key = MARY_EVENT_KEY_ARROW_DOWN; break;
      case VK_INSERT:
        keyup->key = MARY_EVENT_KEY_INSERT; break;
      case VK_DELETE:
        keyup->key = MARY_EVENT_KEY_DELETE; break;
      case VK_HOME:
        keyup->key = MARY_EVENT_KEY_HOME; break;
      case VK_END:
        keyup->key = MARY_EVENT_KEY_END; break;
      case VK_PRIOR:
        keyup->key = MARY_EVENT_KEY_PAGE_UP; break;
      case VK_NEXT:
        keyup->key = MARY_EVENT_KEY_PAGE_DOWN; break;
      default:
      {
        int units = ToUnicode((u32)wp, (u16)(lp >> 16), g_keyboard_state, g_keyboard_codes, 4, 0);
        switch (units)
        {
          case -1:
            keyup->key = MARY_EVENT_KEY_DEADKEY; break;
          case 0:
            keyup->key = MARY_EVENT_KEY_UNKNOWN; break;
          default:
            keyup->key = keyup->key = g_keyboard_codes[0];
        }
      }
    }
    keyup->buttons = MARY_EVENT_MOUSE_NONE;
    if (IS_DOWN(VK_LBUTTON)) keyup->buttons |= MARY_EVENT_MOUSE_LEFT;
    if (IS_DOWN(VK_MBUTTON)) keyup->buttons |= MARY_EVENT_MOUSE_MIDDLE;
    if (IS_DOWN(VK_RBUTTON)) keyup->buttons |= MARY_EVENT_MOUSE_RIGHT;
    keyup->control = IS_DOWN(VK_CONTROL);
    keyup->shift = IS_DOWN(VK_SHIFT);
    keyup->alt = IS_DOWN(VK_MENU);
    return 0;
  }
  else if (msg == WM_LBUTTONDOWN)
  {
    Mary_Event_Mousedown_t *event = Mary_Event_Create(window, MARY_EVENT_MOUSEDOWN);
    event->x = (short)GET_LO(lp);
    event->y = (short)GET_HI(lp);
    event->button = MARY_EVENT_MOUSE_LEFT;
    event->buttons = MARY_EVENT_MOUSE_LEFT;
    if (wp & MK_MBUTTON) event->buttons |= MARY_EVENT_MOUSE_MIDDLE;
    if (wp & MK_RBUTTON) event->buttons |= MARY_EVENT_MOUSE_RIGHT;
    event->control = wp & MK_CONTROL ? 1 : 0;
    event->shift = wp & MK_SHIFT ? 1 : 0;
    event->alt = IS_DOWN(VK_MENU);
    return 0;
  }
  else if (msg == WM_MBUTTONDOWN)
  {
    Mary_Event_Mousedown_t *event = Mary_Event_Create(window, MARY_EVENT_MOUSEDOWN);
    event->x = (short)GET_LO(lp);
    event->y = (short)GET_HI(lp);
    event->button = MARY_EVENT_MOUSE_MIDDLE;
    event->buttons = MARY_EVENT_MOUSE_MIDDLE;
    if (wp & MK_LBUTTON) event->buttons |= MARY_EVENT_MOUSE_LEFT;
    if (wp & MK_RBUTTON) event->buttons |= MARY_EVENT_MOUSE_RIGHT;
    event->control = wp & MK_CONTROL ? 1 : 0;
    event->shift = wp & MK_SHIFT ? 1 : 0;
    event->alt = IS_DOWN(VK_MENU);
    return 0;
  }
  else if (msg == WM_RBUTTONDOWN)
  {
    Mary_Event_Mousedown_t *event = Mary_Event_Create(window, MARY_EVENT_MOUSEDOWN);
    event->x = (short)GET_LO(lp);
    event->y = (short)GET_HI(lp);
    event->button = MARY_EVENT_MOUSE_RIGHT;
    event->buttons = MARY_EVENT_MOUSE_RIGHT;
    if (wp & MK_LBUTTON) event->buttons |= MARY_EVENT_MOUSE_LEFT;
    if (wp & MK_MBUTTON) event->buttons |= MARY_EVENT_MOUSE_MIDDLE;
    event->control = wp & MK_CONTROL ? 1 : 0;
    event->shift = wp & MK_SHIFT ? 1 : 0;
    event->alt = IS_DOWN(VK_MENU);
    return 0;
  }
  else if (msg == WM_LBUTTONUP)
  {
    Mary_Event_Mouseup_t *event = Mary_Event_Create(window, MARY_EVENT_MOUSEUP);
    event->x = (short)GET_LO(lp);
    event->y = (short)GET_HI(lp);
    event->button = MARY_EVENT_MOUSE_LEFT;
    event->buttons = MARY_EVENT_MOUSE_LEFT;
    if (wp & MK_MBUTTON) event->buttons |= MARY_EVENT_MOUSE_MIDDLE;
    if (wp & MK_RBUTTON) event->buttons |= MARY_EVENT_MOUSE_RIGHT;
    event->control = wp & MK_CONTROL ? 1 : 0;
    event->shift = wp & MK_SHIFT ? 1 : 0;
    event->alt = IS_DOWN(VK_MENU);
    return 0;
  }
  else if (msg == WM_MBUTTONUP)
  {
    Mary_Event_Mouseup_t *event = Mary_Event_Create(window, MARY_EVENT_MOUSEUP);
    event->x = (short)GET_LO(lp);
    event->y = (short)GET_HI(lp);
    event->button = MARY_EVENT_MOUSE_MIDDLE;
    event->buttons = MARY_EVENT_MOUSE_MIDDLE;
    if (wp & MK_LBUTTON) event->buttons |= MARY_EVENT_MOUSE_LEFT;
    if (wp & MK_RBUTTON) event->buttons |= MARY_EVENT_MOUSE_RIGHT;
    event->control = wp & MK_CONTROL ? 1 : 0;
    event->shift = wp & MK_SHIFT ? 1 : 0;
    event->alt = IS_DOWN(VK_MENU);
    return 0;
  }
  else if (msg == WM_RBUTTONUP)
  {
    Mary_Event_Mouseup_t *event = Mary_Event_Create(window, MARY_EVENT_MOUSEUP);
    event->x = (short)GET_LO(lp);
    event->y = (short)GET_HI(lp);
    event->button = MARY_EVENT_MOUSE_RIGHT;
    event->buttons = MARY_EVENT_MOUSE_RIGHT;
    if (wp & MK_LBUTTON) event->buttons |= MARY_EVENT_MOUSE_LEFT;
    if (wp & MK_MBUTTON) event->buttons |= MARY_EVENT_MOUSE_MIDDLE;
    event->control = wp & MK_CONTROL ? 1 : 0;
    event->shift = wp & MK_SHIFT ? 1 : 0;
    event->alt = IS_DOWN(VK_MENU);
    return 0;
  }
  else if (msg == WM_MOUSEWHEEL)
  {
    Mary_Event_Mousewheel_t *event = Mary_Event_Create(window, MARY_EVENT_MOUSEWHEEL);
    POINT point = { (long)GET_LO(lp), (long)GET_HI(lp) };
    ScreenToClient(hwnd, &point);
    event->x = (short)point.x;
    event->y = (short)point.y;
    event->delta = -((short)GET_HI(wp) / 120); // pos for down, neg for up, return factor
    event->buttons = MARY_EVENT_MOUSE_NONE;
    if (wp & MK_LBUTTON) event->buttons |= MARY_EVENT_MOUSE_LEFT;
    if (wp & MK_MBUTTON) event->buttons |= MARY_EVENT_MOUSE_MIDDLE;
    if (wp & MK_RBUTTON) event->buttons |= MARY_EVENT_MOUSE_RIGHT;
    event->control = wp & MK_CONTROL ? 1 : 0;
    event->shift = wp & MK_SHIFT ? 1 : 0;
    event->alt = IS_DOWN(VK_MENU);
    return 0;
  }
  else if (msg == WM_MOUSEMOVE)
  {
    Mary_Event_Mousemove_t *event = Mary_Event_Create(window, MARY_EVENT_MOUSEMOVE);
    event->x = (short)GET_LO(lp);
    event->y = (short)GET_HI(lp);
    event->buttons = MARY_EVENT_MOUSE_NONE;
    if (wp & MK_LBUTTON) event->buttons |= MARY_EVENT_MOUSE_LEFT;
    if (wp & MK_MBUTTON) event->buttons |= MARY_EVENT_MOUSE_MIDDLE;
    if (wp & MK_RBUTTON) event->buttons |= MARY_EVENT_MOUSE_RIGHT;
    event->control = wp & MK_CONTROL ? 1 : 0;
    event->shift = wp & MK_SHIFT ? 1 : 0;
    event->alt = IS_DOWN(VK_MENU);
    return 0;
  }
  else
  {
    return DefWindowProc(hwnd, msg, wp, lp);
  }

  #undef GET_HI
  #undef GET_LO
  #undef IS_DOWN
  #undef IS_TOGGLED
}

void Mary_OS_Window_Show(Mary_Window_t *window)
{
  ShowWindow(window->os.handle, SW_NORMAL);
  UpdateWindow(window->os.handle);
}

void Mary_OS_Window_Hide(Mary_Window_t *window)
{
  ShowWindow(window->os.handle, SW_HIDE);
}

void Mary_OS_Window_Measure(Mary_Window_t *window)
{
  RECT rect; GetClientRect(window->os.handle, &rect);

  window->w_margin = (float)rect.right;
  window->h_margin = (float)rect.bottom;
  window->w = window->w_margin - window->margin_l - window->margin_r;
  window->h = window->h_margin - window->margin_t - window->margin_b;
  window->w_pad = window->w - window->pad_l - window->pad_r;
  window->h_pad = window->h - window->pad_t - window->pad_b;

  window->x1 = (float)rect.left + window->margin_l;
  window->x2 = (float)rect.right - window->margin_r;
  window->y1 = (float)rect.top + window->margin_t;
  window->y2 = (float)rect.bottom - window->margin_b;
  window->clip_x1 = window->x1;
  window->clip_x2 = window->x2;
  window->clip_y1 = window->y1;
  window->clip_y2 = window->y2;

  // put the following in its own function, for when we need to actually handle the
  // real size of the os window, to make them fit on screen properly.
  // we don't need to have an outer width on every element, because I think we
  // will only ever need it for the Window_t.
  /*GetWindowRect(window->os.handle, &rect);
  window->outer_w = (uint16_t)(rect.right);
  window->outer_h = (uint16_t)(rect.bottom);*/
}

void Mary_OS_Window_Handle_Messages(Mary_Window_t *window)
{
  MSG msg;
  GetKeyboardState(g_keyboard_state);
  while (PeekMessage(&msg, window->os.handle, 0, 0, PM_REMOVE))
  {
    DispatchMessage(&msg);
  }
}

void Mary_OS_Window_Make_Current(Mary_Window_t *window)
{
  wglMakeCurrent(window->os.context, g_hglrc);
}

void Mary_OS_Window_Swap_Buffers(Mary_Window_t *window)
{
  SwapBuffers(window->os.context);
}

void Mary_OS_Window_Back_Color(Mary_Window_t *window)
{
  DeleteObject(window->os.brush);
  window->os.brush = CreateSolidBrush(RGB(window->back_r * 255, window->back_g * 255, window->back_b * 255));
}

void Mary_OS_Sleep(size_t milliseconds)
{
  Sleep((int)milliseconds);
}

double Mary_OS_Microseconds()
{
  LARGE_INTEGER freq, count;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&count);
  double us = count.QuadPart * 1000000.0 / freq.QuadPart;
  return us;
}

double Mary_OS_Milliseconds()
{
  LARGE_INTEGER freq, count;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&count);
  double us = count.QuadPart * 1000000.0 / freq.QuadPart;
  return us / 1000.0;
}

double Mary_OS_Seconds()
{
  LARGE_INTEGER freq, count;
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&count);
  double us = count.QuadPart * 1000000.0 / freq.QuadPart;
  return us / 1000000.0;
}

void Mary_OS_Directory_Create(Mary_OS_Directory_t *dir, Mary_String_t *path)
{
  u16 path_buffer[MAX_PATH];
  Mary_C_String_p path_buffer_p = { path_buffer, sizeof(path_buffer) };
  MARY_String_Append_Back(path, 16, u"*");
  Mary_C_String_Copy((Mary_C_String_p *)path, MARY_Unit_To_UTF(path->unit),
                     &path_buffer_p, 16, 1); // do we need the null?

  WIN32_FIND_DATA find_data;
  dir->os_data = FindFirstFile(path_buffer, &find_data);
  MARY_Assert(dir->os_data != INVALID_HANDLE_VALUE, 0);
  FindNextFile(dir->os_data, &find_data); // trying to skip . and ..

  Mary_OS_Directory_Item_t *item; Mary_Vector_Create(&dir->items, MARY_Allocator_Heap, sizeof(Mary_OS_Directory_Item_t), 8);
  while (FindNextFile(dir->os_data, &find_data))
  {
    item = MARY_Vector_Point_Push(&dir->items);
    item->name.bytes = Mary_C_String_Count_Bytes(find_data.cFileName, 16, 1);
    item->name.data = malloc(item->name.bytes);
    Mary_Copy(find_data.cFileName, item->name.data, item->name.bytes);
  }
}

void Mary_OS_Directory_Destroy(Mary_OS_Directory_t *dir)
{
  FindClose(dir->os_data);

  MARY_Vector_Each(&dir->items, Mary_OS_Directory_Item_t)
  {
    free(it.ptr->name.data);
  }
}

void *Mary_OS_File_Create(Mary_String_t *path, Mary_Enum_t file_mode)
{
  u16 path_buffer[MAX_PATH]; // includes null, but we may be able to extend it?
  u16 file_modes[6][4] = { u"rb", u"r+b", u"wb", u"w+b", u"ab", u"a+b" };
  Mary_C_String_p path_buffer_p = { path_buffer, sizeof(path_buffer) };
  Mary_C_String_Copy((Mary_C_String_p *)path, MARY_Unit_To_UTF(path->unit),
                     &path_buffer_p, 16, 1); // do we need the null?
  return _wfopen(path_buffer, &file_modes[file_mode][0]);
}

void Mary_OS_Textmap2_Create(Mary_OS_Textmap2_t *textmap, Mary_OS_Textmap2_Info_t *info)
{
  u32 flags = (info->enum_align == MARY_OS_TEXTMAP_ALIGN_RIGHT ? DT_RIGHT : DT_LEFT) | DT_NOCLIP;
  RECT rect = { 0, 0, MARY_GL_MAX_TEXTURE_SIZE, 0 };

  DrawTextEx(dib_context, info->text_data, info->text_units, &rect, flags | DT_CALCRECT, 0);
  int dib_bytes = rect.right * rect.bottom * 4;
  dib_bmi.bmiHeader.biWidth = rect.right;
  dib_bmi.bmiHeader.biHeight = -rect.bottom;
  dib_bmi.bmiHeader.biSizeImage = dib_bytes;
  textmap->bitmap.bytes = dib_bytes;
  textmap->bitmap.width = (short)rect.right;
  textmap->bitmap.height = (short)rect.bottom;
  textmap->bitmap.bit_depth = 32;
  textmap->bitmap.unit = 4;
  textmap->os_data = CreateDIBSection(dib_context, &dib_bmi, DIB_RGB_COLORS, &textmap->bitmap.data, 0, 0);

  HBITMAP old_bitmap = (HBITMAP)SelectObject(dib_context, textmap->os_data);
  DrawTextEx(dib_context, info->text_data, info->text_units, &rect, flags, 0);
  SelectObject(dib_context, old_bitmap);

  if (info->bool_alpha != 0)
  {
    u8 *p = (u8 *)textmap->bitmap.data + 3;
    for (u64 i = 3; i < dib_bytes; i += 4, p += 4)
    {
      *p = 0xFF;
    }
  }
}

void Mary_OS_Textmap2_Destroy(Mary_OS_Textmap2_t *textmap)
{
  DeleteObject(textmap->os_data);
}

void Mary_OS_Font_Change(Mary_OS_Font_Info_t *font_info)
{
  if (memcmp(&g_font_info, font_info, sizeof(g_font_info)) == 0) return;

  LOGFONT log_font;
  Mary_Zero(&log_font, sizeof(log_font));
  log_font.lfHeight = -(font_info->hint_px);
  log_font.lfQuality = CLEARTYPE_QUALITY;
  log_font.lfWeight = font_info->bold > 0 ? FW_SEMIBOLD : FW_NORMAL;
  log_font.lfItalic = font_info->italic > 0 ? 1 : 0;
  log_font.lfUnderline = font_info->underline > 0 ? 1 : 0;
  log_font.lfStrikeOut = font_info->strikeout > 0 ? 1 : 0;
  size_t bytes = Mary_C_String_Count_Bytes(font_info->face, 16, 0);
  memcpy(log_font.lfFaceName, font_info->face, bytes);

  DeleteObject(g_font_handle);
  g_font_handle = CreateFontIndirect(&log_font);
  SelectObject(dib_context, g_font_handle);

  GetObject(g_font_handle, sizeof(LOGFONT), &log_font);
  memcpy(g_font_face, log_font.lfFaceName, sizeof(log_font.lfFaceName));

  RECT rect = { 0, 0, 0, 0 };
  DrawTextEx(dib_context, u" ", 1, &rect, DT_CALCRECT, 0);
  g_font_real_px = (u16)rect.bottom;

  memcpy(&g_font_info, font_info, sizeof(g_font_info));
}

uint16_t *Mary_OS_Font_Get_Face()
{
  return g_font_face;
}

uint16_t Mary_OS_Font_Get_Real_Px()
{
  return g_font_real_px;
}

// everything below here is to be deleted once we get the bolts out of it.///////////////////////////////////////////////////////


void Mary_OS_Textmap_Create(Mary_OS_Textmap_t *textmap, uint16_t *text_data, int text_units)
{
  Mary_Vector_t text; Mary_Vector_Create(&text, MARY_Allocator_Heap, sizeof(u16), text_units);
  Mary_Vector_t lines; Mary_Vector_Create(&lines, MARY_Allocator_Heap, sizeof(Mary_OS_Textmap_Line_t), 32);

  // we strip \n from text, and measure how many lines, words, and words per line there are
  Mary_OS_Textmap_Line_t line; u64 total_words = 0;
  line.words.units = 0, line.w = 0, line.h = 0, line.user_w = 0, line.user_h = 0;
  MARY_Range(text_data, u16, 0, text_units)
  {
    if (it.val == '\0')
    {
      ++line.words.units, ++total_words;
      MARY_Vector_Push(&lines, Mary_OS_Textmap_Line_t, line);
      MARY_Vector_Push(&text, u16, it.val);
    }
    else if (it.val == '\n')
    {
      ++line.words.units, ++total_words;
      MARY_Vector_Push(&lines, Mary_OS_Textmap_Line_t, line);
      line.words.units = 0, line.w = 0, line.h = 0, line.user_w = 0, line.user_h = 0;
    }
    else if (it.val == ' ')
    {
      ++line.words.units, ++total_words;
      MARY_Vector_Push(&text, u16, it.val);
    }
    else
    {
      MARY_Vector_Push(&text, u16, it.val);
    }
  }

  // we create the bitmap from the \n-less text to conserve texture space
  int max_line_width = MARY_GL_MAX_TEXTURE_SIZE;
  RECT rect = { 0, 0, max_line_width, 0 }; unsigned int flags = DT_LEFT | DT_NOCLIP | DT_WORDBREAK;
  DrawTextEx(dib_context, text.data, (int)text.units, &rect, flags | DT_CALCRECT, 0);
  HBITMAP dib_bitmap; void *dib_data; int dib_bytes = rect.right * rect.bottom * 4;
  dib_bmi.bmiHeader.biWidth = rect.right;
  dib_bmi.bmiHeader.biHeight = -rect.bottom;
  dib_bmi.bmiHeader.biSizeImage = dib_bytes;
  dib_bitmap = CreateDIBSection(dib_context, &dib_bmi, DIB_RGB_COLORS, &dib_data, 0, 0);
  SelectObject(dib_context, dib_bitmap);
  DrawTextEx(dib_context, text.data, (int)text.units, &rect, flags, 0);

  // we now know how big to create our cache
  textmap->cache = Mary_Pool_Create(dib_bytes +
                                    MARY_Pool_Round(sizeof(Mary_OS_Textmap_Line_t)) * lines.units +
                                    MARY_Pool_Round(sizeof(Mary_OS_Textmap_Word_t)) * total_words);

  // set bitmap
  textmap->bitmap = (Mary_Bitmap_t) { 0, dib_bytes, (short)rect.right, (short)rect.bottom, 32 };
  textmap->bitmap.data = Mary_Pool_Alloc(textmap->cache, dib_bytes);
  memcpy(textmap->bitmap.data, dib_data, dib_bytes);

  // init our first line
  u64 line_idx = 0;
  Mary_OS_Textmap_Line_t *line_ptr = MARY_Vector_Point(&lines, line_idx);
  line_ptr->words.unit = sizeof(Mary_OS_Textmap_Word_t);
  line_ptr->words.bytes = line_ptr->words.units * line_ptr->words.unit;
  line_ptr->words.data = Mary_Pool_Alloc(textmap->cache, line_ptr->words.bytes);
  line_ptr->words.units = 0;

  // init our first word
  Mary_OS_Textmap_Word_t word; Mary_Slice_t word_slice = { 0, 0 }; u16 *word_data; int word_units;
  float bitmap_x = 0, bitmap_y = 0, bitmap_y_max = 0, norm_w, norm_h;
  MARY_Range(text_data, u16, 0, text_units)
  {
    // somehow we are not accounting for two /n's in a row...
    if (it.val == ' ' || it.val == '\n' || it.val == '\0')
    {
      word_slice.to_exclusive = it.val != '\n' ? it.idx + 1 : it.idx;
      word_data = (u16 *)(text_data) + word_slice.from;
      word_units = (int)(word_slice.to_exclusive - word_slice.from);
      word_slice.from = it.idx + 1;
      rect = (RECT) { 0, 0, max_line_width, 0 };
      DrawTextEx(dib_context, word_data, word_units, &rect, flags | DT_CALCRECT, 0);
      word.w = (float)rect.right;
      word.h = (float)rect.bottom;
      line_ptr->w += word.w;
      norm_w = word.w / textmap->bitmap.width;
      norm_h = word.h / textmap->bitmap.height;
      if (norm_h > bitmap_y_max) bitmap_y_max = norm_h;
      if (word.h > line_ptr->h) line_ptr->h = word.h;
      word.x1 = bitmap_x;
      bitmap_x += norm_w;
      if (bitmap_x > 1.0f)
      {
        word.x1 = 0.0f;
        bitmap_x = norm_w;
        bitmap_y += bitmap_y_max;
        bitmap_y_max = 0;
      }
      word.x2 = bitmap_x;
      word.y1 = bitmap_y;
      word.y2 = bitmap_y + norm_h;
      MARY_Vector_Push(&line_ptr->words, Mary_OS_Textmap_Word_t, word);
    }

    if (it.val == '\n')
    {
      ++line_idx;
      line_ptr = MARY_Vector_Point(&lines, line_idx);
      line_ptr->words.unit = sizeof(Mary_OS_Textmap_Word_t);
      line_ptr->words.bytes = line_ptr->words.units * line_ptr->words.unit;
      line_ptr->words.data = Mary_Pool_Alloc(textmap->cache, line_ptr->words.bytes);
      line_ptr->words.units = 0;
    }
  }

  // set lines
  textmap->lines.bytes = lines.units * lines.unit;
  textmap->lines.data = Mary_Pool_Alloc(textmap->cache, textmap->lines.bytes);
  textmap->lines.unit = lines.unit;
  textmap->lines.units = lines.units;
  memcpy(textmap->lines.data, lines.data, textmap->lines.bytes);

  // cleanup
  DeleteObject(dib_bitmap);
  Mary_Vector_Destroy(&lines);
  Mary_Vector_Destroy(&text);
}

void Mary_OS_Textmap_Destroy(Mary_OS_Textmap_t *textmap)
{
  Mary_Pool_Destroy(textmap->cache);
}

#elif defined(__linux__)

#define to_be_done

#endif
