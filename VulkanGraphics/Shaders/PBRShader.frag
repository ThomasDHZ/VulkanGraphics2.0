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

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec3 BiTangent;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBloom;


//MaterialInfo material;
mat3 TBN;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
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
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 getNormalFromMap(MaterialInfo material, vec2 uv)
{
    vec3 T = normalize(mat3(meshProperties[Mesh.MeshIndex].ModelTransform * MeshTransform[Mesh.MeshIndex].Transform) * vec3(Tangent));
    vec3 B = normalize(mat3(meshProperties[Mesh.MeshIndex].ModelTransform * MeshTransform[Mesh.MeshIndex].Transform) * vec3(BiTangent));
    vec3 N = normalize(mat3(meshProperties[Mesh.MeshIndex].ModelTransform * MeshTransform[Mesh.MeshIndex].Transform) * Normal);
    mat3 TBN = transpose(mat3(T, B, N));

    vec3 normal = texture(TextureMap[material.NormalMapID], uv).xyz;
         normal = normalize(normal * 2.0 - 1.0);
    
    return TBN * normal;
}
void main()
{		
    MaterialInfo material = MaterialList[meshProperties[Mesh.MeshIndex].MaterialIndex].material;

    vec3 albedo     = texture(TextureMap[material.AlbedoMapID], TexCoords).rgb;
    float metallic  = texture(TextureMap[material.MatallicMapID], TexCoords).r;
    float roughness = texture(TextureMap[material.RoughnessMapID], TexCoords).r;
    float ao        = texture(TextureMap[material.AOMapID], TexCoords).r;

    vec3 N = getNormalFromMap(material, TexCoords);
    vec3 V = normalize(Mesh.CameraPos - FragPos);
    vec3 R = reflect(-V, N); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 1; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(-DLight[i].direction);
        vec3 H = normalize(V + L);
        vec3 radiance = DLight[i].diffuse;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (we now use IBL as the ambient term)
    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    vec3 irradiance = texture(IrradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * ao;
    // vec3 ambient = vec3(0.002);
    
    vec3 color = ambient + Lo;
    outColor = vec4(ambient, material.Alpha);
}
