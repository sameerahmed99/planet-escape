
b32 r32_match_approx(r32 a, r32 b){
  return fabsf(a-b) < EPSILON;
}
Range_r32 range_r32(r32 min, r32 max){
  Range_r32 res= {min, max};
  if(min> max){
    Swap(r32, res.min, res.max);
  }
  return res;
}

Range_i64 range_i64(i64 min, i64 max){
  Range_i64 res= {min, max};
  if(min> max){
    Swap(i64, res.min, res.max);
  }
  return res;
}
Range_mi range_mi(mi min, mi max){
  Range_mi res= {min, max};
  if(min> max){
    Swap(mi, res.min, res.max);
  }
  return res;
}
u64 range_mi_length(Range_mi range){
  return (u64)range.max - (u64)range.min;
}
vec3 vec3_create(r32 x, r32 y, r32 z){
  vec3 v;
  v.x =x ;
  v.y = y;
  v.z = z;
  return v;
}
vec3 vec3_add(vec3 a, vec3 b){
  vec3 c;
  for(int i=0;i<3;i++){
    c.v[i] = a.v[i] + b.v[i];
  }
  return c;
}
vec3 vec3_sub(vec3 a, vec3 b){
  vec3 c;
  for(int i=0;i<3;i++){
    c.v[i] = a.v[i] - b.v[i];
  }
  return c;
}
vec3 vec3_scalef(vec3 a, r32 f){
  vec3 c;
  for(int i=0;i<3;i++){
    c.v[i] = a.v[i] * f;
  }
  return c;
}

b32 vec3_match(vec3 a, vec3 b){
  for(int i=0;i<3;i++){
    if(!r32_match_approx(a.v[i],b.v[i])) return False;
  }
  return True;
}



ivec3 ivec3_create(i32 x, i32 y, i32 z){
  ivec3 v = {x,y,z};
  return v;
}
mat4x4 mat4x4_projection_ortho(r32 right, r32 left, r32 top, r32 bottom, r32 near, r32 far){
  mat4x4 mat = {0};


  
  // near and far don't affect the objects, useful for other things probably
  
  mat.cr00 = 2 / (right-left);
  mat.cr11 = 2 / (top-bottom);
  mat.cr22 = -2 / (far-near);
  mat.cr33 = 1;

  mat.cr30 = -(right+left)/(right-left);
  mat.cr31 = -(top+bottom)/(top-bottom);
  mat.cr32 = -(far+near)/(far-near);
  mat.cr33 = 1;


  return mat;
}

mat4x4 mat4x4_translation(r32 x, r32 y, r32 z){
  mat4x4 mat = mat4x4_identity;

  mat.cr30 = x;
  mat.cr31 = y;
  mat.cr32 = z;

  return mat;
}
mat4x4 mat4x4_translate(mat4x4 current, r32 x, r32 y, r32 z){
  mat4x4 t = mat4x4_translation(x,y,z);
  current.cr30+=t.cr30;
  current.cr31+=t.cr31;
  current.cr32+=t.cr32;

  return current;
}

mat4x4 mat4x4_rotation_axis(vec3 axis, float radians){
  mat4x4 mat = mat4x4_identity;
  float c = (r32)cos(radians);
  float s = (r32)sin(radians);
  float oneMinusCos = 1 - c;

  float x = axis.x, y = axis.y, z=axis.z;
  float xSqr = x*x, ySqr = y*y, zSqr = z*z;
  
  mat.cr00 = xSqr*oneMinusCos +c;
  mat.cr01 = x*y*oneMinusCos - z*s;
  mat.cr02 = x*z*oneMinusCos + y*s;

  mat.cr10 = x*y*oneMinusCos +z*s;
  mat.cr11 = ySqr*oneMinusCos + c;
  mat.cr12 = y*z*oneMinusCos - x*s;

  mat.cr20 = x*z*oneMinusCos - y*s;
  mat.cr21 = y*z*oneMinusCos + x*s;
  mat.cr22 = zSqr*oneMinusCos + c;

    
  return mat;
  
}

b32 mat4x4_match(mat4x4 a, mat4x4 b){

  for(int i=0;i<16;i++){
    if(!r32_match_approx(a.v[i],b.v[i])) return False;
  }
  return True;
}
