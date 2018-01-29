#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_uv;

uniform bool HorMirror;
uniform bool VerMirror;
//uniform mat4 ViewMatrix;
//uniform mat4 ProjectionMatrix;
//uniform mat4 ModelMatrixSun;

// Matrix Uniforms as specified with glUniformMatrix4fv
out vec2 pass_uv;
//out vec2 pass_sunPos;

void main(void)
{
	gl_Position = vec4(in_pos, 1.0);
	pass_uv = vec2(in_uv);
	if (HorMirror) {
		if (in_uv.y == 0.0) {
			pass_uv.y = 1.0;
		} else {
			pass_uv.y = 0.0;
		}
  	}
	if (VerMirror) {
		if (in_uv.x == 0.0) {
			pass_uv.x = 1.0;
		} else {
			pass_uv.x = 0.0;
		}
  	}
	//vec4 sunPos = ProjectionMatrix * ViewMatrix * ModelMatrixSun * vec4(0.0f,0.0f,0.0f,1.0f);
//	sunPos = sunPos/sunPos.z;
//	pass_sunPos = sunPos.xy;
}