#pragma once
#include <vulkan\vulkan_core.h>
#include <vector>
#include "Vertex.h"
#include "VulkanEngine.h"


class TransformBuffer
{
private:
	VkBuffer transformBuffer = VK_NULL_HANDLE;
	VkDeviceMemory TransformBufferMemory = VK_NULL_HANDLE;

public:
	TransformBuffer();
	TransformBuffer(VulkanEngine& engine, const VkTransformMatrixKHR& TransformData);
	~TransformBuffer();

	void Destory(VulkanEngine& engine);

	VkBuffer GetTransformBuffer() { return transformBuffer; }
};

