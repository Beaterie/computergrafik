#version 150

in vec2 pass_TexCoord;

uniform mat4 ProjectMatrix;
uniform mat4 ViewMatrix;

uniform sampler2D Texture;

out vec4 out_Color;

void main() {
    // export color    
    ColTex = texture(Texture, pass_TexCoord);
    out_Color = vec4(ColTex, 0);
}