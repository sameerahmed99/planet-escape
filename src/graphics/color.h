#ifndef COLOR_CORE_INCLUDE
#define COLOR_CORE_INCLUDE

typedef vec4 Color;

global const Color color_white = {1,1,1,1};
global const Color color_zero = {0,0,0,0};
global const Color color_black = {0,0,0,1};

Color color_create(float r, float g, float b, float a);
u32 color_to_hex(Color col);
Color color_from_hex(i32 hex);

#endif // COLOR_CORE_INCLUDE
