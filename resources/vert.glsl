#version 120

uniform mat4 P;
uniform mat4 MV;
uniform mat3 T;

attribute vec4 aPos;
attribute vec3 aNor;
attribute vec2 aTex;

varying vec2 vTex0;
varying vec2 vTex1;

varying vec3 normal;
varying vec4 vecPos;

uniform samplerCube cubemap;

void main()
{
	gl_Position = P * MV * aPos;
	vTex0 = aTex;

	vec4 newNormal = MV * vec4(aNor, 0.0);
	normal = newNormal.xyz;
	vecPos = MV * aPos;

	vec3 temp = T * vec3(aTex, 1.0);
	vTex1 = temp.xy;
}
