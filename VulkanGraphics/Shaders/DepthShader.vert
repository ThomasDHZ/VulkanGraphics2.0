#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "SceneProperties.glsl"
#include "MeshProperties.glsl"

layout(push_constant) uniform LightSceneInfo
{
	uint MeshIndex;
    mat4 lightSpaceMatrix;
} scene;


layout(binding = 0) uniform SceneDataBuffer { SceneProperties sceneData; } sceneBuffer;
layout(binding = 1) buffer MeshPropertiesBuffer { MeshProperties meshProperties; } meshBuffer[];
layout(binding = 2) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 3) uniform sampler2D TextureMap[];

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec4 aBitangent;

void main() 
{
    gl_Position = scene.lightSpaceMatrix * meshBuffer[scene.MeshIndex].meshProperties.ModelTransform * meshBuffer[scene.MeshIndex].meshProperties.MeshTransform * vec4(inPosition, 1.0);
}