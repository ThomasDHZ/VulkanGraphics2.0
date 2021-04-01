#pragma once
#include "VulkanEngine.h"
#include "TextureManager.h"

struct MaterialTexture
{
	glm::vec3 Ambient = glm::vec3(0.2f);
	glm::vec3 Diffuse = glm::vec3(0.6f);
	glm::vec3 Specular = glm::vec3(1.0f);
	float Shininess = 32;
	float Reflectivness = 0.0f;

	std::shared_ptr<Texture> DiffuseMap;
	std::shared_ptr<Texture> SpecularMap;
	std::shared_ptr<Texture> NormalMap;
	std::shared_ptr<Texture> DepthMap;
	std::shared_ptr<Texture> AlphaMap;
	std::shared_ptr<Texture> EmissionMap;
	std::shared_ptr<Texture> ShadowMap;
};

struct MaterialData
{
	alignas(16) glm::vec3 Ambient = glm::vec3(0.2f);
	alignas(16) glm::vec3 Diffuse = glm::vec3(0.6f);
	alignas(16) glm::vec3 Specular = glm::vec3(1.0f);
	alignas(4) float Shininess = 32;
	alignas(4) float Reflectivness = 0.0f;

	alignas(4) uint32_t DiffuseMapID = 0;
	alignas(4) uint32_t SpecularMapID = 0;
	alignas(4) uint32_t NormalMapID = 0;
	alignas(4) uint32_t DepthMapID = 0;
	alignas(4) uint32_t AlphaMapID = 1;
	alignas(4) uint32_t EmissionMapID = 0;
	alignas(4) uint32_t ShadowMapID = 0;

	void operator=(const MaterialTexture& materialTexture)
	{
		Ambient = materialTexture.Ambient;
		Diffuse = materialTexture.Diffuse;
		Specular = materialTexture.Specular;
		Shininess = materialTexture.Shininess;
		Reflectivness = materialTexture.Reflectivness;

		DiffuseMapID = materialTexture.DiffuseMap->TextureBufferIndex;
		SpecularMapID = materialTexture.SpecularMap->TextureBufferIndex;
		NormalMapID = materialTexture.NormalMap->TextureBufferIndex;
		DepthMapID = materialTexture.DepthMap->TextureBufferIndex;
		AlphaMapID = materialTexture.AlphaMap->TextureBufferIndex;
		EmissionMapID = materialTexture.EmissionMap->TextureBufferIndex;
		ShadowMapID = materialTexture.ShadowMap->TextureBufferIndex;
	}
};

class Material
{
private:
	MaterialData materialData;
	VulkanBuffer MaterialBuffer;

public:
	std::string MaterialName;
	uint32_t MaterialID = 0;
	uint32_t MaterialBufferIndex = 0;
	MaterialTexture materialTexture;

	Material();
	Material(VulkanEngine& engine, TextureManager& textureManager);
	Material(VulkanEngine& engine, MaterialTexture& MaterialInfo);
	~Material();

	void UpdateBufferIndexs(VulkanEngine& engine);
	void Delete(VulkanEngine& engine);

	VkBuffer GetMaterialBuffer() { return MaterialBuffer.Buffer; }
};