#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_debug_printf : enable
#include "Vertex.glsl"
#include "Lighting.glsl"
#include "Material.glsl"

const float PI = 3.14159265359;

layout(push_constant) uniform RayTraceCamera
{
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} Camera;

struct RayPayload {
	vec3 ShadowMask;
	vec3 ReflectionColor;
	vec3 SSAOColor;
	vec3 SkyboxColor;
	uint reflectCount;
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
} ubo;
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


vec3 RTXShadow(vec3 LightResult, vec3 LightDirection, float LightDistance);
#include "RTVertexBuilder.glsl"
vec3 CalcNormalDirLight(Vertex vertex, MaterialInfo material, mat3 TBN, vec3 normal, vec2 uv);
vec3 CalcNormalPointLight(Vertex vertex, MaterialInfo material, mat3 TBN, PointLight light, vec3 normal, vec2 uv);
vec3 CalcNormalSpotLight(Vertex vertex, MaterialInfo material, mat3 TBN, SpotLight light, vec3 normal, vec2 uv);
vec3 CalcDirLight(Vertex vertex, MaterialInfo material, DirectionalLight light, vec2 uv);
vec3 CalcPointLight(Vertex vertex, MaterialInfo material, PointLight light, vec2 uv);
vec3 CalcSpotLight(Vertex vertex, MaterialInfo material, SpotLight light, vec2 uv);
vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir);

void main()
{
   const Vertex vertex = BuildVertexInfo();
   const MaterialInfo material = MaterialList[meshProperties[gl_InstanceCustomIndexEXT].MaterialIndex].material;

   vec2 uv = vertex.uv;
   rayHitInfo.ShadowMask = vec3(0.0f, 0.0f, 0.0f);

	float tmin = 0.001;
	float tmax = 10000.0;
	vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;

	shadowed = true;  
	traceRayEXT(topLevelAS, gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, DLight[0].direction, tmax, 1);
    if (shadowed) 
    {
		rayHitInfo.ShadowMask = vec3(1.0f, 0.0f, 0.0f);
	}

    shadowed = true;  
    traceRayEXT(topLevelAS, gl_RayFlagsSkipClosestHitShaderEXT, 0xFF, 1, 0, 1, origin, tmin, DLight[1].direction, tmax, 1);
    if (shadowed) 
    {
		rayHitInfo.ShadowMask = vec3(1.0f, 0.0f, 0.0f);
	}
}

vec3 RTXShadow(vec3 LightResult, vec3 LightSpecular, vec3 LightDirection, float LightDistance)
{
     if(ubo.Shadowed == 1)
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

vec3 CalcNormalDirLight(Vertex vertex, MaterialInfo material, mat3 TBN, vec3 normal, vec2 uv)
{
    const vec3 TangentLightPos = TBN * ubo.dlight.direction;
    const vec3 TangentViewPos  = TBN * ubo.viewPos;
    const vec3 TangentFragPos  = TBN * vertex.pos;  
    const vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    const vec3 lightDir = normalize(-TangentLightPos);
    const float diff = max(dot(normal, lightDir), 0.0);

    const vec3 halfwayDir = normalize(lightDir + viewDir);  
    const float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    vec3 ambient = ubo.dlight.ambient * material.Diffuse.rgb;
    vec3 diffuse = ubo.dlight.diffuse * diff * material.Diffuse.rgb;
    vec3 specular = ubo.dlight.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = ubo.dlight.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = ubo.dlight.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = ubo.dlight.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }
    
    vec3 result = vec3(ambient + diffuse);
    result = RTXShadow(result, specular, lightDir, 10000.0f);
    return result;
}

