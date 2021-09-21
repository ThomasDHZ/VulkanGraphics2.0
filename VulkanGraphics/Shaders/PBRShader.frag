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
layout(binding = 9) uniform samplerCube CubeMap;

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

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec3 BiTangent;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBloom;


mat3 TBN;

const float PI = 3.14159265359;
vec3 getNormalFromMap(MaterialInfo material, vec2 uv);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3 CalcDirLight(PBRMaterial material, int index);
vec3 CalcPointLight(PBRMaterial material, int index);
vec3 CalcSpotLight(PBRMaterial material, int index);
vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir);

PBRMaterial PBRMaterialProperties()
{
   PBRMaterial material;
   const MaterialInfo materialInfo = MaterialList[meshProperties[Mesh.MeshIndex].MaterialIndex].material;

   vec2 texCoords = TexCoords + meshProperties[Mesh.MeshIndex].UVOffset;
   if(texture(TextureMap[materialInfo.AlphaMapID], texCoords).r == 0.0f ||
      texture(TextureMap[materialInfo.DiffuseMapID], texCoords).a == 0.0f)
   {
	 discard;
   }
   material.UV *= meshProperties[Mesh.MeshIndex].UVScale;
   if(meshProperties[Mesh.MeshIndex].UVFlip.y == 1.0f)
   {
        material.UV.y = 1.0f - material.UV.y;
   }
   if(meshProperties[Mesh.MeshIndex].UVFlip.x == 1.0f)
   {
        material.UV.x = 1.0f - material.UV.x;
   }
   if(meshProperties[Mesh.MeshIndex].UVFlip.y == 1.0f)
   {
        material.UV.y = 1.0f - material.UV.y;
   }

    material.Albedo = material.Albedo;
    if (materialInfo.AlbedoMapID != 0)
    {
        material.Albedo = texture(TextureMap[materialInfo.AlbedoMapID], texCoords).rgb;
    }

    material.Metallic = materialInfo.Matallic;
    if (materialInfo.MatallicMapID != 0)
    {
        material.Metallic = texture(TextureMap[materialInfo.MatallicMapID], texCoords).r;
    }

    material.Roughness = material.Roughness;
    if (materialInfo.RoughnessMapID != 0)
    {
        material.Roughness = texture(TextureMap[materialInfo.RoughnessMapID], texCoords).r;
    }

    material.AmbientOcclusion = material.AmbientOcclusion;
    if (materialInfo.AOMapID != 0)
    {
        material.AmbientOcclusion = texture(TextureMap[materialInfo.AOMapID], texCoords).r;
    }

    material.Alpha = material.Alpha;
    if (materialInfo.AlphaMapID != 0)
    {
       material.Alpha = texture(TextureMap[materialInfo.AlphaMapID], texCoords).r;
    }

    if(materialInfo.NormalMapID != 0)
    {
       material.Normal = getNormalFromMap(materialInfo, TexCoords);
    }

    return material;
}

