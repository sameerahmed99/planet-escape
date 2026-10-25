Texture texture_load_from_file(Arena *arena, string8 path, b32 flip, GfxTextureWrapMode wrapMode, GfxTextureFilteringMode filteringMode, b32 generateMipmaps){
  
  i32 width, height, nrChannels;
  stbi_set_flip_vertically_on_load(flip);

  unsigned char *data = stbi_load((char*)path.ptr, &width, &height, &nrChannels, 0);

  Assert(nrChannels == 4);

  Texture tex=texture_create_empty((mi)width, (mi)height, wrapMode, filteringMode);

  tex.data.ptr = arena_push(arena, width*height*nrChannels, sizeof(u8)*TEXTURE_NUM_CHANNELS,True);
  MemoryCopy(tex.data.ptr, data, tex.sizeBytes);

  GfxTexture gfxTex = gfx_create_texture(width,height, wrapMode, generateMipmaps, filteringMode,tex.data);
  tex.gfxTex = gfxTex;

  stbi_image_free(data);
  return tex;
}
void texture_free_data(Texture tex){

  _log("Not freeing tex data yet, implement it\n");
  //@TODO
  // we already free stb data now since we're using arena to copy the data to
  // find a proper way to release the data stored in the arena, maybe a bool in the load_from_file function "releaseDataAfterGpuTexCreated" and then using temp arena for releasing data after generating gfx tex
  
  /* if(tex.data.ptr){ */
  /*   stbi_image_free(tex.data.ptr); */
  /* } */
}
Texture texture_create_empty(mi width, mi height, GfxTextureWrapMode wrapMode, GfxTextureFilteringMode filteringMode){
  Texture tex = {0};

  tex.width = (mi)width;
  tex.height = (mi)height;
  tex.sizeBytes = (mi)(width*height*4);
  tex.channels = (mi)4;
  tex.rowStride = tex.channels * width;
  tex.wrapMode = wrapMode;
  tex.filteringMode = filteringMode;

  return tex;
}

void texture_init_empty(Arena *arena, Texture *tex, string8 buffer, b32 generateMipmaps){


  void* data = arena_push(arena, tex->sizeBytes, sizeof(u8)*TEXTURE_NUM_CHANNELS,True);
  tex->data = str8((u8*)data, tex->sizeBytes);
  MemoryCopy(data, buffer.ptr, tex->sizeBytes);

  GfxTexture gfxTex = gfx_create_texture(tex->width,tex->height, tex->wrapMode, generateMipmaps, tex->filteringMode,tex->data);
  tex->gfxTex = gfxTex;
}



Texture texture_create(Arena *arena, mi width, mi height, Color fillColor, GfxTextureWrapMode wrapMode, GfxTextureFilteringMode filteringMode, b32 generateMipmaps){
  Texture tex = texture_create_empty(width, height, wrapMode, filteringMode);

  void* data = arena_push(arena, tex.sizeBytes, sizeof(u8)*TEXTURE_NUM_CHANNELS,True);
  tex.data = str8((u8*)data, tex.sizeBytes);

  u8 *row = tex.data.ptr;
  u32 hex = color_to_hex(fillColor);


  for(int y=0;y<tex.height;y++){
    for(int x=0;x<tex.width;x++){
      u32 *p = (u32*)row + x;
      *p = hex;
    }
    row+=tex.rowStride;
  }



  GfxTexture gfxTex = gfx_create_texture(width,height, wrapMode, filteringMode,generateMipmaps, tex.data);
  tex.gfxTex = gfxTex;

  return tex;
}

Texture texture_create_from_buffer(Arena *arena,mi width, mi height, string8 buffer, GfxTextureWrapMode wrapMode, GfxTextureFilteringMode filteringMode, b32 generateMipmaps){


  Texture tex = texture_create_empty(width,height, wrapMode, filteringMode);
  // 1 byte alignment, no problem it's just an array of bytes
  tex.data.ptr = arena_push(arena, width*height*tex.channels, sizeof(u8)*TEXTURE_NUM_CHANNELS,True);

  MemoryCopy(tex.data.ptr, buffer.ptr, tex.sizeBytes);


  GfxTexture gfxTex = gfx_create_texture(width,height, wrapMode, generateMipmaps, filteringMode,tex.data);
  tex.gfxTex = gfxTex;

  return tex;
}
