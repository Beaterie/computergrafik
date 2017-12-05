#version 150

in vec2 pass_TexCoord;

uniform mat4 ProjectMatrix;
uniform mat4 ViewMatrix;

uniform sampler2D Texture;

out vec4 out_Color;

void main() {
    // export color    
    out_Color = texture(Texture, pass_TexCoord);
}