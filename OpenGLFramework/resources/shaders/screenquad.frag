#version 150
in vec2 pass_uv;
in vec4 gl_FragCoord;

uniform sampler2D QuadTex;
uniform bool MisterGrey;
uniform bool GaussBlur;

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

}