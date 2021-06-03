#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable
#extension GL_NV_ray_tracing : enable
#include "Vertex.glsl"
#include "Lighting.glsl"
#include "Material.glsl"

layout(push_constant) uniform RayTraceCamera
{
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
    uint frame;
} ConstMesh;

struct RayPayload {
	vec3 color;
	uint seed;
	vec3 normal;
    int reflectCount;
};

layout(location = 0) rayPayloadInEXT RayPayload rayHitInfo;
layout(location = 1) rayPayloadEXT bool shadowed;
hitAttributeEXT vec2 attribs;

layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 2) uniform UBO 
{
	DirectionalLight dlight;
	PointLight plight[5];
	SpotLight sLight;
    mat4 viewInverse;
	mat4 projInverse;
	mat4 view;
	mat4 proj;
    vec3 viewPos;
    uint DirectionalLightCount;
    uint PointLightCount;
    uint SpotLightCount;
	float timer;
    int Shadowed;
    int temp;
} scenedata;
layout(binding = 3) buffer MeshProperties 
{
	mat4 ModelTransform;
	mat4 BoneTransform[100];
	vec2 UVOffset;
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
} SLight[];

layout(binding = 7, scalar) buffer Vertices { Vertex v[]; } vertices[];
layout(binding = 8) buffer Indices { uint i[]; } indices[];
layout(binding = 9) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 10) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 11) uniform sampler2D TextureMap[];
layout(binding = 12) uniform sampler3D Texture3DMap[];

const float PI = 3.14159265359;
vec3 RTXShadow(vec3 LightResult, vec3 LightDirection, float LightDistance);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 getNormalFromMap(Vertex vertex, MaterialInfo material);

Vertex BuildVertexInfo()
{
	Vertex vertex;
	const ivec3 index = ivec3(indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID],
		                      indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 1],
		                      indices[gl_InstanceCustomIndexEXT].i[3 * gl_PrimitiveID + 2]);

	const Vertex v0 = vertices[gl_InstanceCustomIndexEXT].v[index.x];
	const Vertex v1 = vertices[gl_InstanceCustomIndexEXT].v[index.y];
	const Vertex v2 = vertices[gl_InstanceCustomIndexEXT].v[index.z];

	const vec3 barycentricCoords = vec3(1.0f - attribs.x - attribs.y, attribs.x, attribs.y);

	vertex.pos = v0.pos * barycentricCoords.x + v1.pos * barycentricCoords.y + v2.pos * barycentricCoords.z;
	vertex.pos = vec3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform * vec4(vertex.pos, 1.0));

	vertex.normal = v0.normal * barycentricCoords.x + v1.normal * barycentricCoords.y + v2.normal * barycentricCoords.z;
	vertex.normal = mat3(transpose(inverse(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform))) * vertex.normal;

	vertex.uv = v0.uv * barycentricCoords.x + v1.uv * barycentricCoords.y + v2.uv * barycentricCoords.z;
	vertex.uv += meshProperties[gl_InstanceCustomIndexEXT].UVOffset;

	vertex.tangent = v0.tangent * barycentricCoords.x + v1.tangent * barycentricCoords.y + v2.tangent * barycentricCoords.z;
	vertex.BiTangant = v0.BiTangant * barycentricCoords.x + v1.BiTangant * barycentricCoords.y + v2.BiTangant * barycentricCoords.z;

	const vec3 color = v0.Color.xyz * barycentricCoords.x + v1.Color.xyz * barycentricCoords.y + v2.Color.xyz * barycentricCoords.z;
	vertex.Color = vec4(color, 1.0f);

	return vertex;
}

