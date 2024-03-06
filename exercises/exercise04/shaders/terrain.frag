#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;
in float Height;

out vec4 FragColor;

uniform vec4 Color;
uniform sampler2D ColorTexture0;
uniform sampler2D ColorTexture1;
uniform sampler2D ColorTexture2;
uniform sampler2D ColorTexture3;
uniform vec2 ColorTextureRange01;
uniform vec2 ColorTextureRange12;
uniform vec2 ColorTextureRange23;
uniform vec2 ColorTextureScale;

float inverseMix(vec2 range, float value)
{
	return clamp((value - range.x) / (range.y - range.x), 0, 1);
}

void main()
{
	vec4 sampleTex0 = texture(ColorTexture0, TexCoord * ColorTextureScale);
	vec4 sampleTex1 = texture(ColorTexture1, TexCoord * ColorTextureScale);
	vec4 sampleTex2 = texture(ColorTexture2, TexCoord * ColorTextureScale);
	vec4 sampleTex3 = texture(ColorTexture3, TexCoord * ColorTextureScale);

	vec4 sampleTex = sampleTex0;
	sampleTex = mix(sampleTex, sampleTex1, inverseMix(ColorTextureRange01, Height));
	sampleTex = mix(sampleTex, sampleTex2, inverseMix(ColorTextureRange12, Height));
	sampleTex = mix(sampleTex, sampleTex3, inverseMix(ColorTextureRange23, Height));

	FragColor = Color * sampleTex;
}