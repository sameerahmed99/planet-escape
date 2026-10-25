layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec4 in_color;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec2 u_tiling;
uniform vec2 u_offset;		

uniform vec4 u_color = vec4(1.0, 1.0, 1.0,1.0);

out vec4 var_color;
out vec2 var_uv;
void main(){
var_uv = (in_uv*u_tiling) + u_offset;
gl_Position = u_projection*u_view*u_model*vec4(in_pos.x, in_pos.y, in_pos.z, 1.0);
};		


