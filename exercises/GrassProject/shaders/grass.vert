#version 330 core

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

out vec3 WorldPosition;
out vec3 WorldNormal;
out vec2 TexCoord;

uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float amplitude;
uniform float frequency;
uniform float phase;
uniform float grass_scale;

const vec3 grass_vertices[3] = vec3[3](
	vec3(0.5f, 0.0f, 0.0f),
	vec3(-0.5f, 0.0f, 0.0f),
	vec3(0.0f, 1.0f, 0.0f)
);


void main()
{


	WorldPosition = (WorldMatrix * vec4(VertexPosition, 1.0)).xyz;
	WorldNormal = (WorldMatrix * vec4(VertexNormal, 0.0)).xyz;
	TexCoord = VertexTexCoord;
	gl_Position = ViewProjMatrix * vec4(WorldPosition, 1.0);
}
