#include "Material.h"

Material::Material()
{

}

Material::Material(const std::string materialName)
{
	MaterialID = EnginePtr::GetEnginePtr()->GenerateID();
	MaterialName = materialName;

	MaterialTextureData.DiffuseMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	MaterialTextureData.SpecularMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	MaterialTextureData.NormalMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	MaterialTextureData.DepthMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	MaterialTextureData.AlphaMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultAlphaTexture");
	MaterialTextureData.EmissionMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	MaterialTextureData.ShadowMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	MaterialTextureData.AlbedoMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	MaterialTextureData.MatallicMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	MaterialTextureData.RoughnessMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	MaterialTextureData.AOMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");

	MaterialBuffer.CreateBuffer(sizeof(MaterialData), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &MaterialBufferData);
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

Material::Material(const std::string materialName, MaterialTexture& MaterialInfo)
{
	MaterialID = EnginePtr::GetEnginePtr()->GenerateID();
	MaterialName = materialName;
	MaterialTextureData = MaterialInfo;

	MaterialBuffer.CreateBuffer(sizeof(MaterialData), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &MaterialBufferData);
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

Material::~Material()
{
}

void Material::UpdateTextureIndexs()
{
	MaterialBufferData = MaterialTextureData;
	MaterialBuffer.CopyBufferToMemory(&MaterialBufferData, sizeof(MaterialBufferData));
}

void Material::Delete()
{
	MaterialBuffer.DestoryBuffer();
}

void Material::UpdateAmbientProperty(glm::vec3 Ambient)
{
	MaterialTextureData.Ambient = Ambient;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateDiffuseProperty(glm::vec3 Diffuse)
{
	MaterialTextureData.Diffuse = Diffuse;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateDiffuseProperty(std::shared_ptr<Texture> DiffuseMap)
{
	MaterialTextureData.DiffuseMap = DiffuseMap;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateSpecularProperty(glm::vec3 Specular)
{
	MaterialTextureData.Specular = Specular;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateSpecularProperty(std::shared_ptr<Texture> SpecularMap)
{
	MaterialTextureData.SpecularMap = SpecularMap;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateShininessProperty(float Shininess)
{
	MaterialTextureData.Shininess = Shininess;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateReflectivnessProperty(float Reflectivness)
{
	MaterialTextureData.Reflectivness = Reflectivness;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateNormalProperty(std::shared_ptr<Texture> NormalMap)
{
	MaterialTextureData.NormalMap = NormalMap;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateDepthProperty(std::shared_ptr<Texture> DepthMap)
{
	MaterialTextureData.DepthMap = DepthMap;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateAlphaProperty(std::shared_ptr<Texture> AlphaMap)
{
	MaterialTextureData.AlphaMap = AlphaMap;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateEmissionMapProperty(std::shared_ptr<Texture> EmissionMap)
{
	MaterialTextureData.EmissionMap = EmissionMap;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateShadowMapProperty(std::shared_ptr<Texture> ShadowMap)
{
	MaterialTextureData.ShadowMap = ShadowMap;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateAlbedoProperty(glm::vec3 Albedo)
{
	MaterialTextureData.Albedo = Albedo;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateMatallicProperty(float Matallic)
{
	MaterialTextureData.Matallic = Matallic;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateRoughnessProperty(float Roughness)
{
	MaterialTextureData.Roughness = Roughness;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateAmbientOcclusionProperty(float AmbientOcclusion)
{
	MaterialTextureData.AmbientOcclusion = AmbientOcclusion;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void Material::UpdateAlphaProperty(float Alpha)
{
	MaterialTextureData.Alpha = Alpha;
	UpdateTextureIndexs();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}
