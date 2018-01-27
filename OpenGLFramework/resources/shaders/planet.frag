#version 150

in  vec4 pass_Normal;
in  vec4 pass_Position;
in  vec2 pass_TexCoord;
in  vec3 pass_Tangent;

uniform float PlanetNumber;
uniform vec3 SunPosition;
uniform vec3 PlanetColor;
uniform mat4 ViewMatrix;
uniform mat4 NormalMatrix;
uniform sampler2D Texture;
uniform sampler2D TextureNight;
uniform sampler2D NormalMap;

out vec4 out_Color;

void main() {

  // normal mapping
  vec3 normal_Mapping = vec3(texture(NormalMap, pass_TexCoord).rg * 2.0 - 1.0, texture(NormalMap, pass_TexCoord).b);
  vec3 bi_Tangent = cross(pass_Normal.xyz, pass_Tangent);

  mat3 TangentMatrix = mat3(pass_Tangent, bi_Tangent, pass_Normal.xyz);

  vec3 detailNormal = normalize(TangentMatrix * normal_Mapping);
  
  vec3 new_SunPos = (ViewMatrix * vec4(SunPosition, 1.0)).xyz / ((ViewMatrix * vec4(SunPosition, 1.0))).w;

  // vectors
  vec3 viewerWorldPos = normalize(vec3(ViewMatrix[3][0], ViewMatrix[3][1], ViewMatrix[3][2]));
  vec3 v = normalize(viewerWorldPos - pass_Position.xyz);
  vec3 l = normalize(new_SunPos - pass_Position.xyz);
  vec3 n = vec3(0.0);

  if (PlanetNumber == 4.0) {
    n = normalize(detailNormal);
  }
  else {
    n = normalize(pass_Normal.xyz);
  }

  vec3 h = normalize(v + l);

  // color vectors
  vec3 Color        = texture(Texture, pass_TexCoord).xyz;
  vec3 ambientColor = vec3(0.0);
  vec3 diffuseColor = Color * max(dot(n,l), 0);
  vec3 specColor    = vec3(0.5);

  // Blinn-Phong power a
  float shininess   = 25;
  // angle
  float lambertian = clamp(dot(l,n), 0.0, 1.0);
  // specular multiplier
  float specular = 0.0;

  // specular lighting
  if (lambertian > 0.0) {
    float specAngle = clamp(dot(n,h), 0.0, 1.0);
    specular = pow(specAngle, shininess);
  }

  // set color
  vec3 colorLinear = ambientColor +
                     diffuseColor +
                     specular * specColor;

  // export color
  out_Color = vec4(colorLinear, 1.0);

  // export color with earth night texture
  if (PlanetNumber == 3.0) {
    out_Color = vec4( colorLinear + (
        (1.0-lambertian) * texture(TextureNight, pass_TexCoord)
      ).xyz, 1.0 );
  }
}