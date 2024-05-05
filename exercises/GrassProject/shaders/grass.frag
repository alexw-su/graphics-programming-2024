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
    float heightFactor = (WorldPosition.y + 1.0) / 2.0; // Normalize the height to [0, 1]
    vec4 grassColor;
    
    if (heightFactor <= 0.5) {
        // Interpolate between color1 and color2 for the base and middle part of the grass blade
        // grassColor = mix(base_green, middle_green, heightFactor * 2.0);
        grassColor = baseColor;
    } 
    else 
    {
        // Interpolate between color2 and color3 for the tip of the grass blade
        // grassColor = mix(middle_green, tip_green, (heightFactor - 0.5) * 2.0);
        grassColor = tipColor;
    }
    
    // Output the final color
    FragColor = grassColor;
}