void main() 
{
   PBRMaterial material = PBRMaterialProperties();

   vec3 Lo = vec3(0.0);
   for(int x = 0; x < scenedata.DirectionalLightCount; x++)
   {
        Lo += CalcDirLight(material, x);
   }
   for(int x = 0; x < scenedata.PointLightCount; x++)
   {
        //Lo += CalcPointLight(material, x);   
   }
   for(int x = 0; x < scenedata.SpotLightCount; x++)
   {
        //Lo += CalcSpotLight(material, x);   
   }
   for(int x = 0; x < scenedata.SphereAreaLightCount; x++)
   {
      //  Lo += CalcSphereAreaLight(material, x);
   }
   for(int x = 0; x < scenedata.TubeAreaLightCount; x++)
   {
       // result += CalcTubeAreaLight(normal, texCoords, x);   
   }

    vec3 ambient = vec3(0.03) * material. Albedo *  material.AmbientOcclusion;
    vec3 color = ambient + Lo;

    outColor = vec4(color, material.Alpha);
    outBloom = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

vec3 getNormalFromMap(MaterialInfo material, vec2 uv)
{
    vec3 tangentNormal = texture(TextureMap[material.NormalMapID], uv).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
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

vec3 CalcDirLight(PBRMaterial material, int index)
{
    vec3 Lo = vec3(0.0f, 0.0f, 0.0f);
    vec3 LightPos = DLight[index].direction;

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, material.Albedo, material.Metallic);
  
    vec3 V = normalize(Mesh.CameraPos - FragPos);
    vec3 R = reflect(-V, material.Normal); 
    vec3 L = normalize(-LightPos);
    vec3 H = normalize(V + L);

    vec3 radiance = DLight[index].diffuse;

    float NDF = DistributionGGX(material.Normal, H, material.Roughness);   
    float G   = GeometrySmith(material.Normal, V, L, material.Roughness);    
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
    vec3 nominator    = NDF * G * F;
    float denominator = 4 * max(dot(material.Normal, V), 0.0) * max(dot(material.Normal, L), 0.0) + 0.001;
    vec3 specular = nominator / denominator;
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - material.Metallic;	                
            
    float NdotL = max(dot(material.Normal, L), 0.0);        
    Lo += (kD *  material.Albedo / PI + specular) * radiance * NdotL;

    return Lo;
}

vec3 CalcPointLight(PBRMaterial material, int index)
{
    vec3 Lo = vec3(0.0f, 0.0f, 0.0f);
    vec3 LightPos = PLight[index].position;

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, material.Albedo, material.Metallic);
  
    vec3 V = normalize(Mesh.CameraPos - FragPos);
    vec3 R = reflect(-V, material.Normal); 
    vec3 L = normalize(LightPos - FragPos);
    vec3 H = normalize(V + L);

    vec3 radiance = DLight[index].diffuse;

    float NDF = DistributionGGX(material.Normal, H, material.Roughness);   
    float G   = GeometrySmith(material.Normal, V, L, material.Roughness);    
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
    vec3 nominator    = NDF * G * F;
    float denominator = 4 * max(dot(material.Normal, V), 0.0) * max(dot(material.Normal, L), 0.0) + 0.001;
    vec3 specular = nominator / denominator;
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - material.Metallic;	                
            
    float NdotL = max(dot(material.Normal, L), 0.0);        
    Lo += (kD *  material.Albedo / PI + specular) * radiance * NdotL;

    float LightDistance = length(LightPos - FragPos);
    float attenuation = 1.0 / (1.0f + PLight[index].linear * LightDistance + PLight[index].quadratic * (LightDistance * LightDistance));

    return Lo * attenuation;
}

vec3 CalcSpotLight(PBRMaterial material, int index)
{
    vec3 Lo = vec3(0.0f, 0.0f, 0.0f);
    vec3 LightPos = SLight[index].position;

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, material.Albedo, material.Metallic);
  
    vec3 V = normalize(Mesh.CameraPos - FragPos);
    vec3 R = reflect(-V, material.Normal); 
    vec3 L = normalize(LightPos - FragPos);
    vec3 H = normalize(V + L);

    vec3 radiance = SLight[index].diffuse;

    float NDF = DistributionGGX(material.Normal, H, material.Roughness);   
    float G   = GeometrySmith(material.Normal, V, L, material.Roughness);    
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
    vec3 nominator    = NDF * G * F;
    float denominator = 4 * max(dot(material.Normal, V), 0.0) * max(dot(material.Normal, L), 0.0) + 0.001;
    vec3 specular = nominator / denominator;
        
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - material.Metallic;	                
            
    float NdotL = max(dot(material.Normal, L), 0.0);        
    Lo += (kD *  material.Albedo / PI + specular) * radiance * NdotL;

    float LightDistance = length(LightPos - FragPos);
    float attenuation = 1.0 / (1.0f + PLight[index].linear * LightDistance + PLight[index].quadratic * (LightDistance * LightDistance));

    float theta = dot(L, normalize(-SLight[index].direction)); 
    float epsilon = SLight[index].cutOff - SLight[index].outerCutOff;
    float intensity = clamp((theta - SLight[index].outerCutOff) / epsilon, 0.0, 1.0);

    return Lo * attenuation;
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