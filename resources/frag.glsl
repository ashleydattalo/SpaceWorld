#version 120

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 lightPosCam;
uniform vec3 cameraPos;

varying vec2 vTex0;
varying vec2 vTex1;

varying vec3 normal;
varying vec4 vecPos;

uniform mat4 V;

uniform float sun;

void main()
{
	vec4 sunPos4 = V * vec4(cameraPos, 1);
	vec3 lightPos = sunPos4.xyz;

	vec3 n = normalize(normal);
	vec3 e = normalize(cameraPos - vecPos.xyz);
	
	vec3 l = normalize(lightPos - vecPos.xyz);
	vec3 h = normalize(l + e);


	vec3 kd = texture2D(texture0, vTex0).rgb;
	vec3 ks = texture2D(texture0, vTex0).rgb;

	vec3 cd = kd * max(0.0, dot(l, n));
	vec3 cs = ks * pow(max(0.0, dot(h, n)), 50.0);


	vec3 clouds = texture2D(texture2, vTex1).rgb;
	
	vec3 color = cd + cs +  clouds;

	gl_FragColor = vec4(color, 1.0);

	if (sun == 1.0f) {
		gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);		
	}
}
