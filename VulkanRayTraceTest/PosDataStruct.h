#pragma once
#include "VulkanBuffer.h"
#include "VulkanEngine.h"

struct PosDataBufferData {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::mat4 lightSpaceMatrix;
	glm::mat4 BoneTransform[300];
};

struct PosDataStruct
{
	VulkanBuffer SceneDataBuffer;
	PosDataBufferData SceneData;

	PosDataStruct()
	{

	}

	PosDataStruct(VulkanEngine& engine)
	{
		SceneDataBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(PosDataStruct), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}

	void Update(VulkanEngine& engine)
	{
		SceneDataBuffer.CopyBufferToMemory(engine.Device, &SceneData, sizeof(PosDataStruct));
	}

	void Destroy(VulkanEngine& engine)
	{
		SceneDataBuffer.DestoryBuffer(engine.Device);
	}
};
