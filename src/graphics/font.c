
r32 font_string_width_from_pixel_size(Font font, string8 string, i32 pixelSize){
  r32 scale = pixelSize/(r32)font.sizePx;

  return font_string_width(font, string, scale);
}
r32 font_string_width(Font font, string8 string, r32 scale){

  r32 scaleForPixelHeight=stbtt_ScaleForPixelHeight(&font.stbFont, font.sizePx*scale);
  i32 advanceTotal=0;


  u8 *str = string.ptr;


  for(int i=0;i<string.length;i++){
    i32 advance=0;
    i32 leftBearing = 0;
    stbtt_GetCodepointHMetrics(&font.stbFont, *str, &advance, &leftBearing);

    advanceTotal+=advance;
    str++;
  }

  return advanceTotal*scaleForPixelHeight;
  
}

r32 font_string_height(Font font, string8 string, r32 scale){
  return font.sizePx*scale;
}
r32 font_calc_height(Font font, r32 scale){

  return font.sizePx*scale;
  // stb already does this when baking the font, just return the baked size

  /* i32 ascent,descent,lineGap; */

  /* stbtt_GetFontVMetrics(&font.stbFont, &ascent, &descent, &lineGap); */

  /* r32 as=ascent*font.scaleForPixelHeight*font.scale; */
  /* r32 des=descent*font.scaleForPixelHeight*font.scale; */
  /* return as-des; */
}
