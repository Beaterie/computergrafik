#version 150

in  vec3 pass_Normal;
in  vec3 pass_Position;

uniform mat4 ModelMatrix;
uniform vec3 SunPosition;
uniform mat4 ViewMatrix;

out vec4 out_Color;

void main() {
  // old implementation
  //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);

  vec3 ambientColor = vec3(0.9,0.4,0.0);
  vec3 diffuseColor = vec3(0.3,0.3,0.0);
  vec3 specColor    = vec3(0.1,0.1,0.1);
  float shininess   = 20.0;
  float a 			= 1.0;
  float rand 		= 1.0;

  vec3 viewerWorldPos = normalize(vec3(ViewMatrix[3][0], ViewMatrix[3][1], ViewMatrix[3][2]));
  vec3 v = normalize(viewerWorldPos - pass_Position);
  vec3 l = normalize(SunPosition - pass_Position);
  vec3 n = normalize(pass_Normal);
  vec3 h = normalize(v + l);

  float lambertian = clamp(dot(v,n), 0.0, 1.0);
  float specular = pow(lambertian*1.2, shininess) * 100.0;
  if (lambertian < 0.68) {
  	a = 0.8;
  	//rand = 0.0;
  }
  if (lambertian < 0.5) {
  	a = 0.0;
  	rand = 0.0;
  }

  vec3 colorLinear = (ambientColor +
                     lambertian * diffuseColor +
                     specular * specColor) * rand;

  out_Color = vec4(colorLinear,a);
}
