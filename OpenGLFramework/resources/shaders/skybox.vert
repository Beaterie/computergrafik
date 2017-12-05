#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec2 pass_TexCoord;

void main() {

    gl_Position = ProjectionMatrix * ViewMatrix * vec4(in_Position, 1.0);

    pass_TexCoord = in_TexCoord;
    
}