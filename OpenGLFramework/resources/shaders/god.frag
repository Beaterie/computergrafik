#version 150
in vec2 pass_uv;
in vec4 gl_FragCoord;

uniform sampler2D QuadTex;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec4 out_uv;

void main() {
	
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
   vec3 color = texture2D(QuadTex, texCoord).xyz;
  // Set up illumination decay factor.
   float illuminationDecay = 1.0f;
  // Evaluate summation from Equation 3 NUM_SAMPLES iterations.
   for (int i = 0; i < NUM_SAMPLES; i++)
  {
    // Step sample location along ray.
    texCoord -= deltaTexCoord;
    // Retrieve sample at new location.
    vec3 sample = texture2D(QuadTex, texCoord).xyz;
    // Apply sample attenuation scale/decay factors.
    sample *= illuminationDecay * Weight;
    // Accumulate combined color.
    color += sample;
    // Update exponential decay factor.
    illuminationDecay *= Decay;
  }
  // Output final color with a further scale control factor.
   //return float4( color * Exposure, 1);


	out_uv = texture(QuadTex, pass_uv);
	out_uv = out_uv + vec4(color * Exposure,0);
}
