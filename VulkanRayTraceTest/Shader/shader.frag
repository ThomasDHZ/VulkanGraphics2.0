#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

#include "Lighting.glsl"

layout(push_constant) uniform MeshInfo
{
	int MeshID;
	int MaterialID;
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

layout(binding = 0) uniform UniformBufferObject {
	mat4 viewInverse;
	mat4 projInverse;
	mat4 view;
	mat4 proj;
	mat4 model;
	DirectionalLight dlight;
	vec3 viewPos;
	PointLight plight;
	int vertexSize;
} scenedata;
//layout(binding = 1, set = 0, rgba8) uniform image2D image;
//layout(binding = 3) buffer Vertices { vec4 v[]; } vertices[];
//layout(binding = 4) buffer Indices { uint i[]; } indices[];
layout(binding = 5) buffer MaterialInfos { MaterialInfo materialInfo[]; } MaterialList;
layout(binding = 6) uniform sampler2D TextureMap[];
//layout(binding = 8) readonly buffer _TexCoordBuf {float texcoord0[];};


layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 UV;

layout(location = 0) out vec4 outColor;


Material BuildMaterial()
{
	Material material;
	material.Ambient = MaterialList.materialInfo[Mesh.MeshID].Ambient;
	material.Diffuse = MaterialList.materialInfo[Mesh.MeshID].Diffuse;
	material.Specular = MaterialList.materialInfo[Mesh.MeshID].Specular;
	material.Shininess = MaterialList.materialInfo[Mesh.MeshID].Shininess;
	material.Reflectivness = MaterialList.materialInfo[Mesh.MeshID].Reflectivness;
	material.DiffuseMap = vec3(texture(TextureMap[MaterialList.materialInfo[Mesh.MeshID].DiffuseMapID], UV));
	material.SpecularMap = vec3(texture(TextureMap[MaterialList.materialInfo[Mesh.MeshID].SpecularMapID], UV));
	material.NormalMap = vec3(texture(TextureMap[MaterialList.materialInfo[Mesh.MeshID].NormalMapID], UV));
	material.AlphaMap = vec3(texture(TextureMap[MaterialList.materialInfo[Mesh.MeshID].AlphaMapID], UV));
	material.EmissionMap = vec3(texture(TextureMap[MaterialList.materialInfo[Mesh.MeshID].EmissionMapID], UV));
	material.ShadowMap = vec3(texture(TextureMap[MaterialList.materialInfo[Mesh.MeshID].ShadowMapID], UV));
	return material;
}

void main() 
{

	Material material = BuildMaterial();

	vec3 lightDir = normalize(-scenedata.dlight.direction);
	float diff = max(dot(scenedata.dlight.direction, lightDir), 0.0);

	vec3 ambient = scenedata.dlight.ambient *  material.DiffuseMap;
    vec3 diffuse = scenedata.dlight.diffuse * diff *   material.DiffuseMap;
 
 	vec3 halfwayDir = normalize(scenedata.dlight.direction + scenedata.viewPos);  
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), material.Shininess);
    vec3 specular = scenedata.dlight.specular * spec * material.SpecularMap;

	vec3 color = ambient + diffuse;

//	if(material.ShadowMap == vec3(1.0f, 0.0f, 0.0f))
//	{
//		color *= 0.3f;
//	}
//	else
//	{
//		color += specular;
//	}
	 outColor = vec4(color, 1.0f);
}