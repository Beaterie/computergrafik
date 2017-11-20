#version 150

in  vec4 pass_Normal;
in  vec4 pass_Position;

uniform vec3 SunPosition;
uniform vec3 PlanetColor;
uniform mat4 ViewMatrix;

out vec4 out_Color;

void main() {

  // color vectors
  vec3 ambientColor = PlanetColor*0.1;
  vec3 diffuseColor = PlanetColor;

  // vectors
  vec3 viewerWorldPos = normalize(vec3(ViewMatrix[3][0], ViewMatrix[3][1], ViewMatrix[3][2]));
  vec3 v = normalize(viewerWorldPos - pass_Position.xyz);
  vec3 l = normalize(SunPosition - pass_Position.xyz);
  vec3 n = normalize(pass_Normal.xyz);

  // angle
  float lambertian = clamp(dot(l,n), 0.0, 1.0);
  // multiplier
  float celColor = 0.0;

  // different cel-shading areas
  if (lambertian > 0.999) {
    diffuseColor = vec3(1.0,1.0,1.0);
  }
  if (lambertian > 0.0) {
  	celColor = 0.1;
  }
  if (lambertian > 0.35) {
  	celColor = 0.2;
  }
  if (lambertian > 0.65) {
  	celColor = 0.4;
  }
  if (lambertian > 0.85) {
  	celColor = 0.7;
  }
  if (lambertian > 0.96) {
  	celColor = 0.9;
  }

  // vectors with camera view
  vec4 border_Normal = ViewMatrix * pass_Normal;
  vec4 border_Position = ViewMatrix * pass_Position;

  // border vectors
  v = normalize(viewerWorldPos-border_Position.xyz);
  l = normalize(SunPosition-border_Position.xyz);
  n = normalize(border_Normal.xyz);

  // border angle
  lambertian = clamp(dot(l,n), 0.0, 1.0);

  // set color
  vec3 colorLinear = ambientColor +
                     celColor * diffuseColor;

  // cel-shading border
  if (lambertian < 0.2) {
	colorLinear = PlanetColor;  	
  }

  // export color
  out_Color = vec4(colorLinear,1.0);
}
