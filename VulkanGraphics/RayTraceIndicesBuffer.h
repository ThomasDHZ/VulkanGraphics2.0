#pragma once
#include <vulkan\vulkan_core.h>
#include <vector>
#include "VulkanEngine.h"

class RayTraceIndicesBuffer
{
private:
	unsigned int IndiceCount = 0;
	VkBuffer indiceBuffer = VK_NULL_HANDLE;
	VkDeviceMemory IndiceBufferMemory = VK_NULL_HANDLE;

public:
	RayTraceIndicesBuffer();
	RayTraceIndicesBuffer(VulkanEngine& renderer, const std::vector<uint32_t>& indicesdata);
	~RayTraceIndicesBuffer();

	void Destory(VulkanEngine& renderer);

	unsigned int GetIndiceCount() { return IndiceCount; }
	VkBuffer GetIndiceBuffer() { return indiceBuffer; }
};
