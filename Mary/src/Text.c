#include <stdio.h>
#include <stdlib.h>
#include <Mary/File.h>
#include <Mary/OS.h>
#include <Mary/OpenGL.h>
#include <Mary/Window.h>
#include <Mary/Text.h>

MARY_Primitives;

static GLuint g_VAO = 0;
static GLuint g_VBO = 0;
static GLuint g_EBO = 0;
static GLuint g_program = 0;
static GLint g_u_projection = -1;
static GLint g_u_color = -1;
static GLint g_u_texture = -1;

void Mary_Text_Start()
{
  ////// Vertex Array Object
  glGenVertexArrays(1, &g_VAO);
  glBindVertexArray(g_VAO);
  ////// Vertex Buffer Object
  float vertices[] =
  {
    // xy, uv
    0.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f
  };
  glGenBuffers(1, &g_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)(sizeof(float) * 2));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  ////// Element Buffer Object
  unsigned int indices[] =
  {
    0, 1, 2,
    2, 3, 0
  };
  glGenBuffers(1, &g_EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  ////// Program
  g_program = Mary_OpenGL_Program("Mary/shaders/text_v.shader", "Mary/shaders/text_f.shader");
  ////// Uniforms
  g_u_projection = glGetUniformLocation(g_program, "u_projection");
  g_u_color = glGetUniformLocation(g_program, "u_color");
  g_u_texture = glGetUniformLocation(g_program, "u_texture");
}

void Mary_Text_Finish()
{
  // might want to delete program.
  // but I think I want a manager for it like the GL_Texture_t.
}

void Mary_Text_Create(Mary_Text_t *elem, char bit_format, void *text, size_t opt_units)
{
  ////// Element
  Mary_Element_Create(elem, MARY_ELEMENT_TEXT);
  ////// String
  Mary_String_Create_From(&elem->string, MARY_Allocator_Heap, bit_format, text, bit_format);
  Mary_String_Recode(&elem->string, 16);
  ////// Textmap
  Mary_OS_Textmap_Create(&elem->textmap, elem->string.data, (int)elem->string.units);
  ////// Texture
  Mary_GL_Texture_Create(&elem->texture, GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               elem->textmap.bitmap.width,
               elem->textmap.bitmap.height,
               0, GL_BGRA, GL_UNSIGNED_BYTE,
               elem->textmap.bitmap.data);
}

void Mary_Text_Create_With_File(Mary_Text_t *elem, char bit_format, char *file_path)
{
  ////// Element
  Mary_Element_Create(elem, MARY_ELEMENT_TEXT);
  ////// String
  MARY_String_Static(path, 8, file_path);
  Mary_File_t file; Mary_File_String_Create(&file, &path, MARY_FILE_READ, 8);
  Mary_File_String_Read_All(&file, &elem->string, MARY_FALSE);
  Mary_String_Recode(&elem->string, 16);
  Mary_File_Destroy(&file);
  ////// Textmap
  Mary_OS_Textmap_Create(&elem->textmap, elem->string.data, (int)elem->string.units);
  ////// Texture
  Mary_GL_Texture_Create(&elem->texture, GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               elem->textmap.bitmap.width,
               elem->textmap.bitmap.height,
               0, GL_BGRA, GL_UNSIGNED_BYTE,
               elem->textmap.bitmap.data);
}

void Mary_Text_Destroy(Mary_Text_t *elem)
{
  Mary_GL_Texture_Destroy(&elem->texture);
  Mary_OS_Textmap_Destroy(&elem->textmap);
  Mary_String_Destroy(&elem->string);
  Mary_Element_Destroy(elem);
}

void Mary_Text_Update_Size(void *mary_text)
{
  Mary_Text_t *elem = MARY_Text(mary_text);
  Mary_Element_Handle_Unit_Fit_Parent_W(elem);
  float w = 0, h = 0;
  MARY_Vector_Each(&elem->textmap.lines, Mary_OS_Textmap_Line_t)
  {
    Mary_OS_Textmap_Line_t *line = it.ptr;
    line->user_h = 0;
    MARY_Vector_Each(&line->words, Mary_OS_Textmap_Word_t)
    {
      Mary_OS_Textmap_Word_t *word = it.ptr;
      if ((w += word->w) > elem->w)
      {
        w = word->w;
        h += line->h;
        line->user_h += line->h;
      }
    }
    w = 0;
    h += line->h;
    line->user_h += line->h;
  }
  Mary_Element_Size(elem, elem->w, h);
  Mary_Element_Handle_Unit_Pixel_H(elem);
  Mary_Element_Overflow(elem, MARY_ELEMENT_OVERFLOW_NONE);
}

void Mary_Text_Update_Positions(void *mary_text)
{
  return; // never has children.
}

void Mary_Text_Render(void *mary_text)
{
  Mary_Text_t *elem = MARY_Text(mary_text);
  ////// Set VAO, VBO, EBO
  GLint current_VBO; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &current_VBO);
  if (current_VBO != g_VBO)
  {
    glBindVertexArray(g_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
  }
  ////// Set Program
  GLint current_program; glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
  if (current_program != g_program)
  {
    glUseProgram(g_program);
  }
  ////// Set Texture
  Mary_GL_Texture_Activate(&elem->texture);
  ////// Set Uniforms
  glUniformMatrix4fv(g_u_projection, 1, GL_TRUE, elem->window->projection.matrix);
  glUniform4f(g_u_color, elem->fore_r, elem->fore_g, elem->fore_b, elem->fore_a);
  glUniform1i(g_u_texture, elem->texture.slot);
  ////// Draw
  float x = elem->x1, y = elem->y1;
  MARY_Vector_Each(&elem->textmap.lines, Mary_OS_Textmap_Line_t)
  {
    Mary_OS_Textmap_Line_t *line = it.ptr;
    if (y + line->user_h < elem->clip_y1)
    {
      y += line->user_h; continue;
    }
    MARY_Vector_Each(&line->words, Mary_OS_Textmap_Word_t)
    {
      Mary_OS_Textmap_Word_t *word = it.ptr;
      if (x + word->w > elem->x2)
      {
        x = elem->x1, y += line->h;
        if (y > elem->clip_y2) return;
      }
      float vertices[] =
      {
        x          , y          , word->x1, word->y1,
        x + word->w, y          , word->x2, word->y1,
        x + word->w, y + word->h, word->x2, word->y2,
        x          , y + word->h, word->x1, word->y2
      };
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      x += word->w;
    }
    x = elem->x1, y += line->h;
    if (y > elem->clip_y2) return;
  }
}
