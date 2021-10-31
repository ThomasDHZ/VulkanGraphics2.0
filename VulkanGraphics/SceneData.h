#pragma once
#include "VulkanBuffer.h"


const uint32_t DefaultTextureID = 0;
const uint32_t DefaultAlphaTextureID = 1;

struct DirectionalLightBuffer {
	alignas(16) glm::vec3 direction;
	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;
	alignas(16) glm::mat4 LightSpaceMatrix;
};

struct PointLightBuffer {
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;
	alignas(4) float constant = 1.0f;
	alignas(4) float linear = 0.022f;
	alignas(4) float quadratic = 0.0019f;
	alignas(16) glm::mat4 LightSpaceMatrix;
};

struct SpotLightBuffer {
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 direction;
	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;

	alignas(4) float cutOff = glm::cos(glm::radians(12.5f));
	alignas(4) float outerCutOff = glm::cos(glm::radians(15.0f));
	alignas(4) float constant = 1.0f;
	alignas(4) float linear = 0.022f;
	alignas(4) float quadratic = 0.0019f;
	alignas(16) glm::mat4 LightSpaceMatrix;
};

struct SphereAreaLightBuffer {
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;
	alignas(4) float SphereRadius = 1.0f;
	alignas(4) float linear = 0.022f;
	alignas(4) float quadratic = 0.0019f;
	alignas(4) float Luminosity = 100.0f;
};

struct TubeAreaLightBuffer {
	alignas(16) glm::vec3 StartPos;
	alignas(16) glm::vec3 EndPos;
	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;
	alignas(4) float TubeRadius = 1.0f;
	alignas(4) float Luminosity = 100.0f;
};

struct RectangleAreaLightBuffer
{
	alignas(16) glm::vec3 VertexPos1;
	alignas(16) glm::vec3 VertexPos2;
	alignas(16) glm::vec3 VertexPos3;
	alignas(16) glm::vec3 VertexPos4;
	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;
	alignas(4) float Luminosity = 100.0f;
};

struct SceneDataBuffer 
{
	alignas(16) glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
	alignas(4)  uint32_t DirectionalLightCount = 0;
	alignas(4)  uint32_t PointLightCount = 0;
	alignas(4)  uint32_t SpotLightCount = 0;
	alignas(4)  uint32_t SphereAreaLightCount = 0;
	alignas(4)  uint32_t TubeAreaLightCount = 0;
	alignas(4)  uint32_t RectangleAreaLightCount = 0;
	alignas(4)  float timer = 0.0f;
	alignas(4)  int Shadowed = 1;
	alignas(4)  int temp = 0;
	alignas(4)  float temp2 = 1.0f;
};

struct MaterialBuffer
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
};

struct MeshProperties
{
	MaterialBuffer material;
	alignas(16) glm::mat4 ModelTransform = glm::mat4(1.0f);
	alignas(16) glm::mat4 MeshTransform = glm::mat4(1.0f);
	alignas(8) glm::vec2 UVOffset = glm::vec2(0.0f);
	alignas(8) glm::vec2 UVScale = glm::vec2(1.0f);
	alignas(8) glm::vec2 UVFlip = glm::vec2(0.0f);
	alignas(4) uint32_t MaterialBufferIndex = 0;
    alignas(4) float heightScale = 0.1f;
	alignas(4) float minLayers = 8;
	alignas(4) float maxLayers = 32;
};

struct CubeSamplerBuffer
{
	alignas(16) glm::mat4 LightSpaceMatrix[6];
};

template <class T>
class UniformData
{
public:
	T UniformDataInfo;
	VulkanBuffer VulkanBufferData;

	UniformData()
	{
	}

	UniformData(std::shared_ptr<VulkanEngine> engine)
	{
		VulkanBufferData.CreateBuffer(sizeof(T), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}

	UniformData(T UniformData)
	{
		VulkanBufferData.CreateBuffer(sizeof(T), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		Update();
	}

	void Update()
	{
		VulkanBufferData.CopyBufferToMemory( &UniformDataInfo, sizeof(T));
	}

	void Update(T UniformData)
	{
		UniformDataInfo = UniformData;
		VulkanBufferData.CopyBufferToMemory(&UniformDataInfo, sizeof(T));
	}

	void Destroy()
	{
		VulkanBufferData.DestoryBuffer();
	}
};

typedef UniformData<CubeSamplerBuffer> CubeSampler;
typedef UniformData<SceneDataBuffer> SceneDataUniformBuffer;
typedef UniformData<MeshProperties> MeshPropertiesUniformBuffer;