#pragma once

#include <Mary/Utils.h>
#include <Mary/OpenGL.h>

typedef struct
{
  GLuint id;
  GLint slot;
  GLenum target;
}
Mary_GL_Texture_t;

void Mary_GL_Texture_Start();
void Mary_GL_Texture_Finish();
void Mary_GL_Texture_Create(Mary_GL_Texture_t *texture, GLenum target);
void Mary_GL_Texture_Destroy(Mary_GL_Texture_t *texture);
void Mary_GL_Texture_Activate(Mary_GL_Texture_t *texture);
