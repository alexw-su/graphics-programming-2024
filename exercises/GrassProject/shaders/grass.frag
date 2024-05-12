#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in float Height;

out vec4 FragColor;

uniform vec4 baseColor;
uniform vec4 tipColor;

// const vec4 base_green =   vec4(0.0f, 0.4f, 0.0f, 1.0f);
// const vec4 tip_green =    vec4(0.5f, 1.0f, 0.0f, 1.0f);

void main()
{
    vec4 grassColor = mix(baseColor, tipColor, Height);
    
    // Output the final color
    FragColor = grassColor;
}
