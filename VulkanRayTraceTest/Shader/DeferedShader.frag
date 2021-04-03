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

vec3 CalcNormalDirLight(MaterialInfo material, vec3 normal, vec2 uv);
vec3 CalcNormalPointLight(MaterialInfo material, PointLight light, vec3 normal, vec2 uv);
vec3 CalcNormalSpotLight(MaterialInfo material, SpotLight light, vec3 normal, vec2 uv);
vec3 CalcDirLight(MaterialInfo material, vec2 uv);
vec3 CalcPointLight(MaterialInfo material, PointLight light, vec2 uv);
vec3 CalcSpotLight(MaterialInfo material, SpotLight light, vec2 uv);
vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir);

void main() 
{
    const MaterialInfo material = MaterialList[meshProperties[ConstMesh.MeshIndex].MaterialIndex].material;
    vec2 texCoords = TexCoords + meshProperties[ConstMesh.MeshIndex].UVOffset;

   if(texture(TextureMap[material.AlphaMapID], texCoords).r == 0.0f)
   {
	 discard;
   }
    
    const vec3 TangentViewPos  = TBN * scenedata.viewPos;
    const vec3 TangentFragPos  = TBN * FragPos;  
    const vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    vec3 result = vec3(0.0f);
    vec3 normal = Normal;
    if(material.NormalMapID != 0)
    {
        if(material.DepthMapID != 0)
        {
            texCoords = ParallaxMapping(material, texCoords,  viewDir);       
            if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
            {
              discard;
            }
        }
        normal = texture(TextureMap[material.NormalMapID], texCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);

       result = CalcNormalDirLight(material, normal, texCoords);
        for(int x = 0; x < 5; x++)
        {
          result += CalcNormalPointLight(material, scenedata.plight[x], normal, texCoords);   
        }
        result +=  CalcNormalSpotLight( material, scenedata.sLight, normal, texCoords);
    }
    else
    {
        result = CalcDirLight(material, texCoords);
        for(int x = 0; x < 5; x++)
        {
           result += CalcPointLight(material, scenedata.plight[x], texCoords);   
        }
        result +=  CalcSpotLight( material, scenedata.sLight, texCoords);
    }

    outColor = vec4(result, 1.0);
}

vec3 CalcNormalDirLight(MaterialInfo material, vec3 normal, vec2 uv)
{
    const vec3 TangentLightPos = TBN * scenedata.dlight.direction;
    const vec3 TangentViewPos  = TBN * scenedata.viewPos;
    const vec3 TangentFragPos  = TBN * FragPos;  
    const vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    const vec3 lightDir = normalize(-TangentLightPos);
    const float diff = max(dot(normal, lightDir), 0.0);

    const vec3 halfwayDir = normalize(lightDir + viewDir);  
    const float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    vec3 ambient = scenedata.dlight.ambient * Color.rgb;
    vec3 diffuse = scenedata.dlight.diffuse * diff * Color.rgb;
    vec3 specular = scenedata.dlight.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = scenedata.dlight.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = scenedata.dlight.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = scenedata.dlight.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }
    
    return vec3(ambient + diffuse + specular);
}

vec3 CalcNormalPointLight(MaterialInfo material, PointLight light, vec3 normal, vec2 uv)
{
    const vec3 TangentLightPos = TBN * light.position;
    const vec3 TangentViewPos  = TBN * scenedata.viewPos;
    const vec3 TangentFragPos  = TBN * FragPos;  
 
    vec3 lightDir = normalize(-TangentLightPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    vec3 ambient = light.ambient * Color.rgb;
    vec3 diffuse = light.diffuse * diff * Color.rgb;
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

    return (ambient + diffuse + specular);
}

vec3 CalcNormalSpotLight(MaterialInfo material, SpotLight light, vec3 normal, vec2 uv)
{
    const vec3 TangentLightPos = TBN * light.position;
    const vec3 TangentViewPos  = TBN * scenedata.viewPos;
    const vec3 TangentFragPos  = TBN * FragPos;  
    const vec3 viewDir = normalize(TangentViewPos - TangentFragPos);

    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.Shininess);

    float distance = length(light.position - TangentFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * Color.rgb;
    vec3 diffuse = light.diffuse * diff * Color.rgb;
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
    return (ambient + diffuse + specular);
}

vec3 CalcDirLight(MaterialInfo material, vec2 uv)
{
    vec3 viewDir = normalize(scenedata.viewPos - FragPos);
    vec3 lightDir = normalize(-scenedata.dlight.direction);

    float diff = max(dot(Normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);

    vec3 ambient = scenedata.dlight.ambient * Color.rgb;
    vec3 diffuse = scenedata.dlight.diffuse * diff * Color.rgb;
    vec3 specular = scenedata.dlight.specular * spec * material.Specular;
    if(material.DiffuseMapID != 0)
    {
        ambient = scenedata.dlight.ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = scenedata.dlight.diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if(material.SpecularMapID != 0)
    {
        specular = scenedata.dlight.specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(MaterialInfo material, PointLight light, vec2 uv)
{
    vec3 viewDir = normalize(scenedata.viewPos - FragPos);
    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max(dot(Normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    vec3 ambient = light.ambient * Color.rgb;
    vec3 diffuse = light.diffuse * diff * Color.rgb;
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
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(MaterialInfo material, SpotLight light, vec2 uv)
{
    vec3 viewDir = normalize(scenedata.viewPos - FragPos);
    vec3 lightDir = normalize(light.position - FragPos);

    float diff = max(dot(Normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);

    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * Color.rgb;
    vec3 diffuse = light.diffuse * diff * Color.rgb;
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
    return (ambient + diffuse + specular);
}


vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir)
{ 
    const float heightScale = meshProperties[ConstMesh.MeshIndex].heightScale;
    const float minLayers = meshProperties[ConstMesh.MeshIndex].minLayers;
    const float maxLayers = meshProperties[ConstMesh.MeshIndex].maxLayers;
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