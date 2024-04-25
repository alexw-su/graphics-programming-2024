#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 Color;

const vec4 dark_green = vec4(0.0f, 0.4f, 0.0f, 1.0f);
const vec4 light_green = vec4(0.5f, 1.0f, 0.0f, 1.0f);

const vec4 grass_colors[3] = vec4[3](
	dark_green,
	dark_green,
	light_green
);

void main()
{
	FragColor = gras;
}
