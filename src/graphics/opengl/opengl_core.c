#include "opengl_core.h"

#if OS_WINDOWS
#include "./windows/win32_gl.c"

#else
#error where de foak are we (opengl_core.h unknown os)

#endif


internal const char *gl_VersionSrc = "#version 330 core\n";
internal const char *gl_AlphaClippingSrc = "#define G_ALPHA_CLIPPING 1\n";
internal GLShader gl_ShaderFlatNoTransform;
internal GLShader gl_ShaderTexturedNoTransform;

// @TODO, wire debug stuff
/* internal GLShader gl_ShaderWireRectangle; */
/* internal GLShader gl_ShaderWireCircle; */
/* internal GLShader gl_ShaderFont; */

internal GLShader gl_ShaderDefault;


void gl_clear_texture_binds(){
  gl_State.textureIsBound =False;
  gl_State.numActiveTextures = 0;
}

void gl_update_shader_matrices(GLShader shader){
  GLint m = shader.u_model;
  GLint v = shader.u_view;
  GLint p = shader.u_projection;

  mat4x4 lm = gl_State.lastModelMatAssigned;
  mat4x4 lv = gl_State.lastViewMatAssigned;
  mat4x4 lp = gl_State.lastProjectionMatAssigned;

  if(!mat4x4_match(gl_State.modelMatrix,lm)){
    glUniformMatrix4fv(m, 1, GL_FALSE, gl_State.modelMatrix.v);
    gl_State.lastModelMatAssigned = gl_State.modelMatrix;
  }
  
  if(!mat4x4_match(gl_State.viewMatrix,lv)){
    glUniformMatrix4fv(v, 1, GL_FALSE, gl_State.viewMatrix.v);
    gl_State.lastViewMatAssigned = gl_State.viewMatrix;
  }


  if(!mat4x4_match(gl_State.projectionMatrix,lp)){
    glUniformMatrix4fv(p, 1, GL_FALSE, gl_State.projectionMatrix.v);
    gl_State.lastProjectionMatAssigned = gl_State.projectionMatrix;
  }

}

//@TODO, handle window resizing with glViewport

