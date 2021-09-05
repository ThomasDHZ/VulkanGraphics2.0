#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "Structs.glsl"

#define MAXPOINTLIGHTS 4

layout(binding = 1) uniform sampler2D DiffuseMap;
layout(binding = 2) uniform sampler2D SpecularMap;
layout(binding = 3) uniform sampler2D normalMap;
layout(binding = 4) uniform sampler2D depthMap;
layout(binding = 5) uniform sampler2D AlphaMap;
layout(binding = 6) uniform sampler2D EmissionMap;
layout(binding = 7) uniform sampler2D ReflectionMap;
layout(binding = 8) uniform samplerCube SkyBox;
layout(binding = 9) uniform MeshProperties
{
    Material material;
} meshProperties;
layout(binding = 10) uniform Light
{
    DirectionalLightStruct DLight;
    PointLightStruct PLight[MAXPOINTLIGHTS];
    SpotLightStruct SLight;
    vec3 viewPos;
} light;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BloomColor;

void main()
{  
    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}