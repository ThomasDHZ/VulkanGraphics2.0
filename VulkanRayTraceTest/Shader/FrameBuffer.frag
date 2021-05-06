#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"

layout(binding = 0) uniform sampler2D FrameBufferTexture;
layout(binding = 1) uniform sampler2D BloomTexture;

layout(location = 0) in vec2 TexCoords;
layout(location = 0) out vec4 outColor;


void main() 
{
    outColor = vec4(texture(FrameBufferTexture, TexCoords));
}
