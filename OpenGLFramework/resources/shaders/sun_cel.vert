#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;

// Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec3 pass_Normal;
out vec3 pass_Position;

void main(void)
{
	gl_Position = (ProjectionMatrix * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);

	// vertex position
	pass_Position = (ViewMatrix * ModelMatrix * vec4(in_Position, 1.0)).xyz;

	// normal position
	pass_Normal = (ViewMatrix * ModelMatrix * vec4(in_Normal, 0.0)).xyz;
}