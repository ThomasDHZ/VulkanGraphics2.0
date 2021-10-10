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
    float TestRoughness;
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
layout(binding = 13, set = 0) uniform samplerCube CubeMap[];

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
#include "RTXRandom.glsl"

//MaterialInfo material;
mat3 TBN;

const float PI = 3.14159265359;

mat3 getTBNFromMap(Vertex vertex);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3 Irradiate(vec3 N);
vec3 PrefilterColor(vec3 N);
vec3 CalcDirectionalLight(vec3 F0, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic);
vec3 CalcPointLight(vec3 F0, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic);
vec3 CalcSpotLight(vec3 F0, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic);
vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir);

void main()
{		
   vertex = BuildVertexInfo();
   MaterialInfo material = MaterialList[meshProperties[gl_InstanceCustomIndexEXT].MaterialIndex].material;
   vec2 UV = vertex.uv + meshProperties[gl_InstanceCustomIndexEXT].UVOffset;

    vec3 albedo = material.Albedo; 
    if(material.AlbedoMapID != 0)
    {
        albedo = texture(TextureMap[material.AlbedoMapID], UV).rgb;
    }   

    float metallic = material.Matallic; 
    if(material.MatallicMapID != 0)
    {
        metallic = texture(TextureMap[material.MatallicMapID], UV).r;
    }

    float roughness = material.Roughness; 
    if(material.RoughnessMapID != 0)
    {
        roughness = texture(TextureMap[material.RoughnessMapID], UV).r;
    }

    float ao = material.AmbientOcclusion;
    if(material.AOMapID != 0)
    {
        ao = texture(TextureMap[material.AOMapID], UV).r;
    }

    float alpha = material.Alpha;
    if(material.Alpha != 0)
    {
        alpha = texture(TextureMap[material.AlphaMapID], UV).r;
    }

    vec3 V = normalize(ConstMesh.CameraPos - vertex.pos);
    mat3 TBN = getTBNFromMap(vertex);
    if(material.DepthMapID != 0)
    {
            UV = ParallaxMapping(material, UV,  V);       
    }

    vec3 normal = texture(TextureMap[material.NormalMapID], UV).xyz * 2.0 - 1.0;
    vec3 N = normalize(TBN * normal);
    vec3 R = reflect(-V, N); 
  
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    Lo += CalcDirectionalLight(F0, V, N, albedo, roughness, metallic);
//    Lo += CalcPointLight(F0, V, N, albedo, roughness, metallic);
//    Lo += CalcSpotLight(F0, V, N, albedo, roughness, metallic);

    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = Irradiate(vertex.normal);
    vec3 diffuse      = irradiance * albedo;
    
    vec3 prefilteredColor = PrefilterColor(N);
    vec3 specular = prefilteredColor;

    vec3 ambient = (kD * diffuse + specular) * ao;
    
    vec3 color = ambient + Lo;

    rayHitInfo.color = color;
	//rayHitInfo.distance = gl_RayTmaxNV;
	rayHitInfo.normal = vertex.normal;
}

mat3 getTBNFromMap(Vertex vertex)
{
    vec3 T = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.tangent));
    vec3 B = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.BiTangant));
    vec3 N = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vertex.normal);
    mat3 TBN = mat3(T, B, N);

    return TBN;
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

vec3 Irradiate(vec3 N)
{
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));

    vec3 irradiance = vec3(0.0f);
    if(rayHitInfo.reflectCount != ConstMesh.MaxRefeflectCount)
    {
        if(rayHitInfo.domePhi < 2.0 * PI)
        {
            if(rayHitInfo.domeTheta < 0.5 * PI)
            {
                vec3 tangentSample = vec3(sin(rayHitInfo.domeTheta) * cos(rayHitInfo.domePhi),  sin(rayHitInfo.domeTheta) * sin(rayHitInfo.domePhi), cos(rayHitInfo.domeTheta));
                vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

                uint seed = tea(gl_LaunchIDEXT.y * gl_LaunchSizeEXT.x + gl_LaunchIDEXT.x, ConstMesh.frame);
                float r1        = rnd(seed);
                float r2        = rnd(seed);
                float sq        = sqrt(1.0 - r2);
                float phi       = 2 * PI * r1;

                vec3 hitPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_RayTmaxEXT;
                vec3 origin   = hitPos.xyz + N * 0.001f;
                vec3 rayDir   = reflect(gl_WorldRayDirectionEXT * (vec3(cos(phi) * sq, sin(phi) * sq, sqrt(r2))), sampleVec);
        
               rayHitInfo.reflectCount++;
               traceRayEXT(topLevelAS, gl_RayFlagsNoneEXT, 0xff, 0, 0, 0, origin, 0.001f, rayDir, 10000.0f, 0);
               irradiance += rayHitInfo.color;
               rayHitInfo.domeTheta += rayHitInfo.domeSampleDelta;
               rayHitInfo.domeSampleCount++;
            }
        }
        rayHitInfo.domePhi +=  rayHitInfo.domeSampleDelta;
    }
     irradiance = PI * irradiance * (1.0 / float(rayHitInfo.domeSampleCount + rayHitInfo.reflectCount));
    return irradiance;
}

vec3 PrefilterColor(vec3 N)
{
    vec3 V = normalize(ConstMesh.CameraPos - vertex.pos);

    vec3 prefilteredColor  = vec3(0.0f);
    if(rayHitInfo.reflectCount != ConstMesh.MaxRefeflectCount)
    {
        vec3 hitPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_RayTmaxEXT;
        vec3 origin   = hitPos.xyz + vertex.normal * 0.001f;
        vec3 rayDir   = reflect(gl_WorldRayDirectionEXT, N);
        
        rayHitInfo.reflectCount++;
        traceRayEXT(topLevelAS, gl_RayFlagsNoneEXT, 0xff, 0, 0, 0, origin, 0.001f, rayDir, 10000.0f, 0);
        prefilteredColor = mix(prefilteredColor, rayHitInfo.color, .5f); 
    }

    return prefilteredColor;
}

vec3 CalcDirectionalLight(vec3 F0, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic)
{
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < scenedata.DirectionalLightCount; ++i) 
    {
        vec3 L = normalize(DLight[i].direction - vertex.pos);
        vec3 H = normalize(V + L);
        vec3 radiance = DLight[i].diffuse;

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
    return Lo;
}

vec3 CalcPointLight(vec3 F0, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic)
{
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < scenedata.PointLightCount; ++i) 
    {
        vec3 L = normalize(PLight[i].position - vertex.pos);
        vec3 H = normalize(V + L);
        float distance = length(PLight[i].position - vertex.pos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = PLight[i].diffuse * attenuation;

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
    return Lo;
}

vec3 CalcSpotLight(vec3 F0, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic)
{
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < scenedata.SpotLightCount; ++i) 
    {
        vec3 L = normalize(SLight[i].position - vertex.pos);
        vec3 H = normalize(V + L);

        float theta = dot(L, normalize(-SLight[i].direction)); 
        float epsilon = SLight[i].cutOff - SLight[i].outerCutOff;
        float intensity = clamp((theta - SLight[i].outerCutOff) / epsilon, 0.0, 1.0);

        float distance = length(SLight[i].position - vertex.pos);
        float attenuation = 1.0 / (distance * distance) ;
        attenuation *= intensity;
        vec3 radiance = PLight[i].diffuse * attenuation;

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
    return Lo;
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
