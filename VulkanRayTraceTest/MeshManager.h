#pragma once
#include "Mesh.h"
#include <vector>
#include <memory>

class MeshManager
{
private: 
protected:

public:
	std::vector<std::shared_ptr<Mesh>> MeshList;

	MeshManager();
	~MeshManager();

	void AddMesh(std::shared_ptr<Mesh> mesh);
	void Update(VulkanEngine& engine);
	void UpdateBufferIndex(VulkanEngine& engine);
	void Draw(VkCommandBuffer& commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline);

	std::vector<VkDescriptorBufferInfo> GetVertexBufferListDescriptors();
	std::vector<VkDescriptorBufferInfo> GetIndexBufferListDescriptors();
	std::vector<VkDescriptorBufferInfo> GetTransformBufferListDescriptors();
	std::vector<VkDescriptorBufferInfo> GetMeshPropertiesListDescriptors();

	uint32_t GetMeshDescriptorCount() { return MeshList.size(); }
};

