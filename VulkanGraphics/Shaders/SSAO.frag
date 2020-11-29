#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D gPositionTexture;
layout(binding = 1) uniform sampler2D gNormalTexture;
layout(binding = 2) uniform sampler2D NoiseTexture;
layout(binding = 3) uniform SSAOConfig
{
    int KernelSize;
    float Radius;
    float Bias;
} config;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

void main() 
{
    vec3 fragPos = texture(gNormalTexture, fragTexCoord).xyz;
    outColor = vec4(fragPos, 1.0f);
}