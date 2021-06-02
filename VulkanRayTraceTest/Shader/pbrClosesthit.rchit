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

Vertex vertex;
MaterialInfo material;
mat3 TBN;

const float PI = 3.14159265359;
vec3 RTXShadow(vec3 LightResult, vec3 LightDirection, float LightDistance);
#include "RTVertexBuilder.glsl"
#include "BlinePhongLighting.glsl"
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 getNormalFromMap(Vertex vertex, MaterialInfo material);

void main()
{
   vertex = BuildVertexInfo();
   material = MaterialList[meshProperties[gl_InstanceCustomIndexEXT].MaterialIndex].material;
   const vec3 T = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.tangent));
   const vec3 B = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.BiTangant));
   const vec3 N = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vertex.normal);
   TBN = transpose(mat3(T, B, N));

    vec3 result = vec3(0.0f);
    vec3 baseColor = vec3(0.0f);
    vec3 normal = vertex.normal;
    vec3 ViewPos  = ConstMesh.CameraPos;
    vec3 FragPos  = vertex.pos;
    if(material.NormalMapID != 0)
    {
        ViewPos  = TBN * ConstMesh.CameraPos;
        FragPos  = TBN * vertex.pos;
    }
    const vec3 viewDir = normalize(ViewPos - FragPos);

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
     for(int x = 0; x < scenedata.DirectionalLightCount; x++)
     {
        baseColor += CalcNormalDirLight(FragPos, normal, vertex.uv, x);
     }
     for(int x = 0; x < scenedata.PointLightCount; x++)
     {
       // result += CalcNormalPointLight(FragPos, normal, vertex.uv, x);   
     }
     //result +=  CalcNormalSpotLight(FragPos, scenedata.sLight, normal, texCoords);
       if(material.Reflectivness > 0.0f &&
       rayHitInfo.reflectCount != 13)
    {
        vec3 hitPos = gl_WorldRayOriginNV + gl_WorldRayDirectionNV * gl_RayTmaxNV;
        vec3 origin   = hitPos.xyz + vertex.normal * 0.001f;
        vec3 rayDir   = reflect(gl_WorldRayDirectionEXT, vertex.normal);

        rayHitInfo.reflectCount++;
        traceRayEXT(topLevelAS, gl_RayFlagsNoneNV, 0xff, 0, 0, 0, origin, 0.001f, rayDir, 10000.0f, 0);
		result = mix(baseColor, rayHitInfo.color, material.Reflectivness); 
    }
    else
	{
        debugPrintfEXT("Temp: %f \n", result.r);
        result = baseColor;
        rayHitInfo.reflectCount = 20;
	}

//    	for (int x = 0; x < 20; x++) 
//	{
//		traceRayEXT(topLevelAS, gl_RayFlagsNoneEXT, cullMask, 0, 0, 0, origin.xyz, tmin, direction.xyz, tmax, 0);
//		if(x == 0)
//		{
//			BaseColor = rayPayload.color;
//		}
//		finalColor = mix(BaseColor, rayPayload.color, rayPayload.MaterialReflect);
//		
//
//		if (rayPayload.reflector == 1.0f) 
//		{
//			const vec4 hitPos = origin + direction * rayPayload.distance;
//			origin.xyz = hitPos.xyz + rayPayload.normal * 0.001f;
//			direction.xyz = reflect(direction.xyz, rayPayload.normal);
//		} 
//		else 
//		{
//			break;
//		}
//	}

    rayHitInfo.color = result;
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

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
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
  
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;  
        currentLayerDepth += layerDepth;  
    }
    
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(TextureMap[material.DepthMapID], prevTexCoords).r - currentLayerDepth + layerDepth;
 
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
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
   const vec3 tangentNormal = texture(TextureMap[material.NormalMapID], vertex.uv).xyz * 2.0 - 1.0;

   const vec3 T = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.tangent));
   const vec3 N = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * tangentNormal);
   const vec3 B = normalize(cross(N, T));
   const mat3 TBN = mat3(T, B, N);

   return normalize(TBN * tangentNormal);
}