#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "Lighting.glsl"

layout(push_constant) uniform MeshInfo
{
	uint MeshID;
	uint ModelID;
	uint MaterialID;
} Mesh;

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
	int vertexSize;	
	mat4 PVM;
	mat4 BoneTransform[100];
} scenedata;
layout(binding = 5) buffer Transform { mat4 Transform; } MeshTransform[];
layout(binding = 6) buffer MaterialInfos { MaterialInfo material; } MaterialList[];
layout(binding = 7) uniform sampler2D TextureMap[];


layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 UV;

layout(location = 0) out vec4 outColor;


Material BuildMaterial()
{
	Material material;
	material.Ambient = MaterialList[Mesh.MeshID].material.Ambient;
	material.Diffuse = MaterialList[Mesh.MeshID].material.Diffuse;
	material.Specular = MaterialList[Mesh.MeshID].material.Specular;
	material.Shininess = MaterialList[Mesh.MeshID].material.Shininess;
	material.Reflectivness = MaterialList[Mesh.MeshID].material.Reflectivness;

	if(MaterialList[Mesh.MeshID].material.DiffuseMapID == 0)
	{
		material.DiffuseMap = MaterialList[Mesh.MeshID].material.Diffuse;
	}
	else
	{
		material.DiffuseMap = vec3(texture(TextureMap[MaterialList[Mesh.MeshID].material.DiffuseMapID], UV));
	}

	if(MaterialList[Mesh.MeshID].material.SpecularMapID == 0)
	{
		material.SpecularMap = vec3(texture(TextureMap[MaterialList[Mesh.MeshID].material.SpecularMapID], UV));
	}
	else
	{
		material.SpecularMap = MaterialList[Mesh.MeshID].material.Specular;
	}

	material.SpecularMap = vec3(texture(TextureMap[MaterialList[Mesh.MeshID].material.SpecularMapID], UV));
	material.NormalMap = vec3(texture(TextureMap[MaterialList[Mesh.MeshID].material.NormalMapID], UV));
	material.AlphaMap = vec3(texture(TextureMap[MaterialList[Mesh.MeshID].material.AlphaMapID], UV));
	material.EmissionMap = vec3(texture(TextureMap[MaterialList[Mesh.MeshID].material.EmissionMapID], UV));
	material.ShadowMap = vec3(texture(TextureMap[MaterialList[Mesh.MeshID].material.ShadowMapID], UV));
	return material;
}

void main() 
{

	Material material = BuildMaterial();


//	vec3 ambient  =  CalcAmbient(scenedata.dlight.ambient, vec3(0.7f));
//    vec3 diffuse  =  CalcDiffuse(scenedata.dlight.direction, scenedata.dlight.diffuse, vec3(0.7f));
//	vec3 specular = CalcSpecular(scenedata.dlight.direction , scenedata.dlight.specular, vec3(1.0f), scenedata.viewPos, Normal, material.Shininess);
//	vec3 color = ambient + diffuse + specular;

	 vec3 lightVector = normalize(scenedata.dlight.direction);
	float dot_product = max(dot(lightVector, Normal), 0.2);
	vec3 color = material.DiffuseMap * dot_product;


	 outColor = vec4(color, 1.0f);
}