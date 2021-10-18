#version 460
layout(binding = 0) uniform sampler2D depthMap;
layout(location = 0) in vec3 WorldPos;


float near_plane = 0.1f;
float far_plane = 100.0f;

void main() 
{
    float lightDistance = length(WorldPos.xyz - vec3(1.0f));
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}
