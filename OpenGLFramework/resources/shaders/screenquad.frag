#version 150
in vec2 pass_uv;

uniform sampler2D QuadTex;
uniform bool MisterGrey;

out vec4 out_uv;

void main() {
  out_uv = texture(QuadTex, pass_uv);
  if (MisterGrey) {
    vec3 c1 = vec3(out_uv);
    vec3 c2 = vec3(0.2126,0.7152,0.0722);
    float g = dot(c1,c2);
    out_uv = vec4(g,g,g,1.0);
  } 
}