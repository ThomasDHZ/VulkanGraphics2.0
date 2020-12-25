#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 lightSpaceMatrix;
} ubo;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 TexCoords;
layout(location = 3) out vec4 FragPosLightSpace;
void main() 
{
    FragPos = vec3(ubo.model * vec4(aPos, 1.0));
    Normal = transpose(inverse(mat3(ubo.model))) * aNormal;
    TexCoords = aTexCoords;
    FragPosLightSpace = ubo.lightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(aPos, 1.0);
}

