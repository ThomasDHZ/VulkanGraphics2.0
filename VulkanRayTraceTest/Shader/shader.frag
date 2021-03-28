#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"
#include "material.glsl"
#include "vertex.glsl"

layout(push_constant) uniform ConstMeshProperties
{
	uint MeshIndex;
} ConstMesh;

struct VertexData
{
	vec3 Position;
	vec3 Normal;
	vec2 UV;
	vec3 Tangent;
	vec3 BiTangent;
};

layout(binding = 2) uniform UniformBufferObject {
	DirectionalLight dlight;
	PointLight plight[5];
	SpotLight sLight;
    mat4 viewInverse;
	mat4 projInverse;
	mat4 view;
	mat4 proj;
    vec3 viewPos;
	float timer;
    int temp;
} scenedata;

layout(binding = 3) buffer MeshProperties 
{
	mat4 ModelTransform;
	mat4 BoneTransform[100];
	vec2 UVOffset;
    uint MaterialIndex;
} meshProperties[];
layout(binding = 4) buffer Vertices { Vertex v[]; } vertices[];
layout(binding = 5) buffer Indices { uint i[]; } indices[];
layout(binding = 6) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 7) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 8) uniform sampler2D TextureMap[];
layout(binding = 9) uniform sampler3D Texture3DMap[];

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec4 Color;
layout(location = 3) in vec3 Normal;
layout(location = 4) in mat3 TBN;

layout(location = 0) out vec4 outColor;

vec3 CalcDirLight(MaterialInfo material, vec3 normal);
vec3 CalcPointLight(MaterialInfo material, PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(MaterialInfo material, SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main() 
{
    const MaterialInfo material = MaterialList[meshProperties[ConstMesh.MeshIndex].MaterialIndex].material;
    const vec2 texCoords = TexCoords + meshProperties[ConstMesh.MeshIndex].UVOffset;

    vec3 normal = Normal;
    if(material.AlphaMapID != 0)
    {
        normal = texture(TextureMap[material.NormalMapID], texCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
    }
   
    vec3 result = CalcDirLight(material, normal);
//    for(int x = 0; x < 5; x++)
//    {
//        result += CalcPointLight(material, scenedata.plight[x], Normal, FragPos, viewDir);   
//    }
   // result +=  CalcSpotLight( material, scenedata.sLight, Normal, Normal, viewDir);

    outColor = vec4(result, 1.0);
}

vec3 CalcDirLight(MaterialInfo material, vec3 normal)
{
    const vec3 TangentLightPos = TBN * scenedata.dlight.direction;
    const vec3 TangentViewPos  = TBN * scenedata.viewPos;
    const vec3 TangentFragPos  = TBN * FragPos;  
    const vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    vec3 lightDir = normalize(-TangentLightPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    vec3 ambient = scenedata.dlight.ambient * Color.rgb;
    vec3 diffuse = scenedata.dlight.diffuse * diff * Color.rgb;
    vec3 specular = scenedata.dlight.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = scenedata.dlight.ambient * vec3(texture(TextureMap[material.DiffuseMapID], TexCoords));
        diffuse = scenedata.dlight.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], TexCoords));
    }
    if(material.SpecularMapID != 0)
    {
        specular = scenedata.dlight.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], TexCoords));
    }
    
    return vec3(ambient + diffuse + specular);
}

vec3 CalcPointLight(MaterialInfo material, PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    const vec3 TangentLightPos = TBN * light.position;
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 ambient = light.ambient * Color.rgb;
    vec3 diffuse = light.diffuse * diff * Color.rgb;
    vec3 specular = light.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = light.ambient * vec3(texture(TextureMap[material.DiffuseMapID], TexCoords));
        diffuse = light.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], TexCoords));
    }
    if(material.SpecularMapID != 0)
    {
        specular = light.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], TexCoords));
    }
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(MaterialInfo material, SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    const vec3 TangentLightPos = TBN * light.position;
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * Color.rgb;
    vec3 diffuse = light.diffuse * diff * Color.rgb;
    vec3 specular = light.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = light.ambient * vec3(texture(TextureMap[material.DiffuseMapID], TexCoords));
        diffuse = light.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], TexCoords));
    }
    if(material.SpecularMapID != 0)
    {
        specular = light.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], TexCoords));
    }
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    float heightScale = 0.1;
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(TextureMap[3], currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(TextureMap[3], currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(TextureMap[3], prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}