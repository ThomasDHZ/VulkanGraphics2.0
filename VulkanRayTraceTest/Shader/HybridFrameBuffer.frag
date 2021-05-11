#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"

layout(binding = 0) uniform sampler2D FrameBufferTexture;
layout(binding = 1) uniform sampler2D RTXShadowTexture;
layout(binding = 2) uniform sampler2D RTXReflectionTexture;
layout(binding = 3) uniform sampler2D RTXSSA0Texture;
layout(binding = 4) uniform sampler2D BloomTexture;

layout(location = 0) in vec2 TexCoords;
layout(location = 0) out vec4 outColor;

const float Gamma = 2.2f;
void main() 
{
    vec3 Color = texture(FrameBufferTexture, TexCoords).rgb;
    if(texture(RTXShadowTexture, TexCoords).rgb == vec3(1.0f, 0.0f, 0.0f))
    {
        Color *= vec3(0.3f);
    }
    if(texture(RTXReflectionTexture, TexCoords).rgb != vec3(0.0f))
    {
        Color = texture(RTXReflectionTexture, TexCoords).rgb;
    }
    vec3 result = pow(Color, vec3(1.0 / Gamma));
    outColor = vec4(Color, 1.0f);
}
