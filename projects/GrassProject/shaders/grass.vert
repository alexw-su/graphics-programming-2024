#version 330 core

// Input
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in mat4 instanceMatrix;

// Output
out vec3 WorldPosition;
out vec3 WorldNormal;
out float Height;

// World Projection Uniforms
uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;
mat4 mvc = ViewProjMatrix * WorldMatrix;

// Grass Uniforms
uniform float bladeHeight; // height of a grass blade.
uniform float time; // Time parameter for animation
uniform sampler2D noise; // noise Pattern
uniform vec2 direction; // direction of wind
uniform float strength; // speed / strength of wind

float GetNoiseValue(vec2 point)
{
	return texture(noise, point).r;
}

void main()
{
	// Setup position of vertex
	vec3 pos = VertexPosition;

	// Adjust height of the grass blade height Uniform;
	Height = pos.y;
	pos.y *= bladeHeight;

	// Get position on noise pattern based on instancing
	vec2 noisePos = (instanceMatrix * vec4(pos, 1.0)).xz;

	// Setup Wind, using noise pattern, time, and strenght
	float noiseValue = GetNoiseValue(noisePos + time * strength);
	
	// Add direction of wind
	pos.xz += direction * noiseValue * pos.y;

	// Apply changes
	gl_Position = mvc * instanceMatrix * vec4(pos, 1.0);

	WorldPosition = (WorldMatrix * instanceMatrix * vec4(VertexPosition, 1.0)).xyz;
	WorldNormal = (WorldMatrix * instanceMatrix * vec4(VertexNormal, 0.0)).xyz;
}
