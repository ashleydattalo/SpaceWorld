#version 120

uniform samplerCube cubemap;

varying vec3 vNor; // normal in world space

void main()
{
	// We want the normal to be in world coords, not camera coords.
	gl_FragColor.rgb = textureCube(cubemap, vNor).rgb;
}
