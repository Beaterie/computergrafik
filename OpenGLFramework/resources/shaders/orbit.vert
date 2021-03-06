#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_OrbitPosition;
// layout(location = 1) in vec3 in_OrbitColor;

// Matrix Uniforms as specified with glUniformMatrix4fv
uniform float OrbitDistance;
uniform mat4 MoonMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

//out vec3 pass_OrbitColor;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * MoonMatrix) * vec4(in_OrbitPosition.x*OrbitDistance,
		in_OrbitPosition.z, in_OrbitPosition.y*OrbitDistance, 1.0f);
	//pass_OrbitColor = in_OrbitColor;
}