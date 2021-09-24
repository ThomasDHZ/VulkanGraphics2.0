#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"
#include "material.glsl"
#include "vertex.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshIndex;
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} ConstMesh;

struct VertexData
{
	vec3 Position;
	vec3 Normal;
	vec2 UV;
	vec3 Tangent;
	vec3 BiTangent;
};

layout(binding = 0) uniform samplerCube CubeMap;

layout(location = 0) in vec3 FragPos;
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBloom;

void main() 
{
   outColor = vec4(1.0f);
   outBloom = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}