#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_StarPosition;
layout(location = 1) in vec3 in_StarColor;

// Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 pass_StarColor;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix) * vec4(in_StarPosition, 1.0f);
	pass_StarColor = in_StarColor;
}