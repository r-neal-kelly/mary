#include <Mary/Mary.h>

void Mary_Start()
{
  Mary_Pool_Start();
  Mary_Arena_Start();
  Mary_OS_Start();
  Mary_OpenGL_Start();
  Mary_GL_Texture_Start();
  Mary_Unicode_Start();
  Mary_Regex_Start();
  Mary_Element_Start();
  Mary_Window_Start();
  Mary_Div_Start();
  Mary_Text_Start();
}

void Mary_Stop()
{
  Mary_Text_Finish();
  Mary_Div_Finish();
  Mary_Window_Finish();
  Mary_Element_Finish();
  Mary_Regex_Stop();
  Mary_Unicode_Stop();
  Mary_GL_Texture_Finish();
  Mary_OpenGL_Finish();
  Mary_OS_Finish();
  Mary_Arena_Stop();
  Mary_Pool_Stop();
}
