#version 150

in  vec4 pass_Normal;
in  vec4 pass_Position;
in  vec2 pass_TexCoord;

uniform vec3 SunPosition;
uniform vec3 PlanetColor;
uniform mat4 ViewMatrix;

uniform sampler2D Texture;
uniform sampler2D TextureNight;

out vec4 out_Color;

void main() {
  // old implementation
  //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);

  // color vectors
  vec3 ambientColor = PlanetColor * 0.005;
  vec3 diffuseColor = vec3(0.005);
  vec3 specColor    = vec3(0.1);
  // Blinn-Phong power a
  float shininess   = 2;

  // vectors
  vec3 viewerWorldPos = normalize(vec3(ViewMatrix[3][0], ViewMatrix[3][1], ViewMatrix[3][2]));
  vec3 v = normalize(viewerWorldPos - pass_Position.xyz);
  vec3 l = normalize(SunPosition - pass_Position.xyz);
  vec3 n = normalize(pass_Normal.xyz);
  vec3 h = normalize(v + l);

  // angle
  float lambertian = clamp(dot(l,n), 0.0, 1.0);
  // specular multiplier
  float specular = 0.0;

  // specular lighting
  if (lambertian > 0.0) {
    float specAngle = clamp(dot(h,n), 0.0, 1.0);
    specular = pow(specAngle, shininess);
  }

  // set color
  vec3 colorLinear = ambientColor +
                     lambertian * diffuseColor +
                     specular * specColor;

  // export color
  if (PlanetColor == vec3(0.05f, 0.1f, 1.0f)) {
    colorLinear = colorLinear - ambientColor;
    out_Color = vec4( (lambertian * texture(Texture, pass_TexCoord) +
      (1.0-lambertian) * texture(TextureNight, pass_TexCoord)).xyz + colorLinear, 1.0 );
  }
  else {
    out_Color = vec4(lambertian * texture(Texture, pass_TexCoord).xyz + colorLinear, 1.0);
  }
}
