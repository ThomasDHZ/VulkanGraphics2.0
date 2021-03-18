#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshIndex;
	uint MaterialIndex;
    vec2 UVOffset;
} Mesh;

struct Vertex
{
  vec3 pos;
  float padding1;
  vec3 normal;
  float padding2;
  vec2 uv;
  vec2 padding3;
  vec4 tangent;
  vec4 BiTangant;
  vec4 Color;
  ivec4 BoneID;
  vec4 BoneWeights;
 };

layout(binding = 3, scalar) buffer Vertices
{
  Vertex v[];
}
vertices[];
layout(binding = 2) uniform UniformBufferObject {
	mat4 viewInverse;
	mat4 projInverse;
	mat4 view;
	mat4 proj;
	DirectionalLight dlight;
	vec3 viewPos;
	PointLight plight;
	float timer;
    int temp;
} ubo;
layout(binding = 3) buffer MeshProperties 
{
	mat4 ModelTransform;
	mat4 BoneTransform[100];
	vec2 UVOffset;
} meshProperties[];

layout(binding = 6) buffer Transform { mat4 Transform; } MeshTransform[];

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec4 aBitangent;
layout (location = 5) in vec4 Color;
layout (location = 6) in ivec4 BoneID;
layout (location = 7) in vec4 BoneWeights;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 TexCoords;
layout(location = 2) out vec3 Normal;
layout(location = 3) out mat3 TBN;

void main() 
{
    FragPos = vec3(meshProperties[Mesh.MeshIndex].ModelTransform * vec4(aPos, 1.0));    
    TexCoords = aTexCoords;
    Normal = aNormal;

    vec3 T = normalize(mat3(meshProperties[Mesh.MeshIndex].ModelTransform) * vec3(aTangent));
    vec3 B = normalize(mat3(meshProperties[Mesh.MeshIndex].ModelTransform) * vec3(aBitangent));
    vec3 N = normalize(mat3(meshProperties[Mesh.MeshIndex].ModelTransform) * aNormal);
    TBN = transpose(mat3(T, B, N));

    gl_Position = ubo.proj * ubo.view * meshProperties[Mesh.MeshIndex].ModelTransform * MeshTransform[Mesh.MeshIndex].Transform * vec4(aPos, 1.0);
}