void main()
{
   Vertex vertex = BuildVertexInfo();
   MaterialInfo material = MaterialList[meshProperties[gl_InstanceCustomIndexEXT].MaterialIndex].material;
   
   vec3 albedo     = texture(TextureMap[material.AlbedoMapID], vertex.uv).rgb;
   float metallic  = texture(TextureMap[material.MatallicMapID], vertex.uv).r;
   float roughness = texture(TextureMap[material.RoughnessMapID], vertex.uv).r;
   float ao        = texture(TextureMap[material.AOMapID], vertex.uv).r;

   vec3 N = getNormalFromMap(vertex, material);
   vec3 V = normalize(scenedata.viewPos - vertex.pos);
   vec3 R = reflect(-V, N); 

   vec3 F0 = vec3(0.04f);
   vec3 Lo = vec3(0.0f);
   F0 = mix(F0, albedo, metallic);

      for(int x = 0; x < scenedata.DirectionalLightCount; x++)
    {
        vec3 L = normalize(-DLight[x].direction);
        vec3 H = normalize(V + L);

        vec3 radiance = DLight[x].diffuse;

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
    }

    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	
    
    vec3 irradiance = vec3(0.4f);
    if(metallic > 0.0f)
    {
        vec3 hitPos = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_RayTmaxNV;
        vec3 origin   = hitPos.xyz + vertex.normal * 0.001f;
        vec3 rayDir   = reflect(gl_WorldRayDirectionEXT, vertex.normal);

        rayHitInfo.reflectCount++;
        traceRayEXT(topLevelAS, gl_RayFlagsNoneNV, 0xff, 0, 0, 0, origin, 0.001f, rayDir, 10000.0f, 0);
    
		irradiance = rayHitInfo.color; 
    }

    vec3 diffuse      = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * ao;
   vec3 color = ambient + Lo;

   rayHitInfo.color = color;
   rayHitInfo.normal = vertex.normal;
}

vec3 RTXShadow(vec3 LightResult, vec3 LightSpecular, vec3 LightDirection, float LightDistance)
{
     if(scenedata.Shadowed == 1)
     {
        float tmin = 0.001;
	    float tmax = LightDistance;
	    vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
	    shadowed = true;  
	    traceRayEXT(topLevelAS, gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, LightDirection, tmax, 1);
	    if (shadowed) 
        {
            LightResult *= 0.3f;
	    }
        else
        {
           LightResult += LightSpecular;
        }
    }
    else
    {
           LightResult += LightSpecular;
    }
    return LightResult;
}

//vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
//{ 
//    const float heightScale = meshProperties[gl_InstanceCustomIndexEXT].heightScale;
//    const float minLayers = meshProperties[gl_InstanceCustomIndexEXT].minLayers;
//    const float maxLayers = meshProperties[gl_InstanceCustomIndexEXT].maxLayers;
//
//    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
//    float layerDepth = 1.0 / numLayers;
//    float currentLayerDepth = 0.0;
//
//    viewDir.y = -viewDir.y; 
//    vec2 P = viewDir.xy / viewDir.z * heightScale; 
//    vec2 deltaTexCoords = P / numLayers;
//  
//    vec2  currentTexCoords     = texCoords;
//    float currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;
//      
//    while(currentLayerDepth < currentDepthMapValue)
//    {
//        currentTexCoords -= deltaTexCoords;
//        currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;  
//        currentLayerDepth += layerDepth;  
//    }
//    
//    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
//
//    float afterDepth  = currentDepthMapValue - currentLayerDepth;
//    float beforeDepth = texture(TextureMap[material.DepthMapID], prevTexCoords).r - currentLayerDepth + layerDepth;
// 
//    float weight = afterDepth / (afterDepth - beforeDepth);
//    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
//
//    return finalTexCoords;
//}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
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

vec3 getNormalFromMap(Vertex vertex, MaterialInfo material)
{
    vec3 T = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.tangent));
    vec3 B = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.BiTangant));
    vec3 N = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vertex.normal);
    mat3 TBN = transpose(mat3(T, B, N));

    vec3 normal = texture(TextureMap[material.NormalMapID], vertex.uv).rgb;
         normal = normalize(normal * 2.0 - 1.0);
    
    return TBN * normal;
}