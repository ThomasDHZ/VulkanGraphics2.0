#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"
#include "material.glsl"
#include "vertex.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshIndex;
    mat4 proj;
    mat4 view;
    vec3 CameraPos;
} ConstMesh;

struct VertexData
{
	vec3 Position;
	vec3 Normal;
	vec2 UV;
	vec3 Tangent;
	vec3 BiTangent;
};

layout(binding = 0) uniform UniformBufferObject 
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

layout(binding = 2) buffer DirectionalLight2
{ 
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} DLight[];

layout(binding = 3) buffer PointLight2
{ 
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
} PLight[];

layout(binding = 4) buffer SpotLight2
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

layout(binding = 5) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 6) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 7) uniform sampler2D TextureMap[];
layout(binding = 8) uniform sampler3D Texture3DMap[];
layout(binding = 9) uniform samplerCube CubeMap;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec3 BiTangent;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBloom;

vec3 CalcNormalDirLight(MaterialInfo material, vec3 FragPos, vec3 normal, vec2 uv, mat3 TBN)
{
    vec3 LightPos = DLight[0].direction;
    vec3 ViewPos = ConstMesh.CameraPos;
    vec3 FragPos2 = FragPos;
    if (material.NormalMapID != 0)
    {
        LightPos = TBN * DLight[0].direction;
        ViewPos = TBN * ConstMesh.CameraPos;
        FragPos2 = TBN * FragPos;
    }
    vec3 ViewDir = normalize(ViewPos - FragPos2);

    const vec3 lightDir = normalize(-LightPos);
    const float diff = max(dot(normal, lightDir), 0.0);

    const vec3 halfwayDir = normalize(lightDir + ViewDir);
    const float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0f);

    vec3 ambient = DLight[0].ambient * material.Diffuse.rgb;
    vec3 diffuse = DLight[0].diffuse * diff * material.Diffuse.rgb;
    vec3 specular = DLight[0].specular * spec * material.Specular;
    if (material.DiffuseMapID != 0)
    {
        ambient = DLight[0].ambient * vec3(texture(TextureMap[material.DiffuseMapID], uv));
        diffuse = DLight[0].diffuse * diff * vec3(texture(TextureMap[material.DiffuseMapID], uv));
    }
    if (material.SpecularMapID != 0)
    {
        specular = DLight[0].specular * spec * vec3(texture(TextureMap[material.SpecularMapID], uv));
    }

    return vec3(ambient + diffuse + specular);
}

vec2 ParallaxMapping(MaterialInfo material, vec2 texCoords, vec3 viewDir);
void main() 
{
   MaterialInfo material = MaterialList[meshProperties[ConstMesh.MeshIndex].MaterialIndex].material;
   vec2 texCoords = TexCoords + meshProperties[ConstMesh.MeshIndex].UVOffset;
   texCoords *= meshProperties[ConstMesh.MeshIndex].UVScale;
   if(meshProperties[ConstMesh.MeshIndex].UVFlip.x == 1.0f)
   {
        texCoords.x = 1.0f - texCoords.x;
   }
   if(meshProperties[ConstMesh.MeshIndex].UVFlip.y == 1.0f)
   {
        texCoords.y = 1.0f - texCoords.y;
   }
   if(texture(TextureMap[material.AlphaMapID], texCoords).r == 0.0f)
   {
	 discard;
   }

   vec3 T = normalize(mat3(meshProperties[ConstMesh.MeshIndex].ModelTransform * MeshTransform[ConstMesh.MeshIndex].Transform) * vec3(Tangent));
   vec3 B = normalize(mat3(meshProperties[ConstMesh.MeshIndex].ModelTransform * MeshTransform[ConstMesh.MeshIndex].Transform) * vec3(BiTangent));
   vec3 N = normalize(mat3(meshProperties[ConstMesh.MeshIndex].ModelTransform * MeshTransform[ConstMesh.MeshIndex].Transform) * Normal);
   mat3 TBN = transpose(mat3(T, B, N));

   vec3 result = vec3(0.0f);
   vec3 normal = Normal;
   vec3 ViewPos  = ConstMesh.CameraPos;
   vec3 FragPos2  = FragPos;
   if(material.NormalMapID != 0)
   {
        ViewPos  = TBN * ConstMesh.CameraPos;
        FragPos2  = TBN * FragPos;
   }
   const vec3 viewDir = normalize(ViewPos - FragPos2);

   if(material.NormalMapID != 0)
   {
//        if(material.DepthMapID != 0)
//        {
//            texCoords = ParallaxMapping(material, texCoords,  viewDir);       
//            if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
//            {
//              discard;
//            }
//        }
        normal = texture(TextureMap[material.NormalMapID], texCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
   }

   result = CalcNormalDirLight(material, FragPos2, normal, texCoords, TBN);
   outColor = vec4(result, material.Alpha);

   outBloom = vec4(0.0f, 0.0f, 0.0f, 1.0f);
   if(material.EmissionMapID != 0)
   {
        if(texture(TextureMap[material.EmissionMapID], texCoords).r != 0.0 &&
           texture(TextureMap[material.EmissionMapID], texCoords).g != 0.0 &&
           texture(TextureMap[material.EmissionMapID], texCoords).b != 0.0)
        {
            outColor = vec4(texture(TextureMap[material.EmissionMapID], texCoords).rgb, 1.0f);
        }
        outBloom = vec4(texture(TextureMap[material.EmissionMapID], texCoords).rgb, 1.0f);
   }
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
    vec2  currentTexCoords = texCoords;
    float currentDepthMapValue = texture(TextureMap[material.DepthMapID], currentTexCoords).r;

    while (currentLayerDepth < currentDepthMapValue)
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
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(TextureMap[material.DepthMapID], prevTexCoords).r - currentLayerDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}