#pragma once
#include <vulkan\vulkan_core.h>
#include <vector>
#include "Vertex.h"
#include "VulkanEngine.h"

class RayTraceVertexBuffer
{
private:
	unsigned int VertexCount = 0;
	VkBuffer vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory VertexBufferMemory = VK_NULL_HANDLE;

public:

	RayTraceVertexBuffer();
	RayTraceVertexBuffer(VulkanEngine& renderer, const std::vector<Vertex>& vertexdata);
	~RayTraceVertexBuffer();

	void Destory(VulkanEngine& renderer);

	unsigned int GetVertexCount() { return VertexCount; }
	VkBuffer GetVertexBuffer() { return vertexBuffer; }
};

