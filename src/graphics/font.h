#ifndef FONT_CORE_INCLUDE
#define FONT_CORE_INCLUDE


typedef enum TextAlignment{
  TEXT_ALIGN_LEFT_TOP,
  TEXT_ALIGN_LEFT_MIDDLE,
  TEXT_ALIGN_LEFT_BOTTOM,
  TEXT_ALIGN_RIGHT_TOP,
  TEXT_ALIGN_RIGHT_MIDDLE,
  TEXT_ALIGN_RIGHT_BOTTOM,

  TEXT_ALIGN_CENTER_TOP,
  TEXT_ALIGN_CENTER_MIDDLE,
  TEXT_ALIGN_CENTER_BOTTOM,



} TextAlignment;

typedef struct Font Font;

struct Font{
  i32 sizePx;
  Texture *tex;
  stbtt_packedchar *stbCharData;
  stbtt_fontinfo stbFont;
};



r32 font_string_width(Font font, string8 string, r32 scale);
r32 font_string_width_from_pixel_size(Font font, string8 string, i32 pixelSize);
r32 font_string_height(Font font, string8 string, r32 scale);
r32 font_calc_height(Font font, r32 scale);



#endif // FONT_CORE_INCLUDE