vec3 CalcNormalPointLight(Vertex vertex, MaterialInfo material, mat3 TBN, PointLight light, vec3 normal, vec2 uv)
{
    const vec3 TangentLightPos = TBN * light.position;
    const vec3 TangentViewPos  = TBN * ubo.viewPos;
    const vec3 TangentFragPos  = TBN * vertex.pos;  
 
    vec3 lightDir = normalize(-TangentLightPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    vec3 ambient = light.ambient * material.Diffuse.rgb;
    vec3 diffuse = light.diffuse * diff * material.Diffuse.rgb;
    vec3 specular = light.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = light.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = light.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = light.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }

    float distance = length(TangentLightPos - TangentFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = vec3(ambient + diffuse);
    result = RTXShadow(result, specular, lightDir, distance);
    return result;
}

vec3 CalcNormalSpotLight(Vertex vertex, MaterialInfo material, mat3 TBN, SpotLight light, vec3 normal, vec2 uv)
{
    const vec3 TangentLightPos = TBN * light.position;
    const vec3 TangentViewPos  = TBN * ubo.viewPos;
    const vec3 TangentFragPos  = TBN * vertex.pos;  
    const vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    vec3 lightDir = normalize(light.position - vertex.pos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    float distance = length(light.position - TangentFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * material.Diffuse.rgb;
    vec3 diffuse = light.diffuse * diff * material.Diffuse.rgb;
    vec3 specular = light.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = light.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = light.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = light.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    vec3 result = vec3(ambient + diffuse);
    result = RTXShadow(result, specular, lightDir, distance);
    return result;
}

vec3 CalcDirLight(Vertex vertex, MaterialInfo material, DirectionalLight light, vec2 uv)
{
    const vec3 viewDir = normalize(ubo.viewPos - vertex.pos);
    const vec3 lightDir = normalize(-ubo.dlight.direction);

    const float diff = max(dot(vertex.normal, lightDir), 0.0);

    const vec3 reflectDir = reflect(-lightDir, vertex.normal);
    const float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);

    vec3 ambient = ubo.dlight.ambient * material.Diffuse.rgb;
    vec3 diffuse = ubo.dlight.diffuse * diff * material.Diffuse.rgb;
    vec3 specular = ubo.dlight.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = ubo.dlight.ambient * vec3(texture(TextureMap[material.DiffuseMapID], vertex.uv));
        diffuse = ubo.dlight.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = ubo.dlight.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }

    vec3 result = vec3(ambient + diffuse + specular);
    result = RTXShadow(result, specular, lightDir, 10000.0f);
    return result;
}

vec3 CalcPointLight(Vertex vertex, MaterialInfo material, PointLight light, vec2 uv)
{
    vec3 viewDir = normalize(ubo.viewPos - vertex.pos);
    vec3 lightDir = normalize(light.position - vertex.pos);

    float diff = max(dot(vertex.normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, vertex.normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);

    float distance = length(light.position - vertex.pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 ambient = light.ambient * material.Diffuse.rgb;
    vec3 diffuse = light.diffuse * diff * material.Diffuse.rgb;
    vec3 specular = light.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = light.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = light.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = light.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = vec3(ambient + diffuse);
    result = RTXShadow(result, specular, lightDir, distance);
    return result;
}

vec3 CalcSpotLight(Vertex vertex, MaterialInfo material, SpotLight light, vec2 uv)
{
    vec3 viewDir = normalize(ubo.viewPos - vertex.pos);
    vec3 lightDir = normalize(light.position - vertex.pos);

    float diff = max(dot(vertex.normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, vertex.normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);

    float distance = length(light.position - vertex.pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * material.Diffuse.rgb;
    vec3 diffuse = light.diffuse * diff * material.Diffuse.rgb;
    vec3 specular = light.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = light.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = light.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = light.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    vec3 result = vec3(ambient + diffuse);
    result = RTXShadow(result, specular, lightDir, distance);
    return result;
}

vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir)
{ 
    const float heightScale = meshProperties[gl_InstanceCustomIndexEXT].heightScale;
    const float minLayers = meshProperties[gl_InstanceCustomIndexEXT].minLayers;
    const float maxLayers = meshProperties[gl_InstanceCustomIndexEXT].maxLayers;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    viewDir.y = -viewDir.y; 
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(TextureMap[material.DepthMapID], prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}