#version 150
in vec2 pass_uv;

uniform sampler2D QuadTex;

out vec4 out_uv;

void main() {
  out_uv = texture(QuadTex, pass_uv);
}