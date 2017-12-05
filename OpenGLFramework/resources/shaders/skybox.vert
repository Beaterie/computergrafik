#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec2 in_TexCoord;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec2 pass_TexCoord;

void main() {

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(in_TexCoord, 1.0);
    
    pass_TexCoord = in_TexCoord;
}