

#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
        mat4 Lightmodel;
    mat4 Lightview;
    mat4 Lightproj;
    mat4 BoneTransform[100];
} ubo;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 BoneID;
layout (location = 6) in vec4 BoneWeights;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec4 LightSpaceMatrix;
layout(location = 2) out vec2 TexCoords;
layout(location = 3) out vec3 Normal;
layout(location = 4) out mat3 TBN;

void main() 
{
   mat4 BoneTransform = mat4(1.0f);
   BoneTransform =  ubo.BoneTransform[BoneID[0]] * BoneWeights[0];
   BoneTransform += ubo.BoneTransform[BoneID[1]] * BoneWeights[1];
   BoneTransform += ubo.BoneTransform[BoneID[2]] * BoneWeights[2];
   BoneTransform += ubo.BoneTransform[BoneID[3]] * BoneWeights[3];
    vec4 BonePosition = BoneTransform * vec4(inPosition, 1.0);

    LightSpaceMatrix = ubo.Lightproj * ubo.Lightview * ubo.Lightmodel * vec4(inPosition, 1.0);
    FragPos = vec3(ubo.model * BonePosition);    
    TexCoords = inTexCoord;
    Normal = normalize(transpose(inverse(mat3(ubo.view * ubo.model * BoneTransform))) * aNormal);

    vec3 T = normalize(mat3(ubo.model) * aTangent);
    vec3 B = normalize(mat3(ubo.model) * aBitangent);
    vec3 N = normalize(mat3(ubo.model) * normalize(transpose(inverse(mat3(ubo.view * ubo.model * BoneTransform))) * aNormal));
    TBN = transpose(mat3(T, B, N));

    gl_Position = ubo.proj * ubo.view * ubo.model * BonePosition;
}

