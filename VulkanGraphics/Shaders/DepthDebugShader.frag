#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "Structs.glsl"

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
    float EmissionStrength;
} meshProperties;
layout(binding = 10) uniform Light
{
    DirectionalLightStruct dLight;
    PointLightStruct pLight[1];
    SpotLightStruct sLight;
    vec3 viewPos;
} light;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 FragColor;

float near = 0.1; 
float far = 100.0; 
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; 
    return (2.0 * near * far) / (far + near - z * (far - near));	
} 

void main() 
{
    float depth = LinearizeDepth(gl_FragCoord.z) / far; 
    FragColor = vec4(vec3(depth), 1.0);
}

