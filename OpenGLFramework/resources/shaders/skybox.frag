#version 150

in vec3 pass_TexCoord;

uniform samplerCube TextureSky;

out vec4 out_Color;

void main() {
    // export color    
    out_Color = vec4(1.0,0.0,0.0,1.0);//texture(TextureSky, pass_TexCoord);
}