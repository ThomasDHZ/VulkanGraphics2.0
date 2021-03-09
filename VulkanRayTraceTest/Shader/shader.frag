#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_debug_printf : enable

#include "Lighting.glsl"

layout(push_constant) uniform ConstMeshProperties
{
	uint MeshID;
	uint ModelID;
	uint MaterialID;
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
	mat4 model;
	DirectionalLight dlight;
	vec3 viewPos;
	PointLight plight;
	float vertexSize;	
	mat4 PVM;
	mat4 BoneTransform[100];
} scenedata;
//layout(binding = 3) uniform MeshProperties 
//{
//	mat4 model;
//	mat4 BoneTransform[100];
//	vec2 UVOffset;
//} meshProperties;

layout(binding = 5) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 6) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 7) uniform sampler2D TextureMap[];


layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
layout(location = 3) in mat3 TBN;

layout(location = 0) out vec4 outColor;


Material BuildMaterial()
{
	Material material;
	material.Ambient = MaterialList[ConstMesh.MeshID].material.Ambient;
	material.Diffuse = MaterialList[ConstMesh.MeshID].material.Diffuse;
	material.Specular = MaterialList[ConstMesh.MeshID].material.Specular;
	material.Shininess = MaterialList[ConstMesh.MeshID].material.Shininess;
	material.Reflectivness = MaterialList[ConstMesh.MeshID].material.Reflectivness;

	if(MaterialList[ConstMesh.MeshID].material.DiffuseMapID == 0)
	{
		material.DiffuseMap = MaterialList[ConstMesh.MeshID].material.Diffuse;
	}
	else
	{
		material.DiffuseMap = vec3(texture(TextureMap[MaterialList[ConstMesh.MeshID].material.DiffuseMapID], TexCoords));
	}

	if(MaterialList[ConstMesh.MeshID].material.SpecularMapID == 0)
	{
		material.SpecularMap = vec3(texture(TextureMap[MaterialList[ConstMesh.MeshID].material.SpecularMapID], TexCoords));
	}
	else
	{
		material.SpecularMap = MaterialList[ConstMesh.MeshID].material.Specular;
	}

	material.SpecularMap = vec3(texture(TextureMap[MaterialList[ConstMesh.MeshID].material.SpecularMapID], TexCoords));
	material.NormalMap = vec3(texture(TextureMap[MaterialList[ConstMesh.MeshID].material.NormalMapID], TexCoords));
	material.AlphaMap = vec3(texture(TextureMap[MaterialList[ConstMesh.MeshID].material.AlphaMapID], TexCoords));
	material.EmissionMap = vec3(texture(TextureMap[MaterialList[ConstMesh.MeshID].material.EmissionMapID], TexCoords));
	material.ShadowMap = vec3(texture(TextureMap[MaterialList[ConstMesh.MeshID].material.ShadowMapID], TexCoords));
	return material;
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

void main() 
{
    vec3 TangentLightPos = TBN * scenedata.plight.position;
    vec3 TangentViewPos  = TBN * scenedata.viewPos;
    vec3 TangentFragPos  = TBN * FragPos;
  // offset texture coordinates with Parallax Mapping
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec2 texCoords = TexCoords;
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

	if(texture(TextureMap[4], texCoords).r == 0.0f)
	{
		discard;
	}
    outColor = vec4(texture(TextureMap[1], texCoords).rgb, 1.0);
}