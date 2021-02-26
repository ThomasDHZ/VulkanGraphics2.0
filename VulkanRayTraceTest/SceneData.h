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

struct SceneDataBufferData {
	alignas(16) glm::mat4 viewInverse;
	alignas(16) glm::mat4 projInverse;
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	DirectionalLight dlight;
	alignas(16) glm::vec3 viewPos;
	PointLight plight;
	alignas(4) int vertexSize;
	alignas(16) glm::mat4 PVM;
	alignas(16) glm::mat4 BoneTransform[100];
};

struct SceneDataStruct
{
	VulkanBuffer SceneDataBuffer;
	SceneDataBufferData SceneData;

	SceneDataStruct()
	{

	}

	SceneDataStruct(VulkanEngine& engine)
	{
		SceneDataBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(SceneDataBufferData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}

	void Update(VulkanEngine& engine)
	{
		SceneDataBuffer.CopyBufferToMemory(engine.Device, &SceneData, sizeof(SceneDataBufferData));
	}

	void Destroy(VulkanEngine& engine)
	{
		SceneDataBuffer.DestoryBuffer(engine.Device);
	}
};