#version 150
in vec2 pass_uv;

uniform sampler2D QuadTex;
uniform bool MisterGrey;

out vec4 out_uv;

void main() {
  out_uv = texture(QuadTex, pass_uv);
  if (MisterGrey) {
    //out_uv = vec4(out_uv.x,out_uv.x,out_uv.x,out_uv.w);
    vec3 c1 = vec3(out_uv);
    vec3 c2 = vec3(0.3,0.59,0.11);
    float g = dot(c1,c2);
    out_uv = vec4(g,g,g,1.0);
  }
  //out_uv = vec4(0.5,0.5,0.5,1.0);
}