#version 450
#extension GL_ARB_separate_shader_objects : enable

const int UseDiffuseMapBit    = 0x00000001;
const int UseSpecularMapBit   = 0x00000002;
const int UseNormalMapBit     = 0x00000004;
const int UseDepthMapBit      = 0x00000008;
const int UseAlphaMapBit      = 0x00000016;
const int UseEmissionMapBit   = 0x00000032;
const int UseSkyBoxBit        = 0x00000064;

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
   // MapBits mapBitsFlags;
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

vec3 CalcDirLight(DirectionalLightStruct light, vec3 normal, vec3 viewDir);

void main()
{  
    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0);
}
