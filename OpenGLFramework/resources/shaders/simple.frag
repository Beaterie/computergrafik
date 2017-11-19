#version 150

in  vec3 pass_Normal;
in  vec3 pass_Position;

uniform mat4 ModelMatrix;
uniform vec3 SunPosition;
uniform vec3 PlanetColor;
uniform mat4 ViewMatrix;

out vec4 out_Color;

void main() {
  // old implementation
  //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);

  vec3 ambientColor = PlanetColor*0.005;
  vec3 diffuseColor = PlanetColor;
  vec3 specColor = vec3(1.0,1.0,1.0);
  float shininess = 20.0;

  vec3 v = normalize(vec3(ViewMatrix[3][0],ViewMatrix[3][1],ViewMatrix[3][2]));
  vec3 l = normalize(SunPosition-pass_Position);
  vec3 n = normalize(pass_Normal);
  vec3 h = normalize(v+l);

  float lambertian = clamp(dot(l,n), 0.0, 1.0);
  float specular = 0.0;

  if (lambertian > 0) {
    float specAngle = max(dot(h,n), 0.0);
    specular = pow(specAngle, shininess);
  }

  vec3 colorLinear = ambientColor +
                     lambertian * diffuseColor +
                     specular * specColor;

  out_Color = vec4(colorLinear,1.0);
}
