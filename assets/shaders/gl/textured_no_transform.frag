in vec4 var_color;
in vec2 var_uv;
out vec4 out_frag_color;

uniform sampler2D Tex0;
void main(){
out_frag_color = texture(Tex0, var_uv);

};
