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
	vec3 RayTracedTexture;
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
Vertex BuildVertexInfo();
float DistributionGGX(vec3 N, vec3 H, float roughness);
float SchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3  FresnelSchlick(float cosTheta, vec3 F0);
vec3 CalcNormalDirLight(Vertex vertex, MaterialInfo material, mat3 TBN, vec3 normal, vec2 uv);
vec3 CalcNormalPointLight(Vertex vertex, MaterialInfo material, mat3 TBN, PointLight light, vec3 normal, vec2 uv);
vec3 CalcNormalSpotLight(Vertex vertex, MaterialInfo material, mat3 TBN, SpotLight light, vec3 normal, vec2 uv);
vec3 CalcDirLight(Vertex vertex, MaterialInfo material, DirectionalLight light, vec2 uv);
vec3 CalcPointLight(Vertex vertex, MaterialInfo material, PointLight light, vec2 uv);
vec3 CalcSpotLight(Vertex vertex, MaterialInfo material, SpotLight light, vec2 uv);
vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir);
//vec3 getNormalFromMap(Vertex vertex, MaterialInfo material)
//{
////    vec3 tangentNormal = texture(TextureMap[material.NormalMapID], vertex.uv).rgb * 2.0f - 1.0f;
////
////    vec3 Q1  = dFdx(vertex.pos);
////    vec3 Q2  = dFdy(vertex.pos);
////    vec2 st1 = dFdx(vertex.uv);
////    vec2 st2 = dFdy(vertex.uv);
////
////    vec3 N   = normalize(vertex.normal);
////    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
////    vec3 B  = -normalize(cross(N, T));
////    mat3 TBN = mat3(T, B, N);
//
//    return normalize(TBN * tangentNormal);
//}

void main()
{
   const Vertex vertex = BuildVertexInfo();
   const MaterialInfo material = MaterialList[meshProperties[gl_InstanceCustomIndexEXT].MaterialIndex].material;

	vec3 lightVector = normalize(ubo.dlight.direction);
	float dot_product = max(dot(lightVector, vertex.normal), 0.6);

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
        diffuse = ubo.dlight.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], vertex.uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = ubo.dlight.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], vertex.uv));
    }

    vec3 result = vec3(ambient + diffuse);
   // result = RTXShadow(result, specular, lightDir, 10000.0f);
	rayHitInfo.RayTracedTexture = result;
    //rayPayload.reflectCount = 0.2543333f;
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

    vec3 result = vec3(ambient + diffuse);
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

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
   float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001);
}

float SchlickGGX(float NdotV, float roughness)
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
    float ggx2 = SchlickGGX(NdotV, roughness);
    float ggx1 = SchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}