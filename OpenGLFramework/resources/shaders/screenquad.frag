#version 150
in vec2 pass_uv;
in vec4 gl_FragCoord;

uniform sampler2D QuadTex;
uniform sampler2D LightTex;
uniform bool MisterGrey;
uniform bool GaussBlur;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

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
        vec2 fragCoord = vec2(gl_FragCoord.x - 1.0 + i, gl_FragCoord.y - 1.0 + j);
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
    vec3 c2 = vec3(0.2126,0.7152,0.0722);
    float g = dot(c1,c2);
    out_uv = vec4(g,g,g,1.0);
  } 

  vec4 sunPos = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec4 ScreenLightPos = ProjectionMatrix * ViewMatrix * sunPos;
	vec3 bigSun = ScreenLightPos.xyz/ScreenLightPos.w;
	vec2 sun = (bigSun.xy/bigSun.z + vec2(1.0f, 1.0f))/2;
	
	int NUM_SAMPLES = 500;
	float Density = 5.0f;
	float Weight = 0.55f;
	float Exposure = 0.15f;
	float Decay = 0.9f;

	vec2 texCoord = pass_uv;
	// Calculate vector from pixel to light source in screen space.
   vec2 deltaTexCoord = (texCoord - sun);//ScreenLightPos.xy);
  // Divide by number of samples and scale by control factor.
  deltaTexCoord *= 1.0f / NUM_SAMPLES * Density;
  // Store initial sample.
   vec3 color = texture2D(LightTex, texCoord).xyz;
  // Set up illumination decay factor.
   float illuminationDecay = 1.0f;
  // Evaluate summation from Equation 3 NUM_SAMPLES iterations.
   for (int i = 0; i < NUM_SAMPLES; i++)
  {
    // Step sample location along ray.
    texCoord -= deltaTexCoord;
    // Retrieve sample at new location.
    vec3 sample = texture2D(LightTex, texCoord).xyz;
    // Apply sample attenuation scale/decay factors.
    sample *= illuminationDecay * Weight;
    // Accumulate combined color.
    color += sample;
    // Update exponential decay factor.
    illuminationDecay *= Decay;
  }
  // Output final color with a further scale control factor.
   //return float4( color * Exposure, 1);

	out_uv = out_uv + vec4(color * Exposure,0);

}