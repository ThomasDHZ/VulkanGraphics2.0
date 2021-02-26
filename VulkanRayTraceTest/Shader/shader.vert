#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "Lighting.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshID;
	uint ModelID;
	uint MaterialID;
} Mesh;

layout(binding = 2) uniform UniformBufferObject {
	mat4 viewInverse;
	mat4 projInverse;
    mat4 model;
    mat4 view;
    mat4 proj;
    DirectionalLight dlight;
	vec3 viewPos;
	PointLight plight;
    int vertexSize;
    mat4 PVM;
    mat4 BoneTransform[100];
} ubo;
layout(binding = 5) buffer Transform { mat4 Transform; } MeshTransform[];

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aTangent;
layout (location = 4) in vec4 aBitangent;
layout (location = 5) in vec4 Color;
layout (location = 6) in ivec4 BoneID;
layout (location = 7) in vec4 BoneWeights;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 UV;

void main() 
{
   mat4 BoneTransform = mat4(1.0f);
    vec4 BonePosition = BoneTransform * vec4(aPos, 1.0);

    FragPos = vec3(ubo.PVM * MeshTransform[Mesh.MeshID].Transform * BonePosition);  
    UV = aTexCoords;
    Normal = normalize(transpose(inverse(mat3(ubo.view * ubo.model * MeshTransform[Mesh.MeshID].Transform * BoneTransform))) * aNormal);

    gl_Position = ubo.PVM * MeshTransform[Mesh.MeshID].Transform * BonePosition;
}
