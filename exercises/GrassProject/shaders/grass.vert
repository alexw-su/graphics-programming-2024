#version 330 core

// Input
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

// Output
out vec3 WorldPosition;
out vec3 WorldNormal;
out float Height;

// Uniforms
uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;

uniform float bladeHeight; // height of a grass blade.
// uniform vec3 offsets[100]; // offset vectors for each instance of grass blade.
// uniform float time; // Time parameter for animation


void main()
{
    // Get offset for this instance
    // vec3 offset = offsets[gl_InstanceID];
    vec3 offset = vec3(0.0f);

    WorldPosition = (WorldMatrix * vec4(VertexPosition + offset, 1.0)).xyz;
	WorldNormal = (WorldMatrix * vec4(VertexNormal, 0.0)).xyz;

    // Pass transformed vertex position and normal to the geometry shader
    gl_Position = ViewProjMatrix * vec4(WorldPosition, 1.0);
}
