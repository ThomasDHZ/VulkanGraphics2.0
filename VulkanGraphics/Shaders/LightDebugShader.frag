#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "SceneProperties.glsl"
#include "MeshProperties.glsl"
#include "lighting.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshIndex;
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} Mesh;

layout(binding = 0) uniform SceneDataBuffer { SceneProperties sceneData; } sceneBuffer;
layout(binding = 1) buffer MeshPropertiesBuffer { MeshProperties meshProperties; } meshBuffer[];

layout(location = 0) in vec3 FragPos;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBloom;

void main() 
{
    outColor = vec4(1.0f, 1.0f, 1.0f, .5f);
    outBloom = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}