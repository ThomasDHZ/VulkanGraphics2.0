#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "MeshProperties.glsl"
#include "Lighting.glsl"
#include "material.glsl"
#include "vertex.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshIndex;
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} Mesh;

layout(binding = 0) buffer MeshPropertiesBuffer { MeshProperties meshProperties; } meshBuffer[];
layout(binding = 1) buffer Transform { mat4 Transform; } MeshTransform[];

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec4 aBitangent;

layout(location = 0) out vec3 FragPos;

void main() 
{
    FragPos = vec3(meshBuffer[Mesh.MeshIndex].meshProperties.ModelTransform * MeshTransform[Mesh.MeshIndex].Transform * vec4(aPos, 1.0));    

    gl_Position = Mesh.proj * Mesh.view * meshBuffer[Mesh.MeshIndex].meshProperties.ModelTransform * MeshTransform[Mesh.MeshIndex].Transform * vec4(aPos, 1.0);
}
