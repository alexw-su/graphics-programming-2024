#version 330 core

// Input
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

// Output
out vec3 WorldPosition;
out vec3 WorldNormal;

// Uniforms
uniform mat4 WorldMatrix;
uniform mat4 ViewProjMatrix;
uniform mat4 grassBlade;

uniform vec3 offsets[100];

uniform float time; // Time parameter for animation


void main()
{
    vec3 offset = offsets[gl_InstanceID];

	// Transform vertex position and normal to world space
    vec4 worldPos = (VertexPosition + offset, 1.0);
    
    vec3 worldNormal = mat3(transpose(inverse(grassBlade))) * VertexNormal;

    // Pass transformed vertex position and normal to the geometry shader
    gl_Position = worldPos;
}
