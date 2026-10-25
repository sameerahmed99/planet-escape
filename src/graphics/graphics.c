

#include "opengl/opengl_core.c"



void gfx_init(Window window){
  gl_init(window);
}


void gfx_exec_list_and_clear(GfxCmdList *list){
  gfx_underlying_exec_list(list);
  list->numCmds = 0;
  arena_clear(list->arena);
}

void gfx_push_clear(GfxCmdList *list, Color color){
  GfxCmdClear *cmd=PushStructCustomAlign(list->arena, GfxCmdClear, GPU_CMD_ALIGNMENT);
  cmd->header.type = GFX_CMD_CLEAR;
  cmd->color = color;
  list->numCmds++;
}



void gfx_push_textured_triangle(GfxCmdList *list,TilingAndOffset tilingAndOffset, Color clampColor,Color tint,Texture *texture, Vertex v1, Vertex v2, Vertex v3){
  GfxCmdTexturedTriangle c = {
    .v1 = v1,
    .v2 = v2,
    .v3 = v3,
    .texture = texture,
    .clampColor = clampColor,
    .color = tint,
    .tilingOff = tilingAndOffset
  };


  GfxCmdTexturedTriangle *cmd=PushStructCustomAlign(list->arena, GfxCmdTexturedTriangle,GPU_CMD_ALIGNMENT);
  *cmd = c;
  cmd->header.type=GFX_CMD_TEXTURED_TRIANGLE;
  list->numCmds++;
}


void gfx_push_colored_triangle(GfxCmdList *list, Color color, Vertex v1, Vertex v2, Vertex v3){
  GfxCmdColoredTriangle c = {
    .color = color,
    .v1 = v1,
    .v2 = v2,
    .v3 = v3
  };


  GfxCmdColoredTriangle *cmd=PushStructCustomAlign(list->arena, GfxCmdColoredTriangle,GPU_CMD_ALIGNMENT);
  *cmd = c;
  cmd->header.type=GFX_CMD_COLORED_TRIANGLE;
  list->numCmds++;
}

void gfx_push_colored_quad(GfxCmdList *list, Color color, r32 width, r32 height, vec3 center){

  r32 hw = width/2;
  r32 hh = height/2;

  
  
  vec3 p1 = center;
  p1.x-= hw;
  p1.y-=hh;

  vec3 p2 = center;
  p2.x-= hw;
  p2.y+=hh;

  vec3 p3 = center;
  p3.x+= hw;
  p3.y+=hh;

  vec3 p4 = center;
  p4.x += hw;
  p4.y -= hh;


  Vertex v1 = {.pos = p1, .uv = {0,0}, .col = {0}};
  Vertex v2 = {.pos = p2, .uv = {0,1}, .col = {0}};
  Vertex v3 = {.pos = p3, .uv = {1,1}, .col = {0}};
  Vertex v4 = {.pos = p4, .uv = {1,0}, .col = {0}};  

  gfx_push_colored_triangle(list, color,  v1,  v2, v4);
  gfx_push_colored_triangle(list, color, v2,  v3, v4);
}
void gfx_push_textured_quad(GfxCmdList *list, TilingAndOffset tilingAndOffset, Color clampColor, Color tint, Texture *tex, r32 width, r32 height, vec3 center, r32 rotation){

  //@TODO, batching. Try to bake as much data as possible into the vertex attributes to avoid setting uniforms and use sprite atlases to avoid texture switching. Bake tint into vertex colors
  // i think uploading unique vertices for each quad is preferable, compared to uploading one set of vertices then setting a unique model matrix for each sprite
  // 
  r32 hw = width/2;
  r32 hh = height/2;

  r32 s = sin(rotation);
  r32 c = cos(rotation);
	      

  
  vec3 p1 = vec3_zero;

  r32 x = -hw;
  r32 y = -hh;
  p1.x= x * c - (y * s);
  p1.y=x * s + y*c;
  p1 = vec3_add(p1,center);

  x = -hw;
  y = hh;
  vec3 p2 = vec3_zero;
  p2.x= x * c - (y * s);
  p2.y=x * s + y*c;
  p2 = vec3_add(p2,center);


  x = hw;
  y = hh;
  vec3 p3 = vec3_zero;
  p3.x= x * c - (y * s);
  p3.y=x * s + y*c;
  p3 = vec3_add(p3,center);

  x = hw;
  y = -hh;
  vec3 p4 = vec3_zero;
  p4.x= x * c - (y * s);
  p4.y=x * s + y*c;
  p4 = vec3_add(p4,center);




  Vertex v1 = {.pos = p1, .uv = {0,0}, .col = {0}};
  Vertex v2 = {.pos = p2, .uv = {0,1}, .col = {0}};
  Vertex v3 = {.pos = p3, .uv = {1,1}, .col = {0}};
  Vertex v4 = {.pos = p4, .uv = {1,0}, .col = {0}};

  
  gfx_push_textured_triangle(list, tilingAndOffset,clampColor, tint, tex,  v1,  v2, v4);
  gfx_push_textured_triangle(list, tilingAndOffset,clampColor,tint, tex, v2,  v3, v4);
}

void gfx_push_wire_rectangle(GfxCmdList *list, Color color, r32 width, r32 height, vec3 center, r32 thickness){

  Assert(False);
  // @TODO
  GfxCmdWireRectangle c = {
    .center = center,
    .width = width,
    .height = height,
    .thickness = thickness,
    .color = color,
  };
  GfxCmdWireRectangle *cmd=PushStructCustomAlign(list->arena, GfxCmdWireRectangle,GPU_CMD_ALIGNMENT);
  *cmd = c;
  cmd->header.type=GFX_CMD_WIRE_RECTANGLE;
  list->numCmds++;
}
void gfx_push_wire_circle(GfxCmdList *list, Color color, r32 width, r32 height, vec3 center, r32 thickness){
  // @TODO
  Assert(False);
}


void gfx_push_text(GfxCmdList *list, Font *font, string8 text, i32 sizePx, vec3 pos, TextAlignment alignment, Color color){
  GfxCmdText *textCmd=PushStructCustomAlign(list->arena, GfxCmdText, GPU_CMD_ALIGNMENT);
  textCmd->header.type = GFX_CMD_TEXT;
  textCmd->font = font;
  textCmd->pos = pos;
  textCmd->alignment = alignment;
  textCmd->color = color;
  textCmd->text = text;
  textCmd->sizePx = sizePx;

  
  list->numCmds++;
}

void gfx_push_set_matrix(GfxCmdList *list, GfxMatrixType type, mat4x4 mat){
  GfxCmdSetMatrix *setProj=PushStructCustomAlign(list->arena, GfxCmdSetMatrix, GPU_CMD_ALIGNMENT);
  setProj->mat = mat;
  setProj->type = type;
  setProj->header.type = GFX_CMD_SET_MATRIX;


  
  list->numCmds++;  
}
