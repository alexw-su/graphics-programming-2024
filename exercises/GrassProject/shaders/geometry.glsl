#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

uniform float grassLength = 1.0; // Length of grass blades

void EmitGrassBlade(vec3 position, vec3 normal) {
    // Calculate endpoints of the grass blade
    vec3 endpoint1 = position;
    vec3 endpoint2 = position + normal * grassLength;

    // Emit triangle strip for grass blade
    gl_Position = projection * view * vec4(endpoint1, 1.0);
    EmitVertex();

    gl_Position = projection * view * vec4(endpoint2, 1.0);
    EmitVertex();

    // Add some variation for more natural look
    endpoint2 += vec3(0.1, 0.0, 0.1);
    gl_Position = projection * view * vec4(endpoint2, 1.0);
    EmitVertex();

    EndPrimitive();
}

void main() {
    // Emit grass blade for each vertex
    EmitGrassBlade(gl_in[0].gl_Position.xyz, gl_in[0].gl_Normal.xyz);
}
