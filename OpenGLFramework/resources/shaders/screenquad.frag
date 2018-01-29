#version 150
in vec2 pass_uv;
//in vec2 pass_sunPos;
in vec4 gl_FragCoord;

uniform sampler2D QuadTex;
uniform bool MisterGrey;
uniform bool GaussBlur;
uniform mat4 ModelMatrixSun;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform vec3 SunPosition;

out vec4 out_uv;

void main() {
  out_uv = texture(QuadTex, pass_uv);

  // blur
  if (GaussBlur) {
    vec4 pix_color[9];
    vec2 pixel_size;
    pixel_size.x = pass_uv.x / gl_FragCoord.x;
    pixel_size.y = pass_uv.y / gl_FragCoord.y;

    // | 20 | 21 | 22 |     | 6 | 7 | 8 |
    // | 10 | 11 | 12 |     | 3 | 4 | 5 |
    // | 00 | 01 | 02 |     | 0 | 1 | 2 |
    int count = 0;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        vec2 fragCoord = vec2(gl_FragCoord.x - 1.0f + i, gl_FragCoord.y - 1.0f + j);
        vec2 texCoord = vec2(pixel_size.x * fragCoord.x, pixel_size.y * fragCoord.y);
        pix_color[count] = (texture(QuadTex, texCoord));
        count++;
      }
    }
    out_uv = pix_color[0]/16 + pix_color[1]/8 + pix_color[2]/16 +
             pix_color[3]/8  + pix_color[4]/4 + pix_color[5]/8  +
             pix_color[6]/16 + pix_color[7]/8 + pix_color[8]/16;
  }
  // greyscale
  if (MisterGrey) {
    vec3 c1 = vec3(out_uv);
    vec3 c2 = vec3(0.2126f, 0.7152f, 0.0722f);
    float g = dot(c1, c2);
    out_uv = vec4(g, g, g, 1.0f);
  } 
  

  float exposure = 0.2f;
  float decay = 0.9f;
  float density = 0.5f;
  float weight = 0.55f;

  // quality of the godrays
  int numParticles = 400;

  mat4 m = ProjectionMatrix * ViewMatrix * ModelMatrixSun;
  vec4 sun = m * vec4(0.0f,0.0f,0.0f,1.0f);
  vec2 sunPos = vec2(sun/sun.z);
  //vec3 new_SunPos = (ViewMatrix * vec4(SunPosition, 1.0)).xyz / ((ViewMatrix * vec4(SunPosition, 1.0))).w;
  //vec2 sunPos = vec2(new_SunPos.xy);
  vec2 textCoo = pass_uv;
  vec2 deltaTexCoord = (textCoo - sunPos);
  deltaTexCoord *= 1.0f / float(numParticles) * density;
  float illuminationDecay = 1.0f;

  vec4 color = texture2D(QuadTex, textCoo); //texture2D(QuadTex, textCoo.xy) * 0.4f;

  for (int i = 0; i < numParticles; i++)
  {
     textCoo -= deltaTexCoord;
     vec4 sample = texture2D(QuadTex, textCoo); // * 0.4f;
     sample *= illuminationDecay * weight;
     color += sample;
     illuminationDecay *= decay;
  }

  out_uv = color;




/*
// Calculate vector from pixel to light source in screen space.
   half2 deltaTexCoord = (texCoord - ScreenLightPos.xy);
  // Divide by number of samples and scale by control factor.
  deltaTexCoord *= 1.0f / NUM_SAMPLES * Density;
  // Store initial sample.
   half3 color = tex2D(frameSampler, texCoord);
  // Set up illumination decay factor.
   half illuminationDecay = 1.0f;
  // Evaluate summation from Equation 3 NUM_SAMPLES iterations.
   for (int i = 0; i < NUM_SAMPLES; i++)
  {
    // Step sample location along ray.
    texCoord -= deltaTexCoord;
    // Retrieve sample at new location.
   half3 sample = tex2D(frameSampler, texCoord);
    // Apply sample attenuation scale/decay factors.
    sample *= illuminationDecay * Weight;
    // Accumulate combined color.
    color += sample;
    // Update exponential decay factor.
    illuminationDecay *= Decay;
  }
  // Output final color with a further scale control factor.
   return float4( color * Exposure, 1);*/

}