#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"

layout(binding = 0) uniform sampler2D SSAOTexture;

layout(location = 0) in vec2 TexCoords;
layout(location = 0) out vec4 outColor;

void main() 
{
    vec2 texelSize = 1.0 / vec2(textureSize(SSAOTexture, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(SSAOTexture, TexCoords + offset).r;
        }
    }

    outColor = vec4(vec3(result / (4.0 * 4.0)), 1.0f);
}