#version 330 core

// Input
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

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
uniform float bend;
uniform vec2 direction;
uniform float strength;

uniform float noiseScale;
uniform vec4 WindFrequency;
uniform float WindStrength;

// Instancing
//uniform vec3 offsets; // offset vectors for each instance of grass blade.


//mat3 ComputeWindRotationMatrix()
//{
//	vec2 uv = VertexPosition.xz * WindDistortionMapScaleOffset.xy + WindDistortionMapScaleOffset.zw + WindFrequency.xy * time;
//	
//	vec2 windSample = (texture(noise, uv).rg * 2 - 1);
//	vec3 wind = normalize(vec3(windSample.x, windSample.y, 0));
//	return rotate(M_PI * length(windSample), wind);
//}

// Tiling and Offset method based on Unity Engine
vec2 TilingAndOffset(vec2 UV, vec2 Tiling, vec2 Offset)
{
	return UV * Tiling + Offset;
}

float GetNoiseValue(vec2 point)
{
	return texture(noise, point).r;
}


void main()
{
	// Setup position of vertex
	vec3 pos = VertexPosition;

	// Adjust height of the grass blade height Uniform;
	pos.y *= bladeHeight;

	// Setup Wind, using noise pattern, time, and strenght
	NoiseValue = GetNoiseValue(VertexPosition.xz + time * strength);
	
	// Add direction of wind
	pos.xz += direction * NoiseValue * pos.y;

	// Apply changes
	gl_Position = mvc * vec4(pos, 1.0);
}
