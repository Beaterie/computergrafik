#version 150

in  vec3 pass_uv;

out vec4 out_uv;

void main() {
  out_uv = texture(FEEEEHLT, pass_uv);
}