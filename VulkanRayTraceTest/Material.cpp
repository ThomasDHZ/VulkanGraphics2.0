#include "Material.h"

Material::Material()
{

}

Material::Material(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<TextureManager> textureManager)
{
	materialTexture.DiffuseMap = textureManager->GetTextureByName("DefaultTexture");
	materialTexture.SpecularMap = textureManager->GetTextureByName("DefaultTexture");
	materialTexture.NormalMap = textureManager->GetTextureByName("DefaultTexture");
	materialTexture.DepthMap = textureManager->GetTextureByName("DefaultTexture");
	materialTexture.AlphaMap = textureManager->GetTextureByName("DefaultAlphaTexture");
	materialTexture.EmissionMap = textureManager->GetTextureByName("DefaultTexture");
	materialTexture.ShadowMap = textureManager->GetTextureByName("DefaultTexture");
	materialTexture.AlbedoMap = textureManager->GetTextureByName("DefaultTexture");
	materialTexture.MatallicMap = textureManager->GetTextureByName("DefaultTexture");
	materialTexture.RoughnessMap = textureManager->GetTextureByName("DefaultTexture");
	materialTexture.AOMap = textureManager->GetTextureByName("DefaultTexture");

	MaterialBuffer.CreateBuffer(sizeof(MaterialData), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &materialData);
	UpdateBufferIndexs(engine);
}

Material::Material(std::shared_ptr<VulkanEngine> engine, MaterialTexture& MaterialInfo)
{
	materialTexture = MaterialInfo;

	MaterialBuffer.CreateBuffer(sizeof(MaterialData), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &materialData);
	UpdateBufferIndexs(engine);
}

Material::~Material()
{
}

void Material::UpdateBufferIndexs(std::shared_ptr<VulkanEngine> engine)
{
	materialData = materialTexture;
	MaterialBuffer.CopyBufferToMemory(&materialData, sizeof(materialData));
}

void Material::Delete(std::shared_ptr<VulkanEngine> engine)
{
	MaterialBuffer.DestoryBuffer();
} 