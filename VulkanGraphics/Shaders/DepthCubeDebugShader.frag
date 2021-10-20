#version 460
layout(binding = 0) uniform samplerCube depthMap;
layout(location = 0) in vec3 WorldPos;
layout(location = 0) out vec4 outColor;

float near_plane = 0.1f;
float far_plane = 100.0f;

void main() 
{
    outColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}
