#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 pass_TexCoord;

void main() {
    mat4 inverseMatrix = inverse(ViewMatrix);
    vec3 camPos = inverseMatrix[3].xyz;   

    // box moves with camera
    gl_Position = ProjectionMatrix * ViewMatrix  * vec4(in_Position + camPos, 1.0);

    pass_TexCoord = in_Position;
    
}