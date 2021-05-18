#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

layout(push_constant) uniform ConstDepthDebugProperites
{
	float NearPlain;
    float FarPlain;
} DepthDebugProperites;


layout(location = 0) in vec2 TexCoords;
layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D DepthTexture;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * DepthDebugProperites.NearPlain * DepthDebugProperites.FarPlain) / (DepthDebugProperites.FarPlain + DepthDebugProperites.NearPlain - z * (DepthDebugProperites.FarPlain - DepthDebugProperites.NearPlain));	
}

void main() 
{
    float depthValue = texture(DepthTexture, TexCoords).r;
    FragColor = vec4(vec3(LinearizeDepth(depthValue) / DepthDebugProperites.FarPlain), 1.0); 
}