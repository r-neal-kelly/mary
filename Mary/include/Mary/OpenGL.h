#pragma once

#include <Mary/OS.h>

#define GL_FALSE 0x0000
#define GL_TRUE 0x0001
#define GL_VENDOR 0x1F00
#define GL_RENDERER 0x1F01
#define GL_VERSION 0x1F02
#define GL_EXTENSIONS 0x1F03
#define GL_COLOR_BUFFER_BIT 0x4000
#define GL_DEPTH_BUFFER_BIT 0x0100
#define GL_STENCIL_BUFFER_BIT 0x0400
#define GL_MODELVIEW 0x1700
#define GL_PROJECTION 0x1701
#define GL_TEXTURE 0x1702
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPUTE_SHADER 0x91B9
#define GL_COMPILE_STATUS 0x8B81
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_ARRAY_BUFFER 0x8892
#define GL_COPY_READ_BUFFER 0x8F36
#define GL_COPY_WRITE_BUFFER 0x8F37
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_PIXEL_PACK_BUFFER 0x88EB
#define GL_PIXEL_UNPACK_BUFFER 0x88EC
#define GL_TEXTURE_BUFFER 0x8C2A
#define GL_TRANSFORM_FEEDBACK_BUFFER 0x8C8E
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_STREAM_DRAW 0x88E0
#define GL_STREAM_READ 0x88E1
#define GL_STREAM_COPY 0x88E2
#define GL_STATIC_DRAW 0x88E4
#define GL_STATIC_READ 0x88E5
#define GL_STATIC_COPY 0x88E6
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_DYNAMIC_READ 0x88E9
#define GL_DYNAMIC_COPY 0x88EA
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_HALF_FLOAT 0x140B
#define GL_FLOAT 0x1406
#define GL_DOUBLE 0x140A
#define GL_POINTS 0x0000
#define GL_LINE_STRIP 0x0003
#define GL_LINE_LOOP 0x0002
#define GL_LINES 0x0001
#define GL_LINE_STRIP_ADJACENCY 0x000B
#define GL_LINES_ADJACENCY 0x000A
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP_ADJACENCY 0x000D
#define GL_TRIANGLES_ADJACENCY 0x000C
#define GL_NO_ERROR 0x0000
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#define GL_OUT_OF_MEMORY 0x0505
#define GL_FRONT_AND_BACK 0x0408
#define GL_POINT 0x1B00
#define GL_LINE 0x1B01
#define GL_FILL 0x1B02
#define GL_CURRENT_PROGRAM 0x8B8D
#define GL_BLEND 0x0BE2
#define GL_DEPTH_TEST 0x0B71
#define GL_SCISSOR_TEST 0x0C11
#define GL_MAX_TEXTURE_SIZE 0x0D33
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7
#define GL_TEXTURE_1D 0x0DE0
#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_3D 0x806F
#define GL_TEXTURE_1D_ARRAY 0x8C18
#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_TEXTURE_RECTANGLE 0x84F5
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_BUFFER 0x8C2A
#define GL_TEXTURE_2D_MULTISAMPLE 0x9100
#define GL_TEXTURE_2D_MULTISAMPLE_ARRAY 0x9102
#define GL_TEXTURE_BINDING_1D 0x8068
#define GL_TEXTURE_BINDING_2D 0x8069
#define GL_TEXTURE_BINDING_3D 0x806A
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_LINEAR 0x2601
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_RGBA 0x1908
#define GL_BGRA 0x80E1
#define GL_UNPACK_ALIGNMENT 0x0CF5
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_ARRAY_BUFFER_BINDING 0x8894
#define GL_CURRENT_PROGRAM 0x8B8D

typedef float GLfloat;
typedef double GLdouble;
typedef int GLint;
typedef char GLchar;
typedef void GLvoid;
typedef unsigned char GLboolean;
typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef unsigned int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned int GLbitfield;
typedef size_t GLsizeiptr;

// Buffer Objects
void (MARY_CALL *glBindBuffer)(GLenum target, GLuint buffer);
void (MARY_CALL *glBufferData)(GLenum target, GLsizeiptr data_size, const GLvoid *data, GLenum usage);
void (MARY_CALL *glDeleteBuffers)(GLsizei count, const GLuint *buffers);
void (MARY_CALL *glDisableVertexAttribArray)(GLuint location_in_shader);
void (MARY_CALL *glDrawArrays)(GLenum mode, GLint first_vertex, GLsizei vertex_count);
void (MARY_CALL *glDrawElements)(GLenum mode, GLsizei elem_count, GLenum elem_type, const GLvoid *elements);
void (MARY_CALL *glEnableVertexAttribArray)(GLuint location_in_shader);
void (MARY_CALL *glGenBuffers)(GLsizei count, GLuint *buffers);
void (MARY_CALL *glVertexAttribIPointer)(GLuint location_in_shader, GLint elems_per_attrib, GLenum attrib_elem_type, GLsizei vertex_bytes, const GLvoid *attrib_offset_bytes);
void (MARY_CALL *glVertexAttribPointer)(GLuint location_in_shader, GLint elems_per_attrib, GLenum attrib_elem_type, GLboolean normalize, GLsizei vertex_bytes, const GLvoid *attrib_offset_bytes);

// Rendering
void (MARY_CALL *glClear)(GLbitfield mask);
void (MARY_CALL *glClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

// Shaders
void (MARY_CALL *glAttachShader)(GLuint program, GLuint shader);
void (MARY_CALL *glCompileShader)(GLuint shader);
GLuint (MARY_CALL *glCreateProgram)(void);
GLuint (MARY_CALL *glCreateShader)(GLenum shader_type);
void (MARY_CALL *glDeleteProgram)(GLuint program);
void (MARY_CALL *glDeleteShader)(GLuint shader);
void (MARY_CALL *glDetachShader)(GLuint program, GLuint shader);
void (MARY_CALL *glGetShaderiv)(GLuint shader, GLenum parameter, GLint *out_params);
void (MARY_CALL *glGetShaderInfoLog)(GLuint shader, GLsizei max_length, GLsizei *length, GLchar *out_info_log);
GLint (MARY_CALL *glGetUniformLocation)(GLuint program, const GLchar *name);
void (MARY_CALL *glLinkProgram)(GLuint program);
void (MARY_CALL *glShaderSource)(GLuint shader, GLsizei count, const GLchar **out_string, const GLint *length);
void (MARY_CALL *glUniform1i)(GLint location, GLint val_0);
void (MARY_CALL *glUniform4f)(GLint location, GLfloat val_0, GLfloat val_1, GLfloat val_2, GLfloat val_3);
void (MARY_CALL *glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void (MARY_CALL *glUseProgram)(GLuint program);
void (MARY_CALL *glValidateProgram)(GLuint program);

// State Management
void (MARY_CALL *glBlendFunc)(GLenum source_factor, GLenum destination_factor);
void (MARY_CALL *glEnable)(GLenum capability);
void (MARY_CALL *glGetIntegerv)(GLenum param_name, GLint *params);
GLenum (MARY_CALL *glGetError)(void);
void (MARY_CALL *glPixelStorei)(GLenum param_name, GLint param);
void (MARY_CALL *glPolygonMode)(GLenum face, GLenum mode);
void (MARY_CALL *glScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
void (MARY_CALL *glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);

// Textures
void (MARY_CALL *glActiveTexture)(GLenum texture_slot);
void (MARY_CALL *glBindTexture)(GLenum target, GLuint texture);
void (MARY_CALL *glDeleteTextures)(GLsizei count, const GLuint *textures);
void (MARY_CALL *glGenTextures)(GLsizei count, GLuint *textures);
void (MARY_CALL *glTexImage2D)(GLenum target, GLint level, GLint internal_format, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *data);
void (MARY_CALL *glTexParameteri)(GLenum target, GLenum param_name, GLint param);

// Utility
const GLubyte *(MARY_CALL *glGetString)(GLenum name);

// Vertex Array Objects
void (MARY_CALL *glBindVertexArray)(GLuint array_object);
void (MARY_CALL *glDeleteVertexArrays)(GLsizei count, const GLuint *array_objects);
void (MARY_CALL *glGenVertexArrays)(GLsizei count, GLuint *array_objects);

typedef struct
{
  float matrix[16];
}
Mary_Matrix_4x4f;

GLuint MARY_GL_MAX_TEXTURE_SIZE;
GLuint MARY_GL_MAX_TEXTURES;

void Mary_OpenGL_Start();
void Mary_OpenGL_Finish();
GLuint Mary_OpenGL_Program(char *vertex, char *fragment);
Mary_Matrix_4x4f Mary_OpenGL_Identity();
Mary_Matrix_4x4f Mary_OpenGL_Ortho(float left, float right, float bottom, float top, float near_val, float far_val);
void Mary_OpenGL_Translate(Mary_Matrix_4x4f *matrix, float x, float y, float z);
void Mary_OpenGL_Scale(Mary_Matrix_4x4f *matrix, float x, float y, float z);
void Mary_OpenGL_Rotate_Z(Mary_Matrix_4x4f *matrix, float degrees);
