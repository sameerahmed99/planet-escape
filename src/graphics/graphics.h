#ifndef GRAPHICS_CORE_INCLUDE
#define GRAPHICS_CORE_INCLUDE


/////////////////////////////////////////////////////
// ~sameer: types to be implemented by underlying api

// nothing yet




#define GPU_CMD_ALIGNMENT 16

typedef struct GfxCmdList GfxCmdList;
#define GFX_VERTEX_BUFFER_SIZE MB(50)
#define GFX_INDICES_BUFFER_SIZE MB(20)




// @TODO, consider adding a parameter to pass rotation to each command, since most calls will not be about changing the projection or view matrix
// will remove the need to always push a model matrix before pushing a render command

/////////////////////////////////
// ~sameer: graphics.h core types

typedef enum GfxRenderingStyle{
  GFX_RENDERING_STYLE_SPRITE_UNLIT,
  GFX_RENDERING_STYLE_SPRITE_LIT,
} GfxRenderingStyle;

typedef enum GfxMatrixType {
  GFX_MATRIX_TYPE_PROJECTION,
  GFX_MATRIX_TYPE_VIEW,
  GFX_MATRIX_TYPE_MODEL,
} GfxMatrixType;



struct GfxCmdList{
  mi numCmds;
  Arena *arena;
};

typedef enum GfxCmdType{
  GFX_CMD_CLEAR,
  GFX_CMD_TEXTURED_TRIANGLE,
  GFX_CMD_COLORED_TRIANGLE,
  GFX_CMD_COLORED_QUAD,
  GFX_CMD_TEXTURED_QUAD,
  GFX_CMD_WIRE_RECTANGLE,
  GFX_CMD_WIRE_CIRCLE,
  GFX_CMD_TEXT,
  GFX_CMD_SET_MATRIX,
} GfxCmdType;


typedef struct GfxCmdHeader{
  GfxCmdType type;
} GfxCmdHeader;




typedef struct GfxCmdClear{
  GfxCmdHeader header;
  Color color;
} GfxCmdClear;

typedef struct GfxCmdTexturedTriangle{
  GfxCmdHeader header;
  Vertex v1,v2,v3;

  Texture *texture;
  Color clampColor;
  Color color;

  TilingAndOffset tilingOff;

} GfxCmdTexturedTriangle;

typedef struct GfxCmdColoredTriangle{
  GfxCmdHeader header;
  Color color;
  Vertex v1,v2,v3;
} GfxCmdColoredTriangle;

// these are just empty, we use two triangle commands for a quad
typedef struct GfxCmdColoredQuad{
  GfxCmdHeader header;
} GfxCmdColoredQuad;



// just empty, we use two triangle commands for a quad
typedef struct GfxCmdTexturedQuad{
  GfxCmdHeader header;
} GfxCmdTexturedQuad;

typedef struct GfxCmdWireRectangle{
  GfxCmdHeader header;
  vec3 center;
  r32 width;
  r32 height;
  r32 thickness;
  Color color;
}GfxCmdWireRectangle;

typedef struct GfxCmdText{
  GfxCmdHeader header;
  Font *font;
  vec3 pos;
  TextAlignment alignment;
  Color color;
  string8 text;
  i32 sizePx;
} GfxCmdText;


typedef struct GfxCmdSetMatrix{
  GfxCmdHeader header;
  GfxMatrixType type;
  mat4x4 mat;
} GfxCmdSetMatrix;



typedef struct GfxMaterial GfxMaterial;
struct GfxMaterial{
  b32 alphaClipping;
  b32 transparent;
  b32 frontFace;
  b32 backFace;
  GfxRenderingStyle style;
  Texture *texture;
  Color tint;
  TilingAndOffset tilingOff;
};


internal const Vertex QuadVertices[] = {
  {.pos = {-0.5f,-0.5f,0.0f}, .uv={0,0}, .col={1,1,1,1}},
  {.pos = {-0.5f,0.5f,0.0f}, .uv={0,1}, .col={1,1,1,1}},
  {.pos = {0.5f,0.5f,0.0f}, .uv={1,1}, .col={1,1,1,1}},
  {.pos = {0.5f,-0.5f,0.0f}, .uv={1,0}, .col={1,1,1,1}},
};
internal const i32 QuadIndices[] = {
  0,1,2,
  2,3,0
};


void gfx_push_clear(GfxCmdList *list, Color color);
void gfx_push_textured_triangle(GfxCmdList *list,TilingAndOffset tilingAndOffset, Color clampColor,Color tint,Texture *tex, Vertex v1, Vertex v2, Vertex v3);
void gfx_push_colored_triangle(GfxCmdList *list, Color color, Vertex v1, Vertex v2, Vertex v3);
void gfx_push_colored_quad(GfxCmdList *list, Color color, r32 width, r32 height, vec3 center);
void gfx_push_textured_quad(GfxCmdList *list, TilingAndOffset tilingAndOffset, Color clampColor, Color tint, Texture *tex, r32 width, r32 height, vec3 center, r32 rotation);
void gfx_push_wire_rectangle(GfxCmdList *list, Color color, r32 width, r32 height, vec3 center, r32 thickness);
void gfx_push_wire_circle(GfxCmdList *list, Color color, r32 width, r32 height, vec3 center, r32 thickness);
void gfx_push_text(GfxCmdList *list, Font *font, string8 text, i32 sizepx,vec3 pos, TextAlignment alignment, Color color);
void gfx_push_set_matrix(GfxCmdList *list, GfxMatrixType type, mat4x4 mat);


void gfx_init(Window window);
void gfx_exec_list_and_clear(GfxCmdList *list);




////////////////////////////////////////////////////
// ~sameer: to be implemented by underlying renderer
void gfx_underlying_exec_list(GfxCmdList *list);
b32 gfx_renderer_requires_textures_to_be_kept_on_cpu_memory();
void gfx_set_swap_interval(i32 val);


// just opengl, ~for now~
#include "opengl/opengl_core.h"


#endif // GRAPHICS_CORE_INCLUDE
