#pragma once
#include "Mesh.h"
#include "PerspectiveCamera.h"
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
	void Update(VulkanEngine& engine, MaterialManager& materialManager, std::shared_ptr<Camera> camera);
	void UpdateBufferIndex(VulkanEngine& engine);
	void Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, VkPipelineLayout layout, RenderPassID RendererID, std::shared_ptr<Camera> CameraView);
	void Destroy(VulkanEngine& engine);
		
	std::vector<VkDescriptorBufferInfo> GetVertexBufferListDescriptors();
	std::vector<VkDescriptorBufferInfo> GetIndexBufferListDescriptors();
	std::vector<VkDescriptorBufferInfo> GetTransformBufferListDescriptors();
	std::vector<VkDescriptorBufferInfo> GetMeshPropertiesListDescriptors();

	std::shared_ptr<Mesh> GetMeshIndex(uint32_t MeshID);
	uint32_t GetMeshDescriptorCount() { return MeshList.size(); }
};

