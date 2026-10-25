#ifndef TEXTURE_INCLUDE
#define TEXTURE_INCLUDE

#define TEXTURE_NUM_CHANNELS 4

typedef struct TilingAndOffset{
  vec2 tiling;
  vec2 offset;
} TilingAndOffset;


typedef struct Texture{
  mi width, height;
  mi sizeBytes;
  mi channels;
  string8 data;
  mi rowStride;
  GfxTextureWrapMode wrapMode;
  GfxTextureFilteringMode filteringMode;
  GfxTexture gfxTex;
  Color clampColor;
} Texture;



const TilingAndOffset tiling_and_offset_identity = {.tiling={1,1}, .offset={0,0}};
// textures
Texture texture_create_empty(mi width, mi height, GfxTextureWrapMode wrapMode, GfxTextureFilteringMode filteringMode);
void texture_init_empty(Arena *arena,Texture *existing, string8 buffer, b32 generateMipmaps);

Texture texture_create(Arena *arena,mi width, mi height, Color fillColor, GfxTextureWrapMode wrapMode, GfxTextureFilteringMode filteringMode, b32 generateMipmaps);
Texture texture_create_from_buffer(Arena *arena,mi width, mi height, string8 buffer, GfxTextureWrapMode wrapMode, GfxTextureFilteringMode filteringMode, b32 generateMipmaps);
Texture texture_load_from_file(Arena *arena,string8 path, b32 flip, GfxTextureWrapMode wrapMode, GfxTextureFilteringMode filteringMode, b32 generateMipmaps);

void texture_free_data(Texture tex);





#endif // TEXTURE_INCLUDE
