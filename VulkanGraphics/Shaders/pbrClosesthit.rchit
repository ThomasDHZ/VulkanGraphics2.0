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
vec3 getNormalFromMap(MaterialInfo material, Vertex vertex)
{
    vec3 T = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.tangent));
    vec3 B = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vec3(vertex.BiTangant));
    vec3 N = normalize(mat3(meshProperties[gl_InstanceCustomIndexEXT].ModelTransform * MeshTransform[gl_InstanceCustomIndexEXT].Transform) * vertex.normal);
    mat3 TBN = transpose(mat3(T, B, N));

    vec3 normal = texture(TextureMap[material.NormalMapID], vertex.uv).xyz;
         normal = normalize(normal * 2.0 - 1.0);
    
    return TBN * normal;
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

vec3 Irradiate()
{
    vec3 irradiance = vec3(0.0f);
    if(rayHitInfo.reflectCount != ConstMesh.MaxRefeflectCount)
    {
        uint seed = tea(gl_LaunchIDEXT.y * gl_LaunchSizeEXT.x + gl_LaunchIDEXT.x, ConstMesh.frame);
        float r1        = rnd(seed);
        float r2        = rnd(seed);
        float sq        = sqrt(1.0 - r2);
        float phi       = 2 * PI * r1;

        vec3 hitPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_RayTmaxEXT;
        vec3 origin   = hitPos.xyz + vertex.normal * 0.001f;
        vec3 rayDir   = reflect(gl_WorldRayDirectionEXT * (vec3(cos(phi) * sq, sin(phi) * sq, sqrt(r2))), vertex.normal);
        
        rayHitInfo.reflectCount++;
        traceRayEXT(topLevelAS, gl_RayFlagsNoneEXT, 0xff, 0, 0, 0, origin, 0.001f, rayDir, 10000.0f, 0);
        irradiance = mix(irradiance, rayHitInfo.color, .5f); 
    }

    return irradiance;
}

//vec3 PrefilterColor(Vertex vertex)
//{
//    vec3 N = getNormalFromMap(material, vertex.normal);
//    vec3 V = normalize(ConstMesh.CameraPos - vertex.pos);
//
//    vec3 prefilteredColor  = vec3(0.0f);
//    if(rayHitInfo.reflectCount != ConstMesh.MaxRefeflectCount)
//    {
//        vec3 hitPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_RayTmaxEXT;
//        vec3 origin   = hitPos.xyz + vertex.normal * 0.001f;
//
//        uint seed = tea(gl_LaunchIDEXT.y * gl_LaunchSizeEXT.x + gl_LaunchIDEXT.x, ConstMesh.frame);
//        float r1        = rnd(seed);
//        float r2        = rnd(seed);
//        float sq        = sqrt(1.0 - r2);
//        float phi       = 2 * PI * r1;
//
//        vec3 L = normalize(gl_WorldRayDirectionEXT);
//        vec3 R = reflect(L), vertex.normal) * NdotL;
//        vec3 H = normalize(V + L);
//        
//        float NdotL = max(dot(N, L), 0.0);
//        vec3 rayDir   = reflect(gl_WorldRayDirectionEXT, vertex.normal) * NdotL;
//        
//        rayHitInfo.reflectCount++;
//        traceRayEXT(topLevelAS, gl_RayFlagsNoneEXT, 0xff, 0, 0, 0, origin, 0.001f, rayDir, 10000.0f, 0);
//        prefilteredColor = mix(prefilteredColor, rayHitInfo.color, .5f); 
//    }
//
//    return prefilteredColor;
//}
//

void main() 
{
    vertex = BuildVertexInfo();
    MaterialInfo material = MaterialList[meshProperties[gl_InstanceCustomIndexEXT].MaterialIndex].material;

    vec3 albedo     = texture(TextureMap[material.AlbedoMapID], vertex.uv).rgb;
    float metallic  = texture(TextureMap[material.MatallicMapID], vertex.uv).r;
    float roughness = texture(TextureMap[material.RoughnessMapID], vertex.uv).r;
    float ao        = texture(TextureMap[material.AOMapID], vertex.uv).r;

    vec3 N = getNormalFromMap(material, vertex);
    vec3 V = normalize(ConstMesh.CameraPos - vertex.pos);
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
    
    vec3 irradiance = Irradiate(vertex.normal);
    vec3 diffuse      = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * ao;
    // vec3 ambient = vec3(0.002);

    vec3 color = ambient + Lo;
    rayHitInfo.color = color;
//	//rayHitInfo.distance = gl_RayTmaxNV;
	rayHitInfo.normal = vertex.normal;
}