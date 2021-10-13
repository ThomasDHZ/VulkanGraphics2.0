#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "material.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshIndex;
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} Mesh;

layout(binding = 0) uniform UniformBufferObject {
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

layout(binding = 1) buffer MeshProperties 
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

layout(binding = 2) buffer DirectionalLight
{ 
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float Luminosity;
} DLight[];

layout(binding = 3) buffer PointLight
{ 
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float Luminosity;
    mat4 lightSpaceMatrix;
} PLight[];

layout(binding = 4) buffer SpotLight
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

layout(binding = 5) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 6) buffer Material { MaterialInfo material; } MaterialList[];
layout(binding = 7) uniform sampler2D TextureMap[];
layout(binding = 8) uniform sampler3D Texture3DMap[];
layout(binding = 9) uniform samplerCube CubeMap[];

layout(binding = 10) buffer SphereAreaLightBuffer {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float SphereRadius;
    float linear;
    float quadratic;
	float Luminosity;
} SphereLight[];

layout(binding = 11) buffer TubeAreaLightBuffer {
	vec3 StartPos;
	vec3 EndPos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
    float TubeRadius;
	float Luminosity;
} TubeLight[];

layout(binding = 12) buffer RectangleAreaLightBuffer
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

layout(binding = 13) uniform samplerCube IrradianceMap;
layout(binding = 14) uniform samplerCube PrefilterMap;
layout(binding = 15) uniform sampler2D BRDFMap;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec3 BiTangent;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBloom;

const float PI = 3.14159265359;

mat3 getTBNFromMap();
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3 CalcDirectionalLight(vec3 F0, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic);
vec3 CalcPointLight(vec3 F0, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic);
vec3 CalcSpotLight(vec3 F0, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic);
vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir);

void main()
{		
   MaterialInfo material = MaterialList[meshProperties[Mesh.MeshIndex].MaterialIndex].material;
   vec2 UV = TexCoords + meshProperties[Mesh.MeshIndex].UVOffset;
   if(texture(TextureMap[material.AlphaMapID], UV).r == 0.0f ||
      texture(TextureMap[material.DiffuseMapID], UV).a == 0.0f)
   {
	 discard;
   }

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

    vec3 V = normalize(Mesh.CameraPos - FragPos);
    mat3 TBN = getTBNFromMap();
    if(material.DepthMapID != 0)
    {
            UV = ParallaxMapping(material, UV,  V);       
            if(UV.x > 1.0 || UV.y > 1.0 || UV.x < 0.0 || UV.y < 0.0)
            {
              discard;
            }
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
    
    vec3 irradiance = texture(IrradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(PrefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(BRDFMap, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;
    
    vec3 color = ambient + Lo;
    outColor = vec4(color , alpha);
}

mat3 getTBNFromMap()
{
    vec3 T = normalize(mat3(meshProperties[Mesh.MeshIndex].ModelTransform * MeshTransform[Mesh.MeshIndex].Transform) * vec3(Tangent));
    vec3 B = normalize(mat3(meshProperties[Mesh.MeshIndex].ModelTransform * MeshTransform[Mesh.MeshIndex].Transform) * vec3(BiTangent));
    vec3 N = normalize(mat3(meshProperties[Mesh.MeshIndex].ModelTransform * MeshTransform[Mesh.MeshIndex].Transform) * Normal);
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

vec3 CalcDirectionalLight(vec3 F0, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic)
{
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < scenedata.DirectionalLightCount; ++i) 
    {
        vec3 L = normalize(DLight[i].direction - FragPos);
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
        vec3 L = normalize(PLight[i].position - FragPos);
        vec3 H = normalize(V + L);
        float distance = length(PLight[i].position - FragPos);
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
        vec3 L = normalize(SLight[i].position - FragPos);
        vec3 H = normalize(V + L);

        float theta = dot(L, normalize(-SLight[i].direction)); 
        float epsilon = SLight[i].cutOff - SLight[i].outerCutOff;
        float intensity = clamp((theta - SLight[i].outerCutOff) / epsilon, 0.0, 1.0);

        float distance = length(SLight[i].position - FragPos);
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
    const float heightScale = meshProperties[Mesh.MeshIndex].heightScale;
    const float minLayers = meshProperties[Mesh.MeshIndex].minLayers;
    const float maxLayers = meshProperties[Mesh.MeshIndex].maxLayers;
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
