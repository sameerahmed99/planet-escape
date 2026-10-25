#ifndef MATH_CORE_INCLUDE
#define MATH_CORE_INCLUDE


const r32 PIE = 3.14159;
const r32 EPSILON = 0.00001f;

b32 r32_match_approx(r32 a, r32 b);

/////////////////////////////////////////////////////////////////////////////////
// ~sameer: ranges

typedef union Range_r32 Range_r32;
union Range_r32
{
  struct
  {
    r32 min;
    r32 max;
  };
  r32 v[2];
};

typedef union Range_i64 Range_i64;
union Range_i64
{
  struct
  {
    i64 min;
    i64 max;
  };
  i64 v[2];
};



typedef union Range_mi Range_mi;
union Range_mi{
  struct
  {
    mi min;
    mi max;
  };
  mi v[2];
};

Range_i64 range_i64(i64 min, i64 max);
Range_mi range_mi(mi min, mi max);
u64 range_mi_length(Range_mi range);



///////////////////
// ~sameer: vector4
typedef union vec4 vec4;
union vec4
{
  struct{
    r32 x;
    r32 y;
    r32 z;
    r32 w;
  };
  struct{
    r32 r;
    r32 g;
    r32 b;
    r32 a;
  };
  r32 v[4];
};

typedef union ivec4 ivec4;
union ivec4
{
  struct{
    i32 x;
    i32 y;
    i32 z;
    i32 w;
  };
  i32 v[4];
};




///////////////////
// ~sameer: vector3
typedef union vec3 vec3;
union vec3
{
  struct{
    r32 x;
    r32 y;
    r32 z;
  };
  r32 v[3];
};

global const vec3 vec3_zero = {0,0,0};
global const vec3 vec3_one = {1,1,1};
global const vec3 vec3_right = {1,0,0};
global const vec3 vec3_left = {-1,0,0};
global const vec3 vec3_up = {0,1,0};
global const vec3 vec3_down = {0,-1,0};
global const vec3 vec3_forward = {0,0,1};
global const vec3 vec3_backward = {0,0,-1};


  

typedef union ivec3 ivec3;
union ivec3
{
  struct{
    i32 x;
    i32 y;
    i32 z;

  };
  i32 v[3];
};


vec3 vec3_create(r32 x, r32 y, r32 z);
vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_sub(vec3 a, vec3 b);
vec3 vec3_scalef(vec3 a, r32 f);
b32 vec3_match(vec3 a, vec3 b);
ivec3 ivec3_create(i32 x, i32 y, i32 z);

///////////////////
// ~sameer: vector2
typedef union vec2 vec2;
union vec2
{
  struct{
    r32 x;
    r32 y;
  };
  r32 v[2];
};


typedef union ivec2 ivec2;
union ivec2
{
  struct{
    i32 x;
    i32 y;
  };
  i32 v[2];
};


////////////////////
// ~sameer: matrices

typedef union mat4x4 mat4x4;
union mat4x4
{
  struct{

    //cr = column, row
    r32 cr00;
    r32 cr01;
    r32 cr02;
    r32 cr03;

    r32 cr10;
    r32 cr11;
    r32 cr12;
    r32 cr13;

    r32 cr20;
    r32 cr21;
    r32 cr22;
    r32 cr23;

    r32 cr30;
    r32 cr31;
    r32 cr32;
    r32 cr33;
  };
  struct {
    // c for column
    r32 c0[4];
    r32 c1[4];
    r32 c2[4];
    r32 c3[4];
  };
  
  r32 v[4*4];
};


global const mat4x4 mat4x4_identity = {
  .cr00=1, .cr11 = 1, .cr22 =1, .cr33 =1
};

global const mat4x4 mat4x4_zero = {0};


mat4x4 mat4x4_projection_ortho(r32 right, r32 left, r32 top, r32 bottom, r32 near, r32 far);
mat4x4 mat4x4_translation(r32 x, r32 y, r32 z);
mat4x4 mat4x4_translate(mat4x4 current, r32 x, r32 y, r32 z);
mat4x4 mat4x4_rotation_axis(vec3 axis, float radians);
b32 mat4x4_match(mat4x4 a, mat4x4 b);
//////////////////
// ~sameer: macros

#define vec_add(a,b, out,numComponents)		\
  do{						\
    for(i32 _i_=0;_i_<numComponents;_i_++){	\
      out.v[_i_] = a.v[_i_] + b.v[_i_];		\
    }						\
  } while(0)					

#define vec_sub(a,b, out,numComponents)		\
  do{						\
    for(i32 _i_=0;_i_<numComponents;_i_++){	\
      out.v[_i_] = a.v[_i_] - b.v[_i_];		\
    }						\
  } while(0)					


#define vec_scalev(a,b, out,numComponents)	\
  do{						\
    for(i32 _i_=0;_i_<numComponents;_i_++){	\
      out.v[_i_] = a.v[_i_] * b.v[_i_];		\
    }						\
  } while(0)					

#define vec_scalef(a,floatVal, out,numComponents)	\
  do{							\
    for(i32 _i_=0;_i_<numComponents;_i_++){		\
      out.v[_i_] = a.v[_i_] * (floatVal);		\
    }							\
  } while(0)					

#define vec_lerp(a,b,t, out,numComponents)	\
  do{						\
    for(i32 _i_=0;_i_<numComponents;_i_++){	\
      out.v[_i_] = Lerp(a.v[_i_], b.v[_i_], t);	\
    }						\
  } while(0)					



#endif // MATH_CORE_INCLUDE
