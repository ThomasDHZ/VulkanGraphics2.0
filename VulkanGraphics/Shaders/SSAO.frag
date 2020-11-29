#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D FrameBufferTexture;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

void main() 
{
    outColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}