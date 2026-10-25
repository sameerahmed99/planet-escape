in vec4 var_color;
in vec2 var_uv;
out vec4 out_frag_color;

uniform vec2 u_tiling;
uniform vec2 u_offset;
uniform vec4 u_color = vec4(1.0, 1.0, 1.0,1.0);


uniform sampler2D Tex0;
void main(){
vec2 sampleAt = var_uv;
vec4 sampled = texture(Tex0, sampleAt);

#if G_ALPHA_CLIPPING
if(sampled.a < 0.1f){
discard;
}
#endif
out_frag_color = sampled  * u_color;

};
