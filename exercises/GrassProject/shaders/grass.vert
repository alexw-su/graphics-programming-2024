#version 330 core

// Input
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in mat4 instanceMatrix;

// Output
out vec3 WorldPosition;
out vec3 WorldNormal;
out float Height;
out float NoiseValue; // a value of a point on the noise pattern. Based on the color.

// World Projection Uniforms
uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;
mat4 mvc = ViewProjMatrix * WorldMatrix;

// Grass Uniforms
uniform float bladeHeight; // height of a grass blade.
uniform float time; // Time parameter for animation
uniform sampler2D noise;
uniform vec2 direction;
uniform float strength;


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

	// Setup Wind, using noise pattern, time, and strenght
	NoiseValue = GetNoiseValue(VertexPosition.xz + time * strength);
	
	// Add direction of wind
	pos.xz += direction * NoiseValue * pos.y;

	// Apply changes
	gl_Position = mvc * instanceMatrix * vec4(pos, 1.0);
}
