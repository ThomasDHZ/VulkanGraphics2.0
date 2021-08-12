#pragma once
#include "VulkanBuffer.h"
struct DirectionalLightBuffer {
	alignas(16) glm::vec3 direction;
	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;
};

struct PointLightBuffer {
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;
	alignas(4) float constant = 1.0f;
	alignas(4) float linear = 0.09f;
	alignas(4) float quadratic = 0.032f;
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
	alignas(4) float linear = 0.09f;
	alignas(4) float quadratic = 0.032f;
};

struct SceneDataBuffer {
	DirectionalLightBuffer dlight;
	PointLightBuffer plight[5];
	SpotLightBuffer sLight;
	alignas(16) glm::mat4 viewInverse;
	alignas(16) glm::mat4 projInverse;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec3 viewPos;
	alignas(4)  uint32_t DirectionalLightCount;
	alignas(4)  uint32_t PointLightCount;
	alignas(4)  uint32_t SpotLightCount;
	alignas(4)  float timer;
	alignas(4)  int Shadowed = 1;
	alignas(4)  int temp = 0;
	alignas(4)  float temp2 = 1.0f;
};

struct SkyboxUniformBuffer
{
	alignas(16) glm::mat4 viewInverse;
	alignas(16) glm::mat4 projInverse;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec3 viewPos;
};

struct MeshProperties
{
	alignas(16) glm::mat4 ModelTransform = glm::mat4(1.0f);
	alignas(8) glm::vec2 UVOffset = glm::vec2(0.0f);
	alignas(8) glm::vec2 UVScale = glm::vec2(1.0f);
	alignas(8) glm::vec2 UVFlip = glm::vec2(0.0f);
	alignas(4) uint32_t MaterialBufferIndex = 0;
    alignas(4) float heightScale = 0.1f;
	alignas(4) float minLayers = 8;
	alignas(4) float maxLayers = 32;
};

template <class T>
class UniformData
{
public:
	VulkanBuffer VulkanBufferData;
	T UniformDataInfo;

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

typedef UniformData<SceneDataBuffer> SceneDataUniformBuffer;
typedef UniformData<MeshProperties> MeshPropertiesUniformBuffer;