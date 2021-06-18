#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "Vertex.glsl"
#include "Lighting.glsl"
#include "Material.glsl"
#include "RTXRandom.glsl"

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
    int reflectCount2;
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
vec3 getNormalFromMap(PBRMaterial material, Vertex vertex);
vec3 Irradiate(Vertex vertex, float metallic, uint SampleCount);

// ----------------------------------------------------------------------------
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
	float a = roughness*roughness;
	
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	
	vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent   = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);
	
	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
}

vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

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

#include "PBRFunctions.glsl"

void main()
{

   Vertex vertex = BuildVertexInfo();
   PBRMaterial material = PBRMaterialBuilder(MaterialList[meshProperties[gl_InstanceCustomIndexEXT].MaterialIndex].material, vertex.uv);

   vec3 N = vertex.normal;
   if(MaterialList[meshProperties[gl_InstanceCustomIndexEXT].MaterialIndex].material.NormalMapID != 0)
   {
      N = getNormalFromMap(material, vertex);
   }
   vec3 V = normalize(scenedata.viewPos - vertex.pos);

   vec3 F0 = vec3(0.04); 
   F0 = mix(F0, material.Albedo, material.Metallic);

    vec3 Lo = vec3(0.0);
    Lo += CalcDirectionalLight(material, F0, V, N);
    Lo += CalcPointLight(material, vertex.pos, F0, V, N);
    Lo += CalcSpotLight(material, vertex.pos, F0, V, N);

   vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, material.Roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - material.Metallic;	  
    
    uint SampleCount =  256;
    vec3 irradiance = vec3(0.0f);
  //  vec3 irradiance = Irradiate(vertex, material.Metallic, SampleCount);
    
    vec3 diffuse  = irradiance * material.Albedo;

    float totalWeight = 0.0f;
    vec3 prefilter = vec3(0.0f);
    vec3 specular = vec3(0.0f);
    if(rayHitInfo.reflectCount2 != SampleCount)
    {
         float r1        = rnd(rayHitInfo.seed);
         float r2        = rnd(rayHitInfo.seed);
         float sq        = sqrt(1.0 - r2);
         float phi       = 2 * PI * r1;
         vec3 scatter   = vec3(cos(phi) * sq, sin(phi) * sq, sqrt(r2));

        vec2 Xi = Hammersley(rayHitInfo.reflectCount2, SampleCount);
        vec3 H = ImportanceSampleGGX(Xi, N, material.Roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        vec3 rayDir = vec3(0.0);
        if(NdotL > 0.0)
        {
            float D   = DistributionGGX(N, H, material.Roughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001; 

            float saTexel  = 4.0 * PI / 6.0f;
            float saSample = 1.0 / (float(SampleCount) * pdf + 0.0001);
            float mipLevel = material.Roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
            scatter *= mipLevel;

            rayDir = L * scatter * NdotL;
            totalWeight += NdotL;
        }
        prefilter /= totalWeight;

        vec3 hitPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_RayTmaxEXT;
        vec3 origin   = hitPos.xyz + vertex.normal * 0.001f;
        rayDir =  reflect(gl_WorldRayDirectionEXT * rayDir, vertex.normal) * material.Roughness * 4.0f;
        traceRayEXT(topLevelAS, gl_RayFlagsNoneEXT, 0xff, 0, 0, 0, origin, 0.001f, rayDir, 100.0f, 0);
        prefilter += rayHitInfo.color;
    }

    vec2 brdf = vec2(max(dot(N, V), 0.0), material.Roughness).rg;
    specular = prefilter * (F * brdf.x + brdf.y);

   vec3 ambient = (kD * diffuse + specular) * material.AmbientOcclusion;
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

vec3 getNormalFromMap(PBRMaterial material, Vertex vertex)
{
    vec3 T = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.tangent));
    vec3 B = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.BiTangant));
    vec3 N = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vertex.normal);
    mat3 TBN = transpose(mat3(T, B, N));

    vec3 normal = material.Normal;
         normal = normalize(normal * 2.0 - 1.0);
    
    return TBN * normal;
}

vec3 Irradiate(Vertex vertex, float metallic, uint SampleCount)
{
    const uint MaxReflectCount  = SampleCount; 
    vec3 irradiance = vec3(0.0f);
    if(rayHitInfo.reflectCount != MaxReflectCount)
    {
        float r1        = rnd(rayHitInfo.seed);
        float r2        = rnd(rayHitInfo.seed);
        float sq        = sqrt(1.0 - r2);
        float phi       = 2 * PI * r1;
 
        vec3 hitPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_RayTmaxEXT;
        vec3 origin   = hitPos.xyz + vertex.normal * 0.001f;
        vec3 rayDir   = reflect(gl_WorldRayDirectionEXT * (vec3(cos(phi) * sq, sin(phi) * sq, sqrt(r2))), vertex.normal);

        rayHitInfo.reflectCount++;
        traceRayEXT(topLevelAS, gl_RayFlagsNoneEXT, 0xff, 0, 0, 0, origin, 0.001f, rayDir, 10000.0f, 0);
		irradiance = mix(irradiance, rayHitInfo.color, metallic); 
    }

    return irradiance;
}