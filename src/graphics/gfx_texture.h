#ifndef GFX_TEXTURE_INCLUDE
#define GFX_TEXTURE_INCLUDE




typedef enum GfxTextureWrapMode {
  GFX_TEXTURE_WRAP_MODE_REPEAT,
  GFX_TEXTURE_WRAP_MODE_MIRROR,
  GFX_TEXTURE_WRAP_MODE_CLAMP,
  GFX_TEXTURE_WRAP_MODE_CLAMP_COLORED,
} GfxTextureWrapMode;

typedef enum GfxTextureFilteringMode{
  GFX_TEXTURE_FILTERING_NEAREST,
  GFX_TEXTURE_FILTERING_BILINEAR,
} GfxTextureFilteringMode;

typedef struct GfxTexture GfxTexture;
struct GfxTexture{

  // for opengl, this id can work as is (since ids in opengl are GLuint)
  // for vulkan or other backends that don't work like this
  // maintain a list or arena in their state and map this id to their resources.
  u32 id;
  GfxTextureWrapMode wrapMode;
  GfxTextureFilteringMode filteringMode;

  // not available on renderer backends that don't require owner memory
  string8 gamesOwnerTextureMemory;
};

////////////////////////////////////////////////////
// ~sameer: to be implemented by underlying renderer
GfxTexture gfx_create_texture(mi width,mi height,
			      GfxTextureWrapMode wrapMode,
			      GfxTextureFilteringMode filteringMode,
			      b32 generateMipmaps,
			      string8 data);

#endif // GFX_TEXTURE_INCLUDE