///////////////////////////////
// ~sameer: opengl_core.h funcs
internal void gl_init(Window window){


  
  gl_os_init(window);


  window_get_size(window, &gl_State.windowWidth, &gl_State.windowHeight);
  gl_State.halfWindowWidth = gl_State.windowWidth/2.0f;
  gl_State.halfWindowHeight = gl_State.windowHeight/2.0f;

  //https://registry.khronos.org/OpenGL-Refpages/gl4/html/glViewport.xhtml
  // read how this is resolved to understand why we keep it at 0,0
  glViewport(0,0, gl_State.windowWidth, gl_State.windowHeight);
  // uncomment this example to see how it works
  // glViewport(50,50, gl_State.windowWidth/2.0f, gl_State.windowHeight/2.0f);

  /* GLint info[4]; */
  /* glGetIntegerv(GL_VIEWPORT, info); */
  
  gl_State.arena =arena_create(MB(5), MB(50));
  glEnable(GL_BLEND);
  glDepthMask(GL_TRUE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  
// t for temp
#define t(name,ret,p) name = (func_type_##name*)gl_get_func_address(#name)
  OPENGL_FUNCS
#undef t


  // default shader
  {

    TempArena temp = arena_temp_begin(gl_State.arena);

    //@TODO, temporary raw paths until we have asset system
    string8 tempStr = str8_lit("../assets/shaders/gl/default.vert");
    string8 vertSrc = file_read_from_path_into_buffer(temp.arena, tempStr);
    tempStr = str8_lit("../assets/shaders/gl/default.frag");
    string8 fragSrc = file_read_from_path_into_buffer(temp.arena, tempStr);
    GLuint shader_default=gl_create_full_shader_from_sources(vertSrc, fragSrc,False);
    gl_ShaderDefault.id = shader_default;



    gl_ShaderDefault.u_color = glGetUniformLocation(gl_ShaderDefault.id, "u_color");
    gl_ShaderDefault.u_model = glGetUniformLocation(gl_ShaderDefault.id, "u_model");
    gl_ShaderDefault.u_view = glGetUniformLocation(gl_ShaderDefault.id, "u_view");
    gl_ShaderDefault.u_projection = glGetUniformLocation(gl_ShaderDefault.id, "u_projection");
    gl_ShaderDefault.u_tiling = glGetUniformLocation(gl_ShaderDefault.id, "u_tiling");
    gl_ShaderDefault.u_offset = glGetUniformLocation(gl_ShaderDefault.id, "u_offset");

  }

    
    // flat shader
  {
    TempArena temp = arena_temp_begin(gl_State.arena);

    //@FIXME, temporary raw paths until we have asset system
    string8 tempStr = str8_lit("../assets/shaders/gl/flat_no_transform.vert");
    string8 vertSrc = file_read_from_path_into_buffer(temp.arena, tempStr);
    tempStr = str8_lit("../assets/shaders/gl/flat_no_transform.frag");
    string8 fragSrc = file_read_from_path_into_buffer(temp.arena, tempStr);
    GLuint shader_flat_no_transform=gl_create_full_shader_from_sources(vertSrc, fragSrc, False);
    gl_ShaderFlatNoTransform.id = shader_flat_no_transform;
    gl_ShaderFlatNoTransform.u_color = glGetUniformLocation(gl_ShaderFlatNoTransform.id, "u_color");
  }
  // textured shader
  {

    TempArena temp = arena_temp_begin(gl_State.arena);

    //@FIXME, temporary raw paths until we have asset system
    string8 tempStr = str8_lit("../assets/shaders/gl/textured_no_transform.vert");
    string8 vertSrc = file_read_from_path_into_buffer(temp.arena, tempStr);
    tempStr = str8_lit("../assets/shaders/gl/textured_no_transform.frag");
    string8 fragSrc = file_read_from_path_into_buffer(temp.arena, tempStr);
    GLuint shader_textured_no_transform=gl_create_full_shader_from_sources(vertSrc, fragSrc, False);
    gl_ShaderTexturedNoTransform.id = shader_textured_no_transform;

    gl_ShaderTexturedNoTransform.u_color = glGetUniformLocation(gl_ShaderTexturedNoTransform.id, "u_color");
  }

  
  
  
  glGenVertexArrays(1, &gl_State.vertArray);
  glGenBuffers(1, &gl_State.vertBuffer);
  glGenBuffers(1, &gl_State.indicesBuffer);

  glBindVertexArray(gl_State.vertArray);

  glBindBuffer(GL_ARRAY_BUFFER,gl_State.vertBuffer);
  glBufferData(GL_ARRAY_BUFFER,GFX_VERTEX_BUFFER_SIZE, 0, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_State.indicesBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, GFX_INDICES_BUFFER_SIZE, 0, GL_DYNAMIC_DRAW);



  glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2,4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col));
  glEnableVertexAttribArray(2);


  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);




  
}

internal GLuint gl_create_and_compile_shader(string8 src,GLuint type, b32 alphaClipping){
  int success;
  char infoLog[512];

  GLuint id = glCreateShader(type);

  const char *str = (const char *)src.ptr;
  const char *alphaClippingSrc;;
  if(alphaClipping){
    alphaClippingSrc = gl_AlphaClippingSrc;    
  }
  else{
    alphaClippingSrc = "";
  }
  const char *srcs[] = {gl_VersionSrc, alphaClippingSrc, str};
  int lengths[] = {strlen(gl_VersionSrc), strlen(alphaClippingSrc), src.length};
  glShaderSource(id, 3, srcs, lengths);
  glCompileShader(id);
  // check for shader compile errors
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success)
    {
      glGetShaderInfoLog(id, 512, NULL, infoLog);
      _log("gl: Couldn't compile %s shader of id %u: %s\n", type == GL_VERTEX_SHADER ? "vertex" : "fragment", id, infoLog);
      Assert(False);
    }

  return id;
}
GLuint gl_create_and_link_shaders(GLuint vert, GLuint frag, b32 deleteVertAndFrag){
  int success;
  char infoLog[512];
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vert);
  glAttachShader(shaderProgram, frag);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    _log("gl: Couldn't link shader programs: %s\n",infoLog);
    Assert(False);
  }
  if(deleteVertAndFrag){
    glDeleteShader(vert);
    glDeleteShader(frag);
  }
  return shaderProgram;
}

