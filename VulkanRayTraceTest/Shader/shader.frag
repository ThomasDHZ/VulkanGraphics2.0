#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"

layout(push_constant) uniform ConstMeshProperties
{
	uint MeshIndex;
	uint MaterialIndex;
	vec2 UVOffset;
} ConstMesh;

struct VertexData
{
	vec3 Position;
	vec3 Normal;
	vec2 UV;
	vec3 Tangent;
	vec3 BiTangent;
};

struct MaterialInfo
{
	vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;    
    float Shininess;
    float Reflectivness;

	uint DiffuseMapID;
	uint SpecularMapID;
	uint NormalMapID;
	uint DepthMapID;
	uint AlphaMapID;
	uint EmissionMapID;
		uint ShadowMapID;
};

struct Material
{
	vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;    
    float Shininess;
    float Reflectivness;

	vec3 DiffuseMap;
	vec3 SpecularMap;
	vec3 NormalMap;
	vec3 DepthMap;
	vec3 AlphaMap;
	vec3 EmissionMap;
	vec3 ShadowMap;
};

layout(binding = 2) uniform UniformBufferObject {
	mat4 viewInverse;
	mat4 projInverse;
	mat4 view;
	mat4 proj;
	DirectionalLight dlight;
	vec3 viewPos;
	PointLight plight;
    float timer;
    int temp;
} scenedata;

layout(binding = 3) buffer MeshProperties 
{
	mat4 ModelTransform;
	mat4 BoneTransform[100];
	vec2 UVOffset;
} meshProperties[];

layout(binding = 6) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 7) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 8) uniform sampler2D TextureMap[];
layout(binding = 9) uniform sampler3D Texture3DMap[];

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 outColor;

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

void main() 
{
    vec3 TangentLightPos = TBN * scenedata.plight.position;
    vec3 TangentViewPos  = TBN * scenedata.viewPos;
    vec3 TangentFragPos  = TBN * FragPos;
  // offset texture coordinates with Parallax Mapping
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec2 texCoords = TexCoords + ConstMesh.UVOffset;
//    
//    texCoords = ParallaxMapping(TexCoords,  viewDir);       
//    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
//        discard;
//
//    // obtain normal from normal map
//    vec3 normal = texture(TextureMap[2], texCoords).rgb;
//    normal = normalize(normal * 2.0 - 1.0);   
//   
//    // get diffuse color
//    vec3 color = texture(TextureMap[1], texCoords).rgb;
//    // ambient
//    vec3 ambient = 0.1 * color;
//    // diffuse
//    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
//    float diff = max(dot(lightDir, normal), 0.0);
//    vec3 diffuse = diff * color;
//    // specular    
//    vec3 reflectDir = reflect(-lightDir, normal);
//    vec3 halfwayDir = normalize(lightDir + viewDir);  
//    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

//    vec3 specular = vec3(0.2) * spec;


const MaterialInfo material = MaterialList[ConstMesh.MaterialIndex].material;
if(texture(TextureMap[material.AlphaMapID], texCoords).r == 0.0f)
	{
		discard;
	}
    outColor = vec4(texture(TextureMap[material.DiffuseMapID], texCoords).rgb, 1.0);
}