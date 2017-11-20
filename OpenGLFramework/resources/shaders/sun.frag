#version 150

in  vec3 pass_Normal;
in  vec3 pass_Position;

uniform mat4 ModelMatrix;
uniform vec3 SunPosition;
uniform mat4 ViewMatrix;

out vec4 out_Color;

void main() {

  // color vectors
  vec3 ambientColor = vec3(0.9,0.4,0.0);
  vec3 diffuseColor = vec3(0.3,0.3,0.0);
  vec3 specColor    = vec3(0.1,0.1,0.1);
  // Blinn-Phong power a
  float shininess   = 20.0;
  // transparency parameter
  float t 			    = 1.0;

  // vectors
  vec3 viewerWorldPos = normalize(vec3(ViewMatrix[3][0], ViewMatrix[3][1], ViewMatrix[3][2]));
  vec3 v = normalize(viewerWorldPos - pass_Position);
  vec3 l = normalize(SunPosition - pass_Position);
  vec3 n = normalize(pass_Normal);
  vec3 h = normalize(v + l);

  // angle
  float lambertian = clamp(dot(v,n), 0.0, 1.0);
  // specular multiplier
  float specular = pow(lambertian * 1.5, shininess) * 100.0;

  // transparency areas
  if (lambertian < 0.5) {
  	t = 0.85;
  }
  if (lambertian == 0.0) {
  	t = 0.0;
  }

  // set color
  vec3 colorLinear = (ambientColor +
                     lambertian * diffuseColor +
                     specular * specColor);

  // export color
  out_Color = vec4(colorLinear,t);
}