internal GLuint gl_create_full_shader_from_sources(string8 vertSrc, string8 fragSrc, b32 alphaClipping){
  GLuint vert= gl_create_and_compile_shader(vertSrc,GL_VERTEX_SHADER, alphaClipping);
  GLuint frag= gl_create_and_compile_shader(fragSrc,GL_FRAGMENT_SHADER, alphaClipping);
  GLuint shader = gl_create_and_link_shaders(vert, frag, True);
  return shader;
}

internal void gl_bind_shader(GLShader shader){
  if(gl_State.shaderIsBound && gl_State.boundShader.id == shader.id){

  }
  else{
    gl_State.boundShader = shader;
    gl_State.shaderIsBound = True;
    glUseProgram(shader.id);
  }
}

internal void gl_bind_texture(GfxTexture texture){
  if(gl_State.textureIsBound && gl_State.boundTexture.id == texture.id){

  }
  else{

    glActiveTexture(GL_TEXTURE0 + gl_State.numActiveTextures);
    gl_State.numActiveTextures+=1;

    
    gl_State.boundTexture = texture;
    gl_State.textureIsBound = True;
    glBindTexture(GL_TEXTURE_2D, texture.id);




  }
}

void gl_exec_clear(GfxCmdClear *cmd){
  Color c = cmd->color;
  glClearColor(c.r,c.g,c.b,c.a);
  //glClearColor(1.0f,0.0f,0.0f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void gl_push_mesh_data(Vertex *verts, i32 *indices, mi numVerts, mi numIndices){
  mi size = sizeof(Vertex)*numVerts;
  mi sizeIndices = sizeof(i32)*numIndices;


  glBindBuffer(GL_ARRAY_BUFFER,gl_State.vertBuffer);
  glBufferSubData(GL_ARRAY_BUFFER, gl_State.currentVerticesOffsetBytes,size,verts);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_State.indicesBuffer);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, gl_State.currentIndicesOffsetBytes,sizeIndices,indices);
  glDrawElementsBaseVertex(GL_TRIANGLES,numIndices,GL_UNSIGNED_INT, (void*)gl_State.currentIndicesOffsetBytes, gl_State.currentVerticesOffset);

  gl_State.currentIndicesOffset+=numIndices;
  gl_State.currentIndicesOffsetBytes+=sizeIndices;
  gl_State.currentVerticesOffset+=numVerts;
  gl_State.currentVerticesOffsetBytes+=size;

}
void gl_push_material(Texture *tex, TilingAndOffset toff, GLShader shader, Color color){


  gl_bind_texture(tex->gfxTex);


  gl_bind_shader(shader);
  gl_update_shader_matrices(shader);

  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, tex->clampColor.v); 
  glProgramUniform4f(shader.id,shader.u_color, color.r, color.g, color.b, color.a);
  glProgramUniform2f(shader.id,shader.u_tiling, toff.tiling.x,toff.tiling.y);
  glProgramUniform2f(shader.id,shader.u_offset, toff.offset.x,toff.offset.y);


  gl_clear_texture_binds();
}
void gl_exec_textured_triangle(GfxCmdTexturedTriangle *cmd){



  
  Vertex verts[] = {cmd->v1, cmd->v2, cmd->v3};
  i32 indices[] = {0,1,2};
  gl_push_material(cmd->texture, cmd->tilingOff, gl_ShaderDefault, cmd->color);
  gl_push_mesh_data(verts, indices, 3, 3);

  /* mi numVerts = 3; */
  /* mi numIndices = 3; */

  /* mi size = sizeof(Vertex)*numVerts; */
  /* mi sizeIndices = sizeof(i32)*numIndices; */

  /* gl_bind_texture(cmd->texture->gfxTex); */


  /* gl_bind_shader(gl_ShaderDefault); */
  /* gl_update_shader_matrices(gl_ShaderDefault); */

  /* glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, cmd->clampColor.v);  */
  /* glProgramUniform4f(gl_ShaderDefault.id,gl_ShaderDefault.u_color, cmd->color.r, cmd->color.g, cmd->color.b, cmd->color.a); */
  /* glProgramUniform2f(gl_ShaderDefault.id,gl_ShaderDefault.u_tiling, cmd->tilingOff.tiling.x,cmd->tilingOff.tiling.y); */
  /* glProgramUniform2f(gl_ShaderDefault.id,gl_ShaderDefault.u_offset, cmd->tilingOff.offset.x,cmd->tilingOff.offset.y); */

  /* glBindBuffer(GL_ARRAY_BUFFER,gl_State.vertBuffer); */
  /* glBufferSubData(GL_ARRAY_BUFFER, gl_State.currentVerticesOffsetBytes,size,verts); */

  /* glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_State.indicesBuffer); */
  /* glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, gl_State.currentIndicesOffsetBytes,sizeIndices,indices); */
  /* glDrawElementsBaseVertex(GL_TRIANGLES,numIndices,GL_UNSIGNED_INT, (void*)gl_State.currentIndicesOffsetBytes, gl_State.currentVerticesOffset); */

  /* gl_State.currentIndicesOffset+=numIndices; */
  /* gl_State.currentIndicesOffsetBytes+=sizeIndices; */
  /* gl_State.currentVerticesOffset+=numVerts; */
  /* gl_State.currentVerticesOffsetBytes+=size; */

  /* gl_clear_texture_binds(); */
}


