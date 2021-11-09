#version 460

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable
#extension GL_EXT_multiview : enable

layout(binding = 0) uniform samplerCube CubeMap;

layout(location = 0) in vec3 WorldPos;
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 BloomColor;

const float PI = 3.14159265359;

void main() 
{
    vec3 color = texture(CubeMap, WorldPos).rgb;
    outColor = vec4(color, 1.0f);
    BloomColor = vec4(vec3(0.0f, 0.0f, 0.0f), 1.0f);
}