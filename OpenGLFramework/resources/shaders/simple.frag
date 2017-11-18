#version 150

in  vec3 pass_Normal;
in  vec3 pass_Position;

uniform vec3 SunPosition;
uniform vec3 PlanetColor;
uniform mat4 ViewMatrix;

out vec4 out_Color;

void main() {
  //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
  vec3 h = normalize(vec3(ViewMatrix[3][0],ViewMatrix[3][1],ViewMatrix[3][2])+vec3(SunPosition-pass_Position));
  float Illusion =	0.2 * 0.5 +
  					0.7 * 1.0 * dot(vec3(SunPosition-pass_Position),pass_Normal) +
  					0.3 * 0.4 * pow(dot(pass_Normal,h),1);
  out_Color = vec4(PlanetColor*Illusion,1.0);
}
