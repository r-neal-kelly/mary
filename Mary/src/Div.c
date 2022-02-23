#include <stdio.h>
#include <Mary/Utils.h>
#include <Mary/OpenGL.h>
#include <Mary/Vector.h>
#include <Mary/Window.h>
#include <Mary/Div.h>

MARY_Primitives;

Mary_Vector_t v_divs; // currently not used.

static GLuint g_VAO = 0;
static GLuint g_VBO = 0;
static GLuint g_EBO = 0;
static GLuint g_program = 0;
static GLint g_u_model = -1;
static GLint g_u_projection = -1;
static GLint g_u_color = -1;

void Mary_Div_Start()
{
  ////// Vertex Array Object
  glGenVertexArrays(1, &g_VAO);
  glBindVertexArray(g_VAO);
  ////// Vertex Buffer Object
  float vertices[] =
  {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
  };
  glGenBuffers(1, &g_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *)0);
  glEnableVertexAttribArray(0);
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
  g_program = Mary_OpenGL_Program("Mary/shaders/div_v.shader", "Mary/shaders/div_f.shader");
  ////// Uniforms
  g_u_projection = glGetUniformLocation(g_program, "u_projection");
  g_u_color = glGetUniformLocation(g_program, "u_color");
}

void Mary_Div_Finish()
{
  // I do think we should keep a cache of all active divs, it could be quite useful.
  // we may even centralize all such cache in element.c
}

void Mary_Div_Create(Mary_Div_t *mary_div)
{
  Mary_Element_Create(mary_div, MARY_ELEMENT_DIV);
}

void Mary_Div_Destroy(Mary_Div_t *mary_div)
{
  Mary_Element_Destroy(mary_div);
}

void Mary_Div_Update_Size(void *mary_div)
{
  Mary_Div_t *div = MARY_Div(mary_div);

  // need to handle none display still

  if (div->unit_w == MARY_ELEMENT_UNIT_FIT_PARENT || div->unit_w == MARY_ELEMENT_UNIT_FIT_CHILDREN)
  {
    Mary_Element_Handle_Unit_Fit_Parent_W(div);
  }
  else if (div->unit_w == MARY_ELEMENT_UNIT_PIXEL)
  {
    Mary_Element_Handle_Unit_Pixel_W(div);
  }

  if (div->unit_h == MARY_ELEMENT_UNIT_FIT_PARENT || div->unit_h == MARY_ELEMENT_UNIT_FIT_CHILDREN)
  {
    Mary_Element_Handle_Unit_Fit_Parent_H(div);
  }
  else if (div->unit_h == MARY_ELEMENT_UNIT_PIXEL)
  {
    Mary_Element_Handle_Unit_Pixel_H(div);
  }

  Mary_Element_Handle_Children_Display(div);

  if (div->unit_w == MARY_ELEMENT_UNIT_FIT_CHILDREN)
  {
    div->w_pad = div->w_children;
    div->w = div->w_pad + div->pad_l + div->pad_r;
    div->w_margin = div->w + div->margin_l + div->margin_r;
  }

  if (div->unit_h == MARY_ELEMENT_UNIT_FIT_CHILDREN)
  {
    div->h_pad = div->h_children;
    div->h = div->h_pad + div->pad_t + div->pad_b;
    div->h_margin = div->h + div->margin_t + div->margin_b;
  }

  Mary_Element_Handle_Scroll_Max(div);
}

void Mary_Div_Update_Positions(void *mary_div)
{
  Mary_Div_t *div = MARY_Div(mary_div);
  Mary_Element_Handle_Children_Positions(div);
}

void Mary_Div_Render(void *mary_div)
{
  Mary_Div_t *div = MARY_Div(mary_div);
  ////// Set VAO, VBO, EBO
  glBindVertexArray(g_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_EBO);
  ////// Set Program
  glUseProgram(g_program);
  ////// Set Uniforms
  glUniformMatrix4fv(g_u_projection, 1, GL_TRUE, div->window->projection.matrix);
  glUniform4f(g_u_color, div->back_r, div->back_g, div->back_b, div->back_a);
  ////// Set Vertices
  float vertices[] =
  {
    div->x1, div->y1,
    div->x2, div->y1,
    div->x2, div->y2,
    div->x1, div->y2,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
  ////// Draw
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
