#include "string.h"


string8 str8(u8 *str, mi length){
  string8 s = {.ptr=str, .length=length};
  return s;
}

mi cstring8_length(u8 *cstr){

  mi length = 0;
  if(cstr){
    u8 *curr=cstr;

    //////////////////////////////////////////////////////////////////////////
    // ~sameer: @note, no for loop body. I saw this in raddbg code, neat trick

    for(;*curr!=0;curr+=1);
    
    length = curr - cstr;

  }    
  return length;
}
string8 str8cat(Arena *arena, string8 a, string8 b){
  string8 res;
  res.length = a.length + b.length;
  res.ptr=PushArrayNoZero(arena, u8, res.length+1);

  MemoryCopy(res.ptr, a.ptr, a.length);
  MemoryCopy(res.ptr+a.length, b.ptr, b.length);

  res.ptr[res.length] = 0;

  return res;
}
string8 i32_to_string8(Arena *arena, i32 val){
  i32 maxDigits = 15;

  string8 str;
  str.ptr=PushArray(arena, u8, maxDigits);


  //@NOTE, snprintf null terminates automatically, no need to zero the end manually
  snprintf(str.ptr,maxDigits,"%d", val);


  str.length = cstring8_length(str.ptr);
  return str;
}



i32 string8_to_i32(string8 s){
  // https://stackoverflow.com/questions/7021725/how-can-i-convert-a-string-to-integer-in-c

  if (s.ptr[0] == '\0' || isspace(s.ptr[0]))
    return INT32_MIN;
  errno = 0;

  char nullTerminated[11];
  MemoryCopy(nullTerminated, s.ptr, s.length);
  nullTerminated[10] = 0;

  char *end;  
  long l = strtol(nullTerminated, &end, 10);

  
  if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
    return INT32_MIN;
  if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
     return INT32_MIN;
  if (*end != '\0')
    return INT32_MIN;
  return (i32)l;
}


// r32

string8 r32_to_string8(Arena *arena, r32 val){
  r32 maxDigits = 51;

  string8 str;
  str.ptr=PushArray(arena, u8, maxDigits+1);
  snprintf(str.ptr,maxDigits,"%f", val);
  str.length = cstring8_length(str.ptr);
  return str;
}



r32 string8_to_r32(string8 s){
  if (s.ptr[0] == '\0' || isspace(s.ptr[0]))
    return 0;

  char nullTerminated[50];
  MemoryCopy(nullTerminated, s.ptr, s.length);
  nullTerminated[49] = 0;

  char *end;  
  r32 l = strtof(nullTerminated, &end);

  return (r32)l;
}
