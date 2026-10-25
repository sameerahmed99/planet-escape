

Color color_create(float r, float g, float b, float a){
  Color c= {r,g,b,a};

  return c;
}


u32 color_to_hex(Color col){
  u32 hex;
  u32 r = (u32)(Clamp01(col.r)*255);
  u32 g = (u32)(Clamp01(col.g)*255);
  u32 b = (u32)(Clamp01(col.b)*255);
  u32 a =  (u32)(Clamp01(col.a)*255);
  hex = (r << 24) | (g << 16) | (b << 8) | (a);

  return hex;
}


Color color_from_hex(i32 hex){
  u32 r = (hex>>24) & 0xFF;
  u32 g = (hex>>16) & 0xFF;
  u32 b = (hex>>8) & 0xFF;
  u32 a = hex & 0xFF;

  Color c=color_create(r/255.0f, g/255.0f, b/255.0f,a/255.0f);
  return c;
}
