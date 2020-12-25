#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D depthMap;
layout(binding = 2) uniform sampler2D depthMap2;
layout(binding = 3) uniform MeshColor
{
    vec3 lightPos;
    vec3 viewPos;
} light;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

float near_plane = 1.0f;
float far_plane = 7.5f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main() 
{
   float depthValue = texture(depthMap, fragTexCoord).r;
    outColor = vec4(vec3(depthValue), 1.0); // orthographic
}


