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
layout(binding = 8) uniform sampler2D ShadowMap;
layout(binding = 9) uniform samplerCube SkyBox;
layout(binding = 10) uniform MeshProperties
{
    Material material;
    vec2 UVOffset;
     int UseDiffuseMapBit;
     int UseSpecularMapBit;
     int UseNormalMapBit;
     int UseDepthMapBit;
     int UseAlphaMapBit;
     int UseEmissionMapBit;
     int UseReflectionMapBit;
     int UseSkyBoxBit;
    float minLayers;
    float maxLayers;
    float heightScale;
    float timer;
    int ReflectSprite;
    vec2 UVScale;
} meshProperties;
layout(binding = 11) uniform Light
{
    DirectionalLightStruct dLight;
    PointLightStruct pLight[MAXPOINTLIGHTS];
    SpotLightStruct sLight;
    vec3 viewPos;
} light;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 FragColor;

float near_plane = 1.0f;
float far_plane = 100.5f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main() 
{
   float depthValue = texture(DiffuseMap, TexCoords).r;
   FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}