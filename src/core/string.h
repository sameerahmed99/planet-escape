#ifndef STRING_SYS_INCLUDE
#define STRING_SYS_INCLUDE


typedef struct string8{
  u8 *ptr;
  mi length;
} string8;


string8 str8(u8 *str, mi length);
mi cstring8_length(u8 *cstr);
string8 str8cat(Arena *arena, string8 a, string8 b);
string8 i32_to_string8(Arena *arena, i32 val);
i32 string8_to_i32(string8 s);
string8 r32_to_string8(Arena *arena, r32 val);
r32 string8_to_r32(string8 s);
// lit = literal
#define str8_lit(s) str8((u8*)(s), sizeof(s)-1)

// c string, includes null terminator
#define str8_litc(s) str8((u8*)(s), sizeof(s))
#define str8_from_cstring8(cs) str8((u8*)(cs), cstring8_length(cs))
#endif // STRING_SYS_INCLUDE
