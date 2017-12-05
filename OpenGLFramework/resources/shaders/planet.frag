#version 150

in  vec4 pass_Normal;
in  vec4 pass_Position;
in  vec2 pass_TexCoord;
in  vec3 pass_Tangent;

uniform vec3 SunPosition;
uniform vec3 PlanetColor;
uniform mat4 ViewMatrix;
uniform sampler2D Texture;
uniform sampler2D TextureNight;
uniform sampler2D NormalMap;

out vec4 out_Color;

void main() {

  // normal mapping
  vec3 normal_Mapping = normalize(texture(NormalMap, pass_TexCoord).rgb * 2.0 - 1.0);
  vec3 bi_Tangent = cross(pass_Normal.xyz, pass_Tangent);
  mat3 TangentMatrix = mat3(pass_Tangent, bi_Tangent, pass_Normal.xyz);
  vec3 detailNormal = normalize(TangentMatrix * normal_Mapping);


  // color vectors
  vec3 ambientColor = PlanetColor * 0.005;
  vec3 diffuseColor = texture(Texture, pass_TexCoord).xyz;
  vec3 specColor    = vec3(0.1);
  // Blinn-Phong power a
  float shininess   = 2;

  // vectors
  vec3 viewerWorldPos = normalize(vec3(ViewMatrix[3][0], ViewMatrix[3][1], ViewMatrix[3][2]));
  vec3 v = normalize(viewerWorldPos - pass_Position.xyz);
  vec3 l = normalize(SunPosition - pass_Position.xyz);
  vec3 n = normalize(detailNormal);
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
  out_Color = vec4(colorLinear, 1.0);

  // export color with earth night texture
  if (PlanetColor == vec3(0.05f, 0.1f, 1.0f)) {
    out_Color = vec4( colorLinear + (
      (1.0-lambertian) * texture(TextureNight, pass_TexCoord)
      ).xyz, 1.0 );
  }
}
