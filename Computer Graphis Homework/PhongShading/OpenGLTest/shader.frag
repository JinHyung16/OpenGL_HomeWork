#version 150 core

out vec4 out_Color;

in vec3 worldPos;
in vec3 normal;
in vec2 texCoord;

uniform vec3 lightPos;
uniform vec3 lightInt;
uniform vec3 ambInt;
uniform vec3 color;
uniform vec3 cam;

void main(void)
{
	vec3 L = normalize(lightPos - worldPos);
	vec3 N = normalize(normal);
	vec3 V = normalize(cam - worldPos);
	vec3 R = 2*dot(N,L)*N-L;

	vec3 diffuse = lightInt * max(0, dot(N,L)) * color;
	vec3 ambient = ambInt * color;
	vec3 specular = lightInt * pow(max(0,dot(R,V)), 20) * max(0, dot(N,L));
	out_Color = vec4(diffuse + ambient + specular, 1.0);
}
