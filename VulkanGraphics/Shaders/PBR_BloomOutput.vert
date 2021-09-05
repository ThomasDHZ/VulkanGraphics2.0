#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 BoneID;
layout (location = 6) in vec4 BoneWeights;

layout(location = 0) out vec3 WorldPos;
layout(location = 1) out vec2 TexCoords;
layout(location = 2) out vec3 Normal;
layout(location = 3) out mat3 TBN;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 lightSpaceMatrix;
} ubo;

void main()
{
    vec3 T = normalize(mat3(ubo.model) * aTangent);
    vec3 B = normalize(mat3(ubo.model) * aBitangent);
    vec3 N = normalize(mat3(ubo.model) * aNormal);
    TBN = transpose(mat3(T, B, N));

    TexCoords = aTexCoords;
    WorldPos = vec3(ubo.model * vec4(aPos, 1.0));
    Normal = mat3(ubo.model) * aNormal;   

    gl_Position =  ubo.projection * ubo.view * vec4(WorldPos, 1.0);
}