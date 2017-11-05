#version 150

in  vec3 pass_StarColor;
out vec4 out_StarColor;

void main() {
  out_StarColor = vec4(abs(normalize(pass_StarColor)), 1.0);
}