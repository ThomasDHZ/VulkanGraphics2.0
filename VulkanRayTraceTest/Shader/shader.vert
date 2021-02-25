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
    mat4 model;
    mat4 view;
    mat4 proj;
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
   BoneTransform =  ubo.BoneTransform[BoneID[0]] * BoneWeights[0];
   BoneTransform += ubo.BoneTransform[BoneID[1]] * BoneWeights[1];
   BoneTransform += ubo.BoneTransform[BoneID[2]] * BoneWeights[2];
   BoneTransform += ubo.BoneTransform[BoneID[3]] * BoneWeights[3];
   
    vec4 BonePosition = BoneTransform * vec4(aPos, 1.0);

    FragPos = vec3(ubo.model * BonePosition);    
    UV = aTexCoords;
    Normal = normalize(transpose(inverse(mat3(ubo.view * ubo.model * BoneTransform))) * aNormal);

    gl_Position = ubo.proj * ubo.view * ubo.model * BonePosition;
}

