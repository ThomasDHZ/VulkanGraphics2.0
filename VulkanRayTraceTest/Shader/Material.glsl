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
//
//Material BuildMaterial(MaterialInfo[1000] MaterialList, vec2 UV, int InstanceCustomIndexEXT)
//{
//	Material material;
//	material.Ambient = MaterialList[InstanceCustomIndexEXT].material.Ambient;
//	material.Diffuse = MaterialList[InstanceCustomIndexEXT].material.Diffuse;
//	material.Specular = MaterialList[InstanceCustomIndexEXT].material.Specular;
//	material.Shininess = MaterialList[InstanceCustomIndexEXT].material.Shininess;
//	material.Reflectivness = MaterialList[InstanceCustomIndexEXT].material.Reflectivness;
//
//	if (MaterialList[InstanceCustomIndexEXT].material.DiffuseMapID == 0)
//	{
//		material.DiffuseMap = MaterialList[InstanceCustomIndexEXT].material.Diffuse;
//	}
//	else
//	{
//		material.DiffuseMap = vec3(texture(TextureMap[MaterialList[InstanceCustomIndexEXT].material.DiffuseMapID], UV));
//	}
//
//	if (MaterialList[gl_InstanceCustomIndexEXT].material.SpecularMapID == 0)
//	{
//		material.SpecularMap = vec3(texture(TextureMap[MaterialList[InstanceCustomIndexEXT].material.SpecularMapID], UV));
//	}
//	else
//	{
//		material.SpecularMap = MaterialList[InstanceCustomIndexEXT].material.Specular;
//	}
//
//	material.SpecularMap = vec3(texture(TextureMap[MaterialList[InstanceCustomIndexEXT].material.SpecularMapID], UV));
//	material.NormalMap = vec3(texture(TextureMap[MaterialList[InstanceCustomIndexEXT].material.NormalMapID], UV));
//	material.AlphaMap = vec3(texture(TextureMap[MaterialList[InstanceCustomIndexEXT].material.AlphaMapID], UV));
//	material.EmissionMap = vec3(texture(TextureMap[MaterialList[InstanceCustomIndexEXT].material.EmissionMapID], UV));
//	material.ShadowMap = vec3(texture(TextureMap[MaterialList[InstanceCustomIndexEXT].material.ShadowMapID], UV));
//	return material;
//}