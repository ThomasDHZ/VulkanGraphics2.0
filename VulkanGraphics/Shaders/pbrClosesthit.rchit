#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "Vertex.glsl"
#include "Lighting.glsl"
#include "Material.glsl"

layout(push_constant) uniform RayTraceConstants
{
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
    uint frame;
    int AntiAliasingCount;
    int MaxRefeflectCount;
    int  ApplyAntiAliasing;
} ConstMesh;

struct RayPayload {
	vec3 color;
	float seed;
	vec3 normal;
    int reflectCount;
    float domeSampleDelta;
    float domePhi;
    float domeTheta;
    float domeSampleCount;
};

layout(location = 0) rayPayloadInEXT RayPayload rayHitInfo;
layout(location = 1) rayPayloadEXT bool shadowed;
hitAttributeEXT vec2 attribs;

layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 2) uniform UBO 
{
    uint DirectionalLightCount;
    uint PointLightCount;
    uint SpotLightCount;
    uint SphereAreaLightCount;
    uint TubeAreaLightCount;
    uint RectangleAreaLightCount;
	float timer;
    int Shadowed;
    int temp;
} scenedata;
layout(binding = 3) buffer MeshProperties 
{
	mat4 ModelTransform;
	vec2 UVOffset;
    vec2 UVScale;
    vec2 UVFlip;
    uint MaterialIndex;
    float heightScale;
	float minLayers;
	float maxLayers;
} meshProperties[];

layout(binding = 4) buffer DirectionalLight2
{ 
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float Luminosity;
} DLight[];

layout(binding = 5) buffer PointLight2
{ 
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float Luminosity;
} PLight[];

layout(binding = 6) buffer SpotLight2
{ 
   vec3 position;
   vec3 direction;
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float cutOff;
   float outerCutOff;
   float constant;
   float linear;
   float quadratic;
   float Luminosity;
} SLight[];

layout(binding = 7, scalar) buffer Vertices { Vertex v[]; } vertices[];
layout(binding = 8) buffer Indices { uint i[]; } indices[];
layout(binding = 9) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 10) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 11) uniform sampler2D TextureMap[];
layout(binding = 12) uniform sampler3D Texture3DMap[];

layout(binding = 14) buffer SphereAreaLightBuffer {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float SphereRadius;
	float Luminosity;
} SphereLight[];

layout(binding = 15) buffer TubeAreaLightBuffer {
	vec3 StartPos;
	vec3 EndPos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
    float TubeRadius;
	float Luminosity;
} TubeLight[];

layout(binding = 16) buffer RectangleAreaLightBuffer
{
	vec3 VertexPos1;
	vec3 VertexPos2;
	vec3 VertexPos3;
	vec3 VertexPos4;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float Luminosity;
} RectangleLight[];

Vertex vertex;
#include "RTVertexBuilder.glsl"


//MaterialInfo material;
mat3 TBN;

const float PI = 3.14159265359;
vec3 getNormalFromMap(MaterialInfo material, vec2 uv);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir);
vec3 CalcNormalDirLight(vec3 normal, vec2 uv, int index);

#include "RTXRandom.glsl"
vec3 Irradiate(Vertex vertex)
{
    vec3 irradiance = vec3(0.0f);
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, vertex.normal));
    up         = normalize(cross(vertex.normal, right));

    if(rayHitInfo.domePhi < 2.0f * PI)
    {
        if(rayHitInfo.domeTheta < 0.5 * PI)
        {
            vec3 raySamplePoint = vec3(sin(rayHitInfo.domeTheta) * cos(rayHitInfo.domePhi),  sin(rayHitInfo.domeTheta) * sin(rayHitInfo.domePhi), cos(rayHitInfo.domeTheta));
            vec3 sampleVec = raySamplePoint.x * right + raySamplePoint.y * up + raySamplePoint.z * vertex.normal; 

            vec3 hitPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_RayTmaxEXT;
            vec3 origin   = hitPos.xyz + vertex.normal * 0.001f;
            vec3 rayDir   = reflect(gl_WorldRayDirectionEXT, sampleVec) * cos(rayHitInfo.domeTheta) * sin(rayHitInfo.domeTheta);

            rayHitInfo.domeSampleCount++;
            traceRayEXT(topLevelAS, gl_RayFlagsNoneEXT, 0xff, 0, 0, 0, origin, 0.001f, rayDir, 10000.0f, 0);
		    irradiance += rayHitInfo.color;
            rayHitInfo.domeTheta += rayHitInfo.domeSampleDelta;
          }
        rayHitInfo.domePhi += rayHitInfo.domeSampleDelta;
    }
    irradiance = PI * irradiance * (1.0 / float(rayHitInfo.domeSampleCount));
    return irradiance;
}

