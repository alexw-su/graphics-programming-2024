#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture;
uniform vec2 ColorTextureScale;

uniform float AmbientReflection;
uniform float DiffuseReflection;

uniform vec3 AmbientColor;
uniform vec3 LightColor;
uniform float LightRange;
uniform vec3 LightPosition;

vec3 GetAmbientReflection(vec3 objectColor)
{
	return AmbientColor * AmbientReflection * objectColor;
}

vec3 GetDiffuseReflection(vec3 objectColor, vec3 lightVector, vec3 normalVector)
{
	return LightColor * DiffuseReflection * objectColor * max(dot(lightVector, normalVector), 0.0f);
}


void main()
{
	vec4 objectColor = Color * texture(ColorTexture, TexCoord * ColorTextureScale);
	vec3 lightVector = normalize(LightPosition - WorldPosition);
	vec3 normalVector = normalize(WorldNormal);

	float distanceToLight = length(LightPosition - WorldPosition);

	float value = distanceToLight - LightRange;

	vec3 diffuse;
	vec3 ambient;

	if(value <= 0) diffuse = GetDiffuseReflection(objectColor.rgb, lightVector, normalVector);
	ambient = GetAmbientReflection(objectColor.rgb);

    FragColor = vec4(ambient + diffuse, 1.0f);
}