void gl_exec_colored_triangle(GfxCmdColoredTriangle *cmd){

  Vertex verts[] = {cmd->v1, cmd->v2, cmd->v3};
  i32 indices[] = {0,1,2};
  

  mi numVerts = 3;
  mi numIndices = 3;

  mi size = sizeof(Vertex)*numVerts;
  mi sizeIndices = sizeof(i32)*numIndices;

  gl_bind_shader(gl_ShaderFlatNoTransform);
  gl_update_shader_matrices(gl_ShaderFlatNoTransform);
  
  glProgramUniform4f(gl_ShaderFlatNoTransform.id,gl_ShaderFlatNoTransform.u_color, cmd->color.r, cmd->color.g, cmd->color.b, cmd->color.a);

  glBindBuffer(GL_ARRAY_BUFFER,gl_State.vertBuffer);
  glBufferSubData(GL_ARRAY_BUFFER, gl_State.currentVerticesOffsetBytes,size,verts);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_State.indicesBuffer);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, gl_State.currentIndicesOffsetBytes,sizeIndices,indices);
  glDrawElementsBaseVertex(GL_TRIANGLES,numIndices,GL_UNSIGNED_INT, (void*)gl_State.currentIndicesOffsetBytes, gl_State.currentVerticesOffset);

  gl_State.currentIndicesOffset+=numIndices;
  gl_State.currentIndicesOffsetBytes+=sizeIndices;
  gl_State.currentVerticesOffset+=numVerts;
  gl_State.currentVerticesOffsetBytes+=size;

}


void gl_exec_wire_rectangle(GfxCmdWireRectangle *wireRect){

}

void gl_exec_text(GfxCmdText *text){



  //@NOTE, this doesn't care about projection or screen space
  // so make sure you set the expected matrix before pushing text cmds
  // to render in screen space, just create an orthographic projection matrix that matches screen dimensions

  TilingAndOffset toff = {.tiling = {1,1}, .offset={0,0}};
  gl_push_material(text->font->tex, toff, gl_ShaderDefault, text->color);



  r32 scale = text->sizePx / (r32)text->font->sizePx;
  i32 width = font_string_width(*text->font, text->text,scale);
  i32 height = font_string_height(*text->font,text->text, scale);;
  r32 hw = width/2.0f;
  r32 hh = height/2.0f;

  u8 *str = text->text.ptr;
  r32 x = text->pos.x - hw;
  r32 y = text->pos.y - hh/2.0f;
    
  i32 texWidth = text->font->tex->width;
  i32 texHeight = text->font->tex->height;





  // @NOTE, @WARNING
  // dependent on stbs GetPackedQuad function, which by default will cause our text to render with pivot on the bottom left of the first character


  // where is the text->pos on the text? is it at the bottom left of the string? top right? center middle? 
  vec3 start = text->pos;
  r32 centerX = start.x-hw;
  r32 leftX = start.x;
  r32 rightX = start.x-width;
  
  r32 middleY = start.y-hh/2.0f;
  r32 topY = start.y-hh;
  r32 bottomY = start.y;


  
  switch(text->alignment){
  case TEXT_ALIGN_LEFT_TOP:{
    start.x=leftX;
    start.y = topY;
  } break;

  case TEXT_ALIGN_LEFT_MIDDLE:{
    start.x=leftX;
    start.y=middleY;
  } break;
  case TEXT_ALIGN_LEFT_BOTTOM:{
    start.x=leftX;
    start.y=bottomY;
  } break;
  case TEXT_ALIGN_RIGHT_TOP:{
    start.x = rightX;
    start.y = topY;
  } break;

  case TEXT_ALIGN_RIGHT_MIDDLE:{
    start.x = rightX;
    start.y = middleY;
  } break;
  case TEXT_ALIGN_RIGHT_BOTTOM:{
    start.x=rightX;
    start.y=bottomY;;
  } break;
    
  case TEXT_ALIGN_CENTER_TOP:{
    start.x=centerX;
    start.y=topY;
  } break;

  case TEXT_ALIGN_CENTER_MIDDLE:{
    start.x=centerX;
    start.y=middleY;
  } break;
  case TEXT_ALIGN_CENTER_BOTTOM:{
    start.x = centerX;
    start.y = bottomY;
  } break;
    
  default:{
    _log("Text alignment bad enum value\n");
    Assert(False);
  } break;
  }


  r32 relX=0,relY=0;
  for(int i=0;i<text->text.length;i++){


  
    
    stbtt_aligned_quad q;
    //@TODO, use position params in GfxCmdText
    // stb: top left is origin, adjust accordingly.

    // subtract 32 because we skip the first 32 unprintable characters when baking textures. baked_chars[0] stores info for [32]
    stbtt_GetPackedQuad(text->font->stbCharData, texWidth , texHeight,  // same data as above
			*str-32,             // character to display
			&relX, &relY,   // pointers to current position in screen pixel space
			&q,      // output: quad to draw
			1);


    //@TODO, font size scaling without messing up positions
    Vertex verts[] = {
      {.pos = {q.x0*scale,q.y0*scale,0.0f}, .uv={q.s0,1-q.t0}, .col={1,1,1,1}},
      {.pos = {q.x0*scale,q.y1*scale,0.0f}, .uv={q.s0,1-q.t1}, .col={1,1,1,1}},
      {.pos = {q.x1*scale,q.y1*scale,0.0f}, .uv={q.s1,1-q.t1}, .col={1,1,1,1}},
      {.pos = {q.x1*scale,q.y0*scale,0.0f}, .uv={q.s1,1-q.t0}, .col={1,1,1,1}},
    };

    for(int i=0;i<4;i++){
      verts[i].pos.y*=-1.0f;
      verts[i].pos.x+=start.x;
      verts[i].pos.y+=start.y;
    }
    // from stb_truetype.h regarding font going beyond left side of the expected bounding box:
    //  Current point:
    //    Set the current point where the first character will appear. The
    //    first character could extend left of the current point; this is font
    //    dependent. You can either choose a current point that is the leftmost
    //    point and hope, or add some padding, or check the bounding box or
    //    left-side-bearing of the first character to be displayed and set
    //    the current point based on that.

    /* for(int v=0;v<4;v++){ */
    /*   verts[v].pos.y =  - verts[v].pos.y ; */
    /* } */


    gl_push_material(text->font->tex, toff, gl_ShaderDefault, text->color);
    gl_push_mesh_data(verts, QuadIndices, 4, 6);

    str++;
  }


}

