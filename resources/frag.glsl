#version 120

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 lightPosCam;

varying vec2 vTex0;
varying vec2 vTex1;

varying vec3 normal;
varying vec4 vecPos;

uniform mat4 V;

void main()
{
	vec4 sunPos4 = V * vec4(lightPosCam, 1);
	vec3 lightPos = sunPos4.xyz;

	vec3 n = normalize(normal);
	vec3 e = normalize(vec3(0.0, 0.0, 0.0) - vecPos.xyz);
	
	vec3 l = normalize(lightPos - vecPos.xyz);
	vec3 h = normalize(l + e);


	vec3 kd = texture2D(texture0, vTex0).rgb;
	vec3 ks = texture2D(texture0, vTex0).rgb;

	vec3 cd = kd * max(0.0, dot(l, n));
	vec3 cs = ks * pow(max(0.0, dot(h, n)), 50.0);


	vec3 clouds = texture2D(texture2, vTex1).rgb;
	
	vec3 color = cd + cs +  clouds;

	gl_FragColor = vec4(color, 1.0);
}