void main() 
{
   vertex = BuildVertexInfo();
   MaterialInfo material = MaterialList[meshProperties[gl_InstanceCustomIndexEXT].MaterialIndex].material;
   vertex.uv = vertex.uv + meshProperties[gl_InstanceCustomIndexEXT].UVOffset;
   vertex.uv *= meshProperties[gl_InstanceCustomIndexEXT].UVScale;
   if(meshProperties[gl_InstanceCustomIndexEXT].UVFlip.y == 1.0f)
   {
        vertex.uv.y = 1.0f - vertex.uv.y;
   }
   if(meshProperties[gl_InstanceCustomIndexEXT].UVFlip.x == 1.0f)
   {
        vertex.uv.x = 1.0f - vertex.uv.x;
   }
   if(meshProperties[gl_InstanceCustomIndexEXT].UVFlip.y == 1.0f)
   {
        vertex.uv.y = 1.0f - vertex.uv.y;
   }

   const vec3 T = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.tangent));
   const vec3 B = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.BiTangant));
   const vec3 N = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vertex.normal);
   TBN = transpose(mat3(T, B, N));
   
   vec3 result = vec3(0.0f);
   vec3 normal = vertex.normal;
   vec3 ViewPos  = ConstMesh.CameraPos;
   vec3 FragPos2  = vertex.pos;
   vec3 viewDir = normalize(ViewPos - FragPos2);
    if(material.NormalMapID != 0)
    {
        ViewPos  = TBN * ConstMesh.CameraPos;
        FragPos2  = TBN * vertex.pos;
    }

    if(material.NormalMapID != 0)
    {
//        if(material.DepthMapID != 0)
//        {
//            texCoords = ParallaxMapping(material, texCoords,  viewDir);       
//            if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
//            {
//              discard;
//            }
//        }
        normal = texture(TextureMap[material.NormalMapID], vertex.uv).rgb;
        normal = normalize(normal * 2.0 - 1.0);
     }

   vec3 albedo     = texture(TextureMap[material.AlbedoMapID], vertex.uv).rgb;
   float metallic  = texture(TextureMap[material.MatallicMapID], vertex.uv).r;
   float roughness = texture(TextureMap[material.RoughnessMapID], vertex.uv).r;
   float ao        = texture(TextureMap[material.AOMapID], vertex.uv).r;

   vec3 Lo = vec3(0.0);
   for(int x = 0; x < scenedata.DirectionalLightCount; x++)
   {
        Lo += CalcNormalDirLight(normal, vertex.uv, x);
   }

    vec3 V = ConstMesh.CameraPos;
    if (material.NormalMapID != 0)
    {

        V = TBN * ConstMesh.CameraPos;
    }

        vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    vec3 kS = fresnelSchlick(max(dot(normal, V), 0.0), F0); 
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = Irradiate(vertex);
    vec3 diffuse      = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * ao;
//    // vec3 ambient = vec3(0.002);
//    
     vec3 color = ambient + Lo;
//
    rayHitInfo.color = Irradiate(vertex);
	//rayHitInfo.distance = gl_RayTmaxNV;
	rayHitInfo.normal = vertex.normal;
}

vec3 CalcNormalDirLight(vec3 N, vec2 uv, int index)
{
   MaterialInfo material = MaterialList[meshProperties[gl_InstanceCustomIndexEXT].MaterialIndex].material;
   vec3 albedo     = texture(TextureMap[material.AlbedoMapID], vertex.uv).rgb;
   float metallic  = texture(TextureMap[material.MatallicMapID], vertex.uv).r;
   float roughness = texture(TextureMap[material.RoughnessMapID], vertex.uv).r;
   float ao        = texture(TextureMap[material.AOMapID], vertex.uv).r;

    vec3 LightPos = DLight[index].direction;
    vec3 V = ConstMesh.CameraPos;
    vec3 FragPos2 = vertex.pos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * DLight[index].direction;
        V = TBN * ConstMesh.CameraPos;
        FragPos2 = TBN * vertex.pos;
    }
    vec3 ViewDir = normalize(V - FragPos2);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    vec3 L = normalize(-DLight[index].direction);
    vec3 H = normalize(V + L);

    vec3 radiance = DLight[index].diffuse;

    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);    
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
    vec3 nominator    = NDF * G * F;
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = nominator / denominator;
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	                
            
    float NdotL = max(dot(N, L), 0.0);        
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    return Lo;
}

vec3 getNormalFromMap(MaterialInfo material, vec2 uv)
{
   const vec3 T = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.tangent));
   const vec3 B = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.BiTangant));
   const vec3 N = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vertex.normal);
   TBN = transpose(mat3(T, B, N));

    vec3 normal = texture(TextureMap[material.NormalMapID], uv).xyz;
         normal = normalize(normal * 2.0 - 1.0);
    
    return TBN * normal;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}  

vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir)
{
    const float heightScale = meshProperties[gl_InstanceCustomIndexEXT].heightScale;
    const float minLayers = meshProperties[gl_InstanceCustomIndexEXT].minLayers;
    const float maxLayers = meshProperties[gl_InstanceCustomIndexEXT].maxLayers;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    viewDir.y = -viewDir.y;
    vec2 P = viewDir.xy / viewDir.z * heightScale;
    vec2 deltaTexCoords = P / numLayers;

    vec2  currentTexCoords = texCoords;
    float currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;

    while (currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(TextureMap[material.DepthMapID], prevTexCoords).r - currentLayerDepth + layerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

//
//vec3 Irradiate(Vertex vertex)
//{
//    vec3 irradiance = vec3(0.0f);
//    float Samples = 0.0f;
//    float sampleDelta = 0.025;
//    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
//    {
//        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
//        {
//            vec3 hitPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_RayTmaxEXT;
//            vec3 origin   = hitPos.xyz + vertex.normal * 0.001f;
//            vec3 rayDir   = reflect(gl_WorldRayDirectionEXT, vertex.normal) * cos(theta) * sin(theta);
//
//            rayHitInfo.reflectCount++;
//            traceRayEXT(topLevelAS, gl_RayFlagsNoneEXT, 0xff, 0, 0, 0, origin, 0.001f, rayDir, 10000.0f, 0);
//		    irradiance += rayHitInfo.color;
//        }
//    }
//    irradiance = PI * irradiance * (1.0 / float(Samples));
//    return irradiance;
//}