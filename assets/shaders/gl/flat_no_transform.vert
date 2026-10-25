layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec2 in_uv;
layout (location = 2) in vec4 in_color;
uniform vec4 u_color = vec4(1.0, 1.0, 1.0,1.0);

out vec4 var_color;
void main(){
var_color = u_color;
gl_Position = vec4(in_pos.x, in_pos.y, in_pos.z, 1.0);
};		


