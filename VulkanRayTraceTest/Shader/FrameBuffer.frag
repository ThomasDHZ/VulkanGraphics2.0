#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"

layout(binding = 0) uniform sampler2D FrameBufferTexture;
layout(binding = 1) uniform sampler2D BloomTexture;

layout(location = 0) in vec2 TexCoords;
layout(location = 0) out vec4 outColor;

const float Gamma = 2.2f;
const float Exposure = 1.0f;

void main() 
{
    vec3 Color = texture(FrameBufferTexture, TexCoords).rgb;
    //Color += texture(BloomTexture, TexCoords).rgb;

    vec3 result = vec3(1.0) - exp(-Color * Exposure);
    result = pow(Color, vec3(1.0 / Gamma));
    outColor = vec4(result, 1.0f);
}
