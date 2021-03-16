#pragma once
#include "VulkanBuffer.h"

struct DirectionalLight {
	alignas(16) glm::vec3 direction;

	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;
};

struct PointLight {
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 ambient;
	alignas(16) glm::vec3 diffuse;
	alignas(16) glm::vec3 specular;
	alignas(4) float constant = 1.0f;
	alignas(4) float linear = 0.09f;
	alignas(4) float quadratic = 0.032f;
};

struct SceneDataBuffer {
	alignas(16) glm::mat4 viewInverse;
	alignas(16) glm::mat4 projInverse;
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	DirectionalLight dlight;
	alignas(16) glm::vec3 viewPos;
	PointLight plight;
	alignas(4) float vertexSize;
	alignas(16) glm::mat4 PVM;
	alignas(16) glm::mat4 BoneTransform[1000];
	alignas(4) float DepthSampler;
};

struct MeshProperties
{
	alignas(16) glm::mat4 ModelTransform;
	alignas(16) glm::mat4 BoneTransform[100];
	alignas(16) glm::vec2 UVOffset = glm::vec2(0.0f);
	alignas(4)  uint32_t MaterialID = 0;
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

	UniformData(VulkanEngine& engine)
	{
		VulkanBufferData.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(T), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}

	void Update(VulkanEngine& engine)
	{
		VulkanBufferData.CopyBufferToMemory(engine.Device, &UniformDataInfo, sizeof(T));
	}

	void Destroy(VulkanEngine& engine)
	{
		VulkanBufferData.DestoryBuffer(engine.Device);
	}
};

typedef UniformData<SceneDataBuffer> SceneDataUniformBuffer;
typedef UniformData<MeshProperties> MeshPropertiesUniformBuffer;