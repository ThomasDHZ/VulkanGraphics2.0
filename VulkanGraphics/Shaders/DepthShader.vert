#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "material.glsl"

layout(push_constant) uniform LightSceneInfo
{
	uint MeshIndex;
    mat4 MeshView;
	mat4 LightProjection;
    mat4 LightView;
    vec3 CameraPos;
} scene;


layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 viewInverse;
	mat4 projInverse;
	mat4 view;
	mat4 proj;
    vec3 viewPos;
    uint DirectionalLightCount;
    uint PointLightCount;
    uint SpotLightCount;
	float timer;
    int Shadowed;
    int temp;
} ubo;

layout(binding = 1) buffer MeshProperties 
{
	mat4 ModelTransform;
	vec2 UVOffset;
    vec2 UVScale;
    vec2 UVFlip;
    uint MaterialIndex;
    float heightScale;
	float minLayers;
	float maxLayers;
} meshProperties[];

layout(binding = 2) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 3) uniform sampler2D TextureMap[];

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec4 aBitangent;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 TexCoords;
layout(location = 2) out vec3 Normal;
layout(location = 3) out vec3 Tangent;
layout(location = 4) out vec3 BiTangent;

void main() 
{
    mat4 LightMatrix = scene.LightProjection * scene.LightView;
    gl_Position = LightMatrix * scene.MeshView * meshProperties[scene.MeshIndex].ModelTransform * MeshTransform[scene.MeshIndex].Transform * vec4(inPosition, 1.0);

}