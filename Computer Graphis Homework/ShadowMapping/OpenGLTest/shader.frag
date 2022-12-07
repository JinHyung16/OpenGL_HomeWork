#version 150 core

out vec4 out_Color;

in vec3 worldPos;
in vec3 normal;
in vec2 texCoord;
in vec4 shadow_Position;

uniform vec3 lightPos;
uniform vec3 lightInt;
uniform vec3 ambInt;
uniform vec3 color;
uniform vec3 specularColor;
uniform vec3 cam;
uniform float shininess;
uniform sampler2D colorTexture;
uniform sampler2D bumpTexture;
uniform sampler2D shadowMap;

vec2 poossionDisk[4] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.94184101, -0.92938870),
	vec2(0.34495938, 0.29387760) );

mat3 getTBN(vec3 N){
	vec3 Q1 = dFdx(worldPos),Q2= dFdy(worldPos);
	vec2 st1 = dFdx(texCoord), st2 = dFdy(texCoord);
	float D = st1.s*st2.t - st2.s*st1.t;
	return mat3(normalize((Q1*st2.t - Q2*st1.t)*D),
			normalize((-Q1*st2.s + Q2*st1.s)*D),
			N);
}
void main(void)
{
	vec3 L = normalize(lightPos - worldPos);
	vec3 N = normalize(normal);

	mat3 TBN= getTBN(N);
	float Bu = texture(bumpTexture, texCoord+vec2(0.0001f, 0)).r - texture(bumpTexture, texCoord+vec2(-0.0001f, 0)).r;
	float Bv = texture(bumpTexture, texCoord+vec2(0, 0.0001f)).r - texture(bumpTexture, texCoord+vec2(0, -0.0001f)).r;
	vec3 bumpVec = vec3(-Bu*15, -Bv*15, 1);
	N = normalize(TBN*bumpVec);
	vec3 V = normalize(cam - worldPos);
	vec3 R = 2*dot(N,L)*N-L;

	vec4 albelo = texture(colorTexture, texCoord);

	vec3 diffuse = lightInt * max(0, dot(N,L)) * albelo.rgb;
	vec3 ambient = ambInt * albelo.rgb;
	vec3 specular = color * lightInt * pow( max(0,dot(R,V)), shininess) * max(0, dot(N,L)) * specularColor;
	
	vec3 shadowCoord = shadow_Position.xyz / shadow_Position.w * 0.5 + 0.5;
	float shadowFactor = 1;
	float biass = 0.001;
	for(int i = 0; i<4; i++)
	{
		if( texture(shadowMap, shadowCoord.xy + poossionDisk[i]/700.0).r < shadowCoord.z - biass)
		{
			shadowFactor -= 0.25;
		}
	}
	//if not use 'glEnable(GL_FRAMEBUFFER_SRGB)'
	//out_Color = vec4(pow(diffuse + ambient + specular, vec3(1/2.f)), 1.0);

	//if use 'glEnable(GL_FRAMEBUFFER_SRGB)'
	out_Color = vec4(pow((diffuse + specular)*shadowFactor + ambient, vec3(1/2.2)), 1.0);

}
