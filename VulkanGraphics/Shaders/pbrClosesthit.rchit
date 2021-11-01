#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable

#include "SceneProperties.glsl"
#include "MeshProperties.glsl"
#include "Vertex.glsl"
#include "Lighting.glsl"

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
layout(binding = 2) uniform SceneDataBuffer { SceneProperties sceneData; } sceneBuffer;
layout(binding = 3) buffer MeshPropertiesBuffer { MeshProperties meshProperties; } meshBuffer[];
layout(binding = 4) buffer DirectionalLightBuffer{ DirectionalLight directionalLight; } DLight[];
layout(binding = 5) buffer PointLightBuffer { PointLight pointLight; } PLight[];
layout(binding = 6) buffer SpotLightBuffer { SpotLight spotLight; } SLight[];
layout(binding = 7, scalar) buffer Vertices { Vertex v[]; } vertices[];
layout(binding = 8) buffer Indices { uint i[]; } indices[];
layout(binding = 11) uniform sampler2D TextureMap[];
layout(binding = 12) uniform sampler3D Texture3DMap[];
layout(binding = 13, set = 0) uniform samplerCube CubeMap[];
layout(binding = 14) buffer SphereAreaLightBuffer { SphereAreaLight sphereLight; } sphereLightBuffer[];
layout(binding = 15) buffer TubeAreaLightBuffer { TubeAreaLight tubeAreaLight; } tubeLightBuffer[];
layout(binding = 16) buffer RectangleAreaLightBuffer { RectangleAreaLight rectangleAreaLight; } rectangleAreaLightBuffer[];

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
vec3 PrefilterColor();
vec3 CalcDirectionalLight(vec3 F0, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic);
vec3 CalcPointLight(vec3 F0, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic);
vec3 CalcSpotLight(vec3 F0, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic);
vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir);

float VanDerCorput(uint n, uint base)
{
    float invBase = 1.0 / float(base);
    float denom   = 1.0;
    float result  = 0.0;

    for(uint i = 0u; i < 32u; ++i)
    {
        if(n > 0u)
        {
            denom   = mod(float(n), 2.0);
            result += denom * invBase;
            invBase = invBase / 2.0;
            n       = uint(float(n) / 2.0);
        }
    }

    return result;
}

vec2 HammersleyNoBitOps(uint i, uint N)
{
    return vec2(float(i)/float(N), VanDerCorput(i, 2u));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
	float a = roughness*roughness;
	
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
	// from spherical coordinates to cartesian coordinates - halfway vector
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	
	// from tangent-space H vector to world-space sample vector
	vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent   = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);
	
	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
}

void main()
{		
   vertex = BuildVertexInfo();
   MaterialInfo material = meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.material;
   vec2 UV = vertex.uv;

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
    
    vec3 prefilteredColor = PrefilterColor();
    vec3 specular = prefilteredColor;

    vec3 ambient = (kD * diffuse + specular) * ao;
    
    vec3 color = ambient + Lo;

    rayHitInfo.color = color;
	//rayHitInfo.distance = gl_RayTmaxNV;
	rayHitInfo.normal = vertex.normal;
}

mat3 getTBNFromMap(Vertex vertex)
{
   const vec3 T = normalize(mat3(meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.ModelTransform * meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.MeshTransform) * vec3(vertex.tangent));
   const vec3 B = normalize(mat3(meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.ModelTransform * meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.MeshTransform) * vec3(vertex.BiTangant));
   const vec3 N = normalize(mat3(meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.ModelTransform * meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.MeshTransform) * vertex.normal);
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

vec3 PrefilterColor()
{
    vec3 N = normalize(vertex.pos);
    vec3 V = normalize(ConstMesh.CameraPos - vertex.pos);

    vec3 prefilteredColor = vec3(0.0);
    if(rayHitInfo.reflectCount != ConstMesh.MaxRefeflectCount)
    {
            vec3 hitPos = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_RayTmaxEXT;
            vec3 origin   = hitPos.xyz + N * 0.001f;
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
    for(int i = 0; i < sceneBuffer.sceneData.DirectionalLightCount; ++i) 
    {
        vec3 L = normalize(DLight[i].directionalLight.direction - vertex.pos);
        vec3 H = normalize(V + L);
        vec3 radiance = DLight[i].directionalLight.diffuse;

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
    for(int i = 0; i < sceneBuffer.sceneData.PointLightCount; ++i) 
    {
        vec3 L = normalize(PLight[i].pointLight.position - vertex.pos);
        vec3 H = normalize(V + L);
        float distance = length(PLight[i].pointLight.position - vertex.pos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = PLight[i].pointLight.diffuse * attenuation;

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
    for(int i = 0; i < sceneBuffer.sceneData.SpotLightCount; ++i) 
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
    const float heightScale = meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.heightScale;
    const float minLayers = meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.minLayers;
    const float maxLayers = meshBuffer[gl_InstanceCustomIndexEXT].meshProperties.maxLayers;
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