void gl_exec_set_matrix(GfxCmdSetMatrix *setProj){

  string8 u;
  switch(setProj->type){
  case GFX_MATRIX_TYPE_PROJECTION:{
    gl_State.projectionMatrix = setProj->mat;
    //u = str8_lit("u_projection");
  } break;
  case GFX_MATRIX_TYPE_VIEW:{
    gl_State.viewMatrix = setProj->mat;
    //u = str8_lit("u_view");
  }break;
  case GFX_MATRIX_TYPE_MODEL:{
    gl_State.modelMatrix = setProj->mat;
    //u = str8_lit("u_model");
  }break;
  default:{
    _log("Unidentified gfx matrix type: %d\n", setProj->type);
    Assert(False);
  } break;

  }
}



// implementing graphics.h

mi gl_align_step_size(mi stepSize){
  stepSize=  (stepSize + GPU_CMD_ALIGNMENT - 1) & ~(GPU_CMD_ALIGNMENT - 1);
  return stepSize;
}
void gfx_underlying_exec_list(GfxCmdList *list){

////////////////////////
// ~sameer: opengl reset
  gl_State.currentIndicesOffset=0;
  gl_State.currentIndicesOffsetBytes=0;
  gl_State.currentVerticesOffset=0;
  gl_State.currentVerticesOffsetBytes=0;
  gl_State.shaderIsBound = False;
  //gl_State.boundShader = {0};
  gl_clear_texture_binds();

  gl_State.projectionMatrix = mat4x4_identity;
  gl_State.viewMatrix = mat4x4_identity;
  gl_State.modelMatrix = mat4x4_identity;

  // @explanation: explicitly set to zero so that they mismatch from the matrices and get set for the first time to identity matrix in the uniforms
  gl_State.lastProjectionMatAssigned = mat4x4_zero;
  gl_State.lastViewMatAssigned = mat4x4_zero;
  gl_State.lastModelMatAssigned = mat4x4_zero;

  //gl_State.boundTexture = {0};
  //@TEMP, @HACK
  // buffer orphaning, as it's called
  // if this becomes a problem due to sync issues with gpu
  // consider using 3 buffers instead and just overwrite them like a ring buffer (you choose one of these buffers per frame, then the next in the next frame and so on)
  // so no need to orphan them in that case
  // otherwise, look up proper ways to synchronize 
  glBindBuffer(GL_ARRAY_BUFFER,gl_State.vertBuffer);
  glBufferData(GL_ARRAY_BUFFER,GFX_VERTEX_BUFFER_SIZE, 0, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_State.indicesBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, GFX_INDICES_BUFFER_SIZE, 0, GL_DYNAMIC_DRAW);

  
  glBindVertexArray(gl_State.vertArray);



  
  Arena *a = list->arena;
  u8 *base = arena_get(a, 0);

  

  for(int i=0;i<list->numCmds;i++){
    GfxCmdHeader *head=(GfxCmdHeader*)base;
    mi stepSize;
    switch(head->type){
    case GFX_CMD_CLEAR:{
      stepSize = sizeof(GfxCmdClear);

      // 15 val, align to multiple of 16
      // 00001111
      // 1. 15 + 16 -1 = 30 = 00011110
      // 2. 30 & ~(16-1) = 30 & ~15 = 30 & 11110000
      // 3. 30 & ~15 = 00011110 & 11110000  = 00010000 = 16
      stepSize = gl_align_step_size(stepSize);
      GfxCmdClear *clear = (GfxCmdClear*)head;
      gl_exec_clear(clear);
    } break;
    case GFX_CMD_TEXTURED_TRIANGLE:{
      stepSize = sizeof(GfxCmdTexturedTriangle);
      stepSize = gl_align_step_size(stepSize);
      GfxCmdTexturedTriangle *texturedTri = (GfxCmdTexturedTriangle*)head;
      gl_exec_textured_triangle(texturedTri);
    } break;
    case GFX_CMD_COLORED_TRIANGLE:{
      stepSize = sizeof(GfxCmdColoredTriangle);
      stepSize = gl_align_step_size(stepSize);
      GfxCmdColoredTriangle *coloredTri = (GfxCmdColoredTriangle*)head;
      gl_exec_colored_triangle(coloredTri);
    }break;
    case GFX_CMD_TEXT:{
      stepSize = sizeof(GfxCmdText);
      stepSize = gl_align_step_size(stepSize);

      GfxCmdText *cmdText =(GfxCmdText*)head;
      gl_exec_text(cmdText);
    } break;
    case GFX_CMD_SET_MATRIX:{
      stepSize = sizeof(GfxCmdSetMatrix);
      stepSize = gl_align_step_size(stepSize);

      GfxCmdSetMatrix *setProj =(GfxCmdSetMatrix*)head;
      gl_exec_set_matrix(setProj);
    } break;
    default :{
      Assert(False);
    } break;
    }

    base+=stepSize;
  }

}

GfxTexture gfx_create_texture(mi width,mi height,
			      GfxTextureWrapMode _wrapMode,
			      GfxTextureFilteringMode _filteringMode,
			      b32 generateMipmaps,
			      string8 data){
  GfxTexture tex = {
    .id=0,
    .wrapMode = _wrapMode,
    .filteringMode = _filteringMode,
    .gamesOwnerTextureMemory = data
  };
  glGenTextures(1,&tex.id);
  glBindTexture(GL_TEXTURE_2D, tex.id);

  
  GLenum wrapMode, filter;
  switch(_wrapMode){
  case  GFX_TEXTURE_WRAP_MODE_REPEAT:{
    wrapMode = GL_REPEAT;
  } break;
  case GFX_TEXTURE_WRAP_MODE_MIRROR:{
    wrapMode = GL_MIRRORED_REPEAT;
  } break;
  case GFX_TEXTURE_WRAP_MODE_CLAMP:{
    wrapMode = GL_CLAMP_TO_EDGE;
  } break;
  case GFX_TEXTURE_WRAP_MODE_CLAMP_COLORED:{
    wrapMode = GL_CLAMP_TO_BORDER;
  } break;
  default:{
    _log("Unknown texture wrap mode: %d\n", wrapMode);
    Assert(False);
  } break;
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);


  switch(_filteringMode){
  case GFX_TEXTURE_FILTERING_NEAREST:{
    filter = GL_NEAREST;
  } break;
  case GFX_TEXTURE_FILTERING_BILINEAR:{
    filter = GL_LINEAR;
  } break;
  default:{
    _log("Unknown texture filtering mode: %d\n", _filteringMode);
    Assert(False);
  } break;
  };
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);



  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.ptr);

  if(generateMipmaps){
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  


  return tex;
}

b32 gfx_renderer_requires_textures_to_be_kept_on_cpu_memory(){
  return False;
}


void gfx_set_swap_interval(i32 val){
  gl_swap_interval(val);
}
