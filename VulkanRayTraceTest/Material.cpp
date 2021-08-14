#include "Material.h"

Material::Material()
{

}

Material::Material(std::shared_ptr<VulkanEngine> engine)
{
	materialTexture.DiffuseMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	materialTexture.SpecularMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	materialTexture.NormalMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	materialTexture.DepthMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	materialTexture.AlphaMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultAlphaTexture");
	materialTexture.EmissionMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	materialTexture.ShadowMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	materialTexture.AlbedoMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	materialTexture.MatallicMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	materialTexture.RoughnessMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");
	materialTexture.AOMap = TextureManagerPtr::GetTextureManagerPtr()->GetTextureByName("DefaultTexture");

	MaterialBuffer.CreateBuffer(sizeof(MaterialData), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &materialData);
	UpdateBufferIndexs();
}

Material::Material(MaterialTexture& MaterialInfo)
{
	materialTexture = MaterialInfo;

	MaterialBuffer.CreateBuffer(sizeof(MaterialData), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &materialData);
	UpdateBufferIndexs();
}

Material::~Material()
{
}

void Material::UpdateBufferIndexs()
{
	materialData = materialTexture;
	MaterialBuffer.CopyBufferToMemory(&materialData, sizeof(materialData));
}

void Material::Delete()
{
	MaterialBuffer.DestoryBuffer();
} 