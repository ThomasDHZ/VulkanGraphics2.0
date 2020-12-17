#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D FrameBufferTexture;
layout(binding = 1) uniform sampler2D BloomTexture;
layout(binding = 2) uniform FrameBufferSettings
{
    float Gamma;
    float HDRValue;
} settings;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

void main() 
{
   vec3 hdrColor = texture(FrameBufferTexture, fragTexCoord).rgb;
   vec3 result = vec3(1.0) - exp(-hdrColor * settings.HDRValue);
   result = pow(result, vec3(1.0f / settings.Gamma));
   outColor = vec4(result, 1.0f);
}