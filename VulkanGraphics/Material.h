#pragma once
#include "Vulkanengine.h"
#include "TextureManager.h"

const uint32_t DefaultTextureID = 0;
const uint32_t DefaultAlphaTextureID = 1;

struct MaterialTexture
{
	glm::vec3 Ambient = glm::vec3(0.2f);
	glm::vec3 Diffuse = glm::vec3(0.6f);
	glm::vec3 Specular = glm::vec3(1.0f);
	float Shininess = 32;
	float Reflectivness = 0.0f;

	std::shared_ptr<Texture> DiffuseMap = nullptr;
	std::shared_ptr<Texture> SpecularMap = nullptr;
	std::shared_ptr<Texture> NormalMap = nullptr;
	std::shared_ptr<Texture> DepthMap = nullptr;
	std::shared_ptr<Texture> AlphaMap = nullptr;
	std::shared_ptr<Texture> EmissionMap = nullptr;
	std::shared_ptr<Texture> ShadowMap = nullptr;

	//PBR
	glm::vec3 Albedo = glm::vec3(0.5, 0.0f, 0.0f);
	float Matallic = .75f;
	float Roughness = .5f;
	float AmbientOcclusion = 1.0f;
	float Alpha = 1.0f;

	std::shared_ptr<Texture> AlbedoMap = nullptr;
	std::shared_ptr<Texture> MatallicMap = nullptr;
	std::shared_ptr<Texture> RoughnessMap = nullptr;
	std::shared_ptr<Texture> AOMap = nullptr;
};

struct MaterialData
{
	alignas(16) glm::vec3 Ambient = glm::vec3(0.2f);
	alignas(16) glm::vec3 Diffuse = glm::vec3(0.6f);
	alignas(16) glm::vec3 Specular = glm::vec3(1.0f);
	alignas(4) float Shininess = 32;
	alignas(4) float Reflectivness = 0.0f;

	alignas(4) uint32_t DiffuseMapID = DefaultTextureID;
	alignas(4) uint32_t SpecularMapID = DefaultTextureID;
	alignas(4) uint32_t NormalMapID = DefaultTextureID;
	alignas(4) uint32_t DepthMapID = DefaultTextureID;
	alignas(4) uint32_t AlphaMapID = DefaultAlphaTextureID;
	alignas(4) uint32_t EmissionMapID = DefaultTextureID;
	alignas(4) uint32_t ShadowMapID = DefaultTextureID;
	alignas(4) uint32_t CubeMapID = DefaultTextureID;
	alignas(4) uint32_t IrradianceMapID = DefaultTextureID;
	alignas(4) uint32_t PrefilterMapID = DefaultTextureID;

	//PBR
	alignas(16) glm::vec3 Albedo = glm::vec3(0.5, 0.0f, 0.0f);
	alignas(4) float Matallic = .75f;
	alignas(4) float Roughness = .5f;
	alignas(4) float AmbientOcclusion = 1.0f;
	alignas(4) float Alpha = 1.0f;

	alignas(4) uint32_t AlbedoMapID = 0;
	alignas(4) uint32_t MatallicMapID = 0;
	alignas(4) uint32_t RoughnessMapID = 0;
	alignas(4) uint32_t AOMapID = 0;

	void operator=(const MaterialTexture& materialTexture)
	{
		Ambient = materialTexture.Ambient;
		Diffuse = materialTexture.Diffuse;
		Specular = materialTexture.Specular;
		Shininess = materialTexture.Shininess;
		Reflectivness = materialTexture.Reflectivness;

		if (materialTexture.DiffuseMap != nullptr)
		{
			DiffuseMapID = materialTexture.DiffuseMap->TextureBufferIndex;
		}
		if (materialTexture.SpecularMap != nullptr)
		{
			SpecularMapID = materialTexture.SpecularMap->TextureBufferIndex;
		}
		if (materialTexture.NormalMap != nullptr)
		{
			NormalMapID = materialTexture.NormalMap->TextureBufferIndex;
		}
		if (materialTexture.DepthMap != nullptr)
		{
			DepthMapID = materialTexture.DepthMap->TextureBufferIndex;
		}
		if (materialTexture.AlphaMap != nullptr)
		{
			AlphaMapID = materialTexture.AlphaMap->TextureBufferIndex;
		}
		if (materialTexture.EmissionMap != nullptr)
		{
			EmissionMapID = materialTexture.EmissionMap->TextureBufferIndex;
		}
		if (materialTexture.ShadowMap != nullptr)
		{
			ShadowMapID = materialTexture.ShadowMap->TextureBufferIndex;
		}


		Albedo = materialTexture.Albedo;
		Matallic = materialTexture.Matallic;
		Roughness = materialTexture.Roughness;
		AmbientOcclusion = materialTexture.AmbientOcclusion;
		Alpha = materialTexture.Alpha;

		if (materialTexture.AlbedoMap != nullptr)
		{
			AlbedoMapID = materialTexture.AlbedoMap->TextureBufferIndex;
		}
		else
		{
			AlbedoMapID = 0;
		}

		if (materialTexture.MatallicMap != nullptr)
		{
			MatallicMapID = materialTexture.MatallicMap->TextureBufferIndex;
		}
		else
		{
			MatallicMapID = 0;
		}

		if (materialTexture.RoughnessMap != nullptr)
		{
			RoughnessMapID = materialTexture.RoughnessMap->TextureBufferIndex;
		}
		else
		{
			RoughnessMapID = 0;
		}

		if (materialTexture.AOMap != nullptr)
		{
			AOMapID = materialTexture.AOMap->TextureBufferIndex;
		}
		else
		{
			AOMapID = 0;
		}
	}
};

class Material
{
private:
	std::string MaterialName;
	uint32_t MaterialID = 0;
	uint32_t MaterialBufferIndex = 0;
	MaterialData MaterialBufferData;
	VulkanBuffer MaterialBuffer;

public:
	MaterialTexture MaterialTextureData;

	Material();
	Material(const std::string materialName);
	Material(const std::string materialName, MaterialTexture& MaterialInfo);
	~Material();

	void UpdateTextureIndexs();
	void UpdateAmbientProperty(glm::vec3 Ambient);
	void UpdateDiffuseProperty(glm::vec3 Diffuse);
	void UpdateDiffuseProperty(std::shared_ptr<Texture> DiffuseMap);
	void UpdateSpecularProperty(glm::vec3 Specular);
	void UpdateSpecularProperty(std::shared_ptr<Texture> SpecularMap);
	void UpdateShininessProperty(float Shininess);
	void UpdateReflectivnessProperty(float Reflectivness);
	void UpdateNormalProperty(std::shared_ptr<Texture> NormalMap);
	void UpdateDepthProperty(std::shared_ptr<Texture> DepthMap);
	void UpdateAlphaProperty(std::shared_ptr<Texture> AlphaMap);
	void UpdateEmissionMapProperty(std::shared_ptr<Texture> EmissionMap);
	void UpdateShadowMapProperty(std::shared_ptr<Texture> ShadowMap);
	void UpdateAlbedoProperty(glm::vec3 Albedo);
	void UpdateMatallicProperty(float Matallic);
	void UpdateRoughnessProperty(float Roughness);
	void UpdateAmbientOcclusionProperty(float AmbientOcclusion);
	void UpdateAlphaProperty(float Alpha);
	void Delete();

	void SetMaterialBufferIndex(uint32_t BufferIndex) { MaterialBufferIndex = BufferIndex; }

	std::string GetMaterialName() { return MaterialName; }
	uint32_t GetMaterialID() { return MaterialID; }
	uint32_t GetMaterialBufferIndex() { return MaterialBufferIndex; }
	VkBuffer GetMaterialBuffer() { return MaterialBuffer.Buffer; }
};