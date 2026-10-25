#ifndef OPENGL_CORE_INCLUDE
#define OPENGL_CORE_INCLUDE
#include "../../core/core.h"
#include "../../core/string.h"








/////////////////////
// ~sameer: os header and os specific things

#if OS_WINDOWS
#include "./windows/win32_gl.h"

#else
#error where de foak are we (opengl_core.h unknown os)

#endif



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ~sameer: @NOTE, win32's gl.h only contains types and funcs in opengl 1.1 and below. Things like GL_FRAMEBUFFER, genBuffers are missing. Look up glext.h on khronos' registry to find missing things  (https://registry.khronos.org/OpenGL/api/GL/glext.h)

/////////////////////////////////
// ~sameer: 1.2 and above opengl defines & functions

#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_DYNAMIC_DRAW                   0x88E8

#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82
#define GL_TEXTURE0                       0x84C0
#define GL_MIRRORED_REPEAT                0x8370
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_CLAMP_TO_BORDER                0x812D

typedef char GLchar;

#define OPENGL_FUNCS							\
  t(glGenVertexArrays,void,(GLsizei n, GLuint *arrays));		\
  t(glGenBuffers,void,(GLsizei n, GLuint *buffers));			\
  t(glBufferData,void,(GLenum target, ptrdiff_t size, const void *data, GLenum usage)); \
  t(glBindBuffer,void,(GLenum target, GLuint buffer));			\
  t(glBindBuffer,void,(GLenum target, GLuint buffer));			\
  t(glBindVertexArray,void, (GLuint array));				\
  t(glVertexAttribPointer,void, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)); \
  t(glEnableVertexAttribArray,void, (GLuint index));			\
  t(glCreateShader,GLuint, (GLenum type));				\
  t(glDeleteShader,void, (GLuint shader));				\
  t(glShaderSource,void, (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length)); \
  t(glCompileShader ,void,(GLuint shader));				\
  t(glGetShaderiv,void, (GLuint shader, GLenum pname, GLint *params));	\
  t(glGetShaderInfoLog,void,(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)); \
  t(glCreateProgram,GLuint, (void));					\
  t(glLinkProgram,void, (GLuint program));				\
  t(glAttachShader,void, (GLuint program, GLuint shader));		\
  t(glGetProgramiv, void,(GLuint program, GLenum pname, GLint *params)); \
  t(glGetProgramInfoLog,void, (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog)); \
  t(glUseProgram,void, (GLuint program));				\
  t(glDrawElementsBaseVertex,void, (GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex)); \
  t(glBufferSubData,void, (GLenum target, ptrdiff_t offset, ptrdiff_t size, const void *data)); \
  t(glGetUniformLocation,GLint, (GLuint program, const GLchar *name));	\
  t(glUniformMatrix4fv,void, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)); \
  t(glProgramUniform4f,void, (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)); \
  t(glProgramUniform2f,void, (GLuint program, GLint location, GLfloat v0, GLfloat v1)); \
  t(glGenerateMipmap,void, (GLenum target));				\
  t(glActiveTexture,void, (GLenum texture));				\

  // t for temp
#define t(name,ret,p) typedef ret func_type_##name p
OPENGL_FUNCS
#undef t

#define t(name,ret,p) func_type_##name *name
OPENGL_FUNCS
#undef t




//////////////////////////////////
// ~sameer: implementing gfx types
// GL_TEXTURE31 is the last one
#define OPENGL_MAX_TEXTURE_SLOTS 32




/////////////////////////////////
// ~sameer: opengl_core functions and types

typedef struct GLShader{
  GLuint id;
  GLint u_color;
  GLint u_model, u_view, u_projection;
  GLint u_tiling, u_offset;
} GLShader;

typedef struct GLState{
  b32 shaderIsBound;
  GLShader boundShader;

  b32 textureIsBound;
  u32 numActiveTextures;
  GfxTexture boundTexture;

  mat4x4 projectionMatrix;
  mat4x4 viewMatrix;
  mat4x4 modelMatrix;  

  mat4x4 lastProjectionMatAssigned, lastViewMatAssigned, lastModelMatAssigned;
  
  GLuint vertBuffer;
  GLuint vertArray;
  GLuint indicesBuffer;


  mi currentIndicesOffset;
  mi currentIndicesOffsetBytes;
  mi currentVerticesOffset;
  mi currentVerticesOffsetBytes;
  Arena *arena;


  mi windowWidth, windowHeight;
  r32 halfWindowWidth, halfWindowHeight;
} GLState;

global GLState gl_State;

internal void gl_init(Window window);
internal GLuint gl_create_and_compile_shader(string8 src,GLuint type, b32 alphaClipping);
internal GLuint gl_create_and_link_shaders(GLuint vert, GLuint frag, b32 deleteVertAndFrag);
internal GLuint gl_create_full_shader_from_sources(string8 vertSrc, string8 fragSrc, b32 alphaClipping);
internal void gl_bind_shader(GLShader shader);
internal void gl_bind_texture(GfxTexture texture);







////////////////////////
// ~sameer: per platform. Problem we're trying to solve is that loading opengl functions and context creation is not part of the spec, and must be done differently for each platform.
internal void gl_os_init(Window window);
internal void *gl_get_func_address(char *name);
internal void gl_swap_interval(i32 val);





#endif // OPENGL_CORE_INCLUDE
