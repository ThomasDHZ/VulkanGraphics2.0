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

Material BuildMaterial()
{
	Material material;
	material.Ambient = MaterialList.materialInfo[Mesh.MeshID].Ambient;
	material.Diffuse = MaterialList.materialInfo[Mesh.MeshID].Diffuse;
	material.Specular = MaterialList.materialInfo[Mesh.MeshID].Specular;
	material.Shininess = MaterialList.materialInfo[Mesh.MeshID].Shininess;
	material.Reflectivness = MaterialList.materialInfo[Mesh.MeshID].Reflectivness;

	if (MaterialList.materialInfo[Mesh.MeshID].DiffuseMapID == 0)
	{
		material.DiffuseMap = MaterialList.materialInfo[Mesh.MeshID].Diffuse;
	}
	else
	{
		material.DiffuseMap = vec3(texture(TextureMap[MaterialList.materialInfo[Mesh.MeshID].DiffuseMapID], UV));
	}

	if (MaterialList.materialInfo[Mesh.MeshID].SpecularMapID == 0)
	{
		material.SpecularMap = vec3(texture(TextureMap[MaterialList.materialInfo[Mesh.MeshID].SpecularMapID], UV));
	}
	else
	{
		material.SpecularMap = MaterialList.materialInfo[Mesh.MeshID].Specular;
	}

	material.SpecularMap = vec3(texture(TextureMap[MaterialList.materialInfo[Mesh.MeshID].SpecularMapID], UV));
	material.NormalMap = vec3(texture(TextureMap[MaterialList.materialInfo[Mesh.MeshID].NormalMapID], UV));
	material.AlphaMap = vec3(texture(TextureMap[MaterialList.materialInfo[Mesh.MeshID].AlphaMapID], UV));
	material.EmissionMap = vec3(texture(TextureMap[MaterialList.materialInfo[Mesh.MeshID].EmissionMapID], UV));
	material.ShadowMap = vec3(texture(TextureMap[MaterialList.materialInfo[Mesh.MeshID].ShadowMapID], UV));
	return material;
};