#include <stdio.h>
#include <stdlib.h>
#include <Mary/Utils.h>
#include <Mary/File.h>
#include <Mary/OpenGL.h>

MARY_Primitives;

static GLuint Load_Shader(char *file_path, GLenum shader_type);

void Mary_OpenGL_Start()
{
  int max_texture_size, max_textures;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_textures);
  MARY_GL_MAX_TEXTURE_SIZE = (GLuint)+max_texture_size;
  MARY_GL_MAX_TEXTURES = (GLuint)+max_textures / 2;
}

void Mary_OpenGL_Finish()
{

}

GLuint Mary_OpenGL_Program(char *vertex, char *fragment)
{
  GLuint program = glCreateProgram();
  GLuint shader_vertex = Load_Shader(vertex, GL_VERTEX_SHADER);
  GLuint shader_fragment = Load_Shader(fragment, GL_FRAGMENT_SHADER);
  glAttachShader(program, shader_vertex);
  glAttachShader(program, shader_fragment);
  glLinkProgram(program);
  glValidateProgram(program);
  // need to make sure program is valid with glGetProgramiv

  glDetachShader(program, shader_vertex);
  glDetachShader(program, shader_fragment);
  glDeleteShader(shader_vertex);
  glDeleteShader(shader_fragment);
  return program;
}

static GLuint Load_Shader(char *file_path, GLenum shader_type)
{
  MARY_String_Static(path, 8, file_path);
  Mary_File_t file; Mary_File_String_Create(&file, &path, MARY_FILE_READ, 8);
  Mary_String_t buffer; Mary_File_String_Read_All(&file, &buffer, 0);

  GLuint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &(char *)buffer.data, 0);
  glCompileShader(shader);
  int did_compile;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &did_compile);
  if (did_compile == GL_FALSE)
  {
    int length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    char errorMsg[1024];
    glGetShaderInfoLog(shader, length, &length, errorMsg);
    glDeleteShader(shader);
    Mary_Exit_Failure(errorMsg);
  }

  Mary_String_Destroy(&buffer);
  Mary_File_Destroy(&file);
  return shader;
}

Mary_Matrix_4x4f Mary_OpenGL_Identity()
{
  Mary_Matrix_4x4f identity =
  {
    {
      1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1
    }
  };
  return identity;
}

Mary_Matrix_4x4f Mary_OpenGL_Ortho(float l, float r, float b, float t, float n, float f)
{
  Mary_Matrix_4x4f ortho =
  {
    {
      2/(r-l),       0,        0, -((r+l)/(r-l)),
            0, 2/(t-b),        0, -((t+b)/(t-b)),
            0,       0, -2/(f-n), -((f+n)/(f-n)),
            0,       0,        0,              1
    }
  };
  return ortho;
}

void Mary_OpenGL_Translate(Mary_Matrix_4x4f *matrix, float x, float y, float z)
{
  matrix->matrix[3] += x;
  matrix->matrix[7] += y;
  matrix->matrix[11] += z;
}

void Mary_OpenGL_Scale(Mary_Matrix_4x4f *matrix, float x, float y, float z)
{
  matrix->matrix[0] *= x;
  matrix->matrix[5] *= y;
  matrix->matrix[10] *= z;
}

void Mary_OpenGL_Rotate_Z(Mary_Matrix_4x4f *matrix, float degrees)
{
  // need to do this sometime.
}
