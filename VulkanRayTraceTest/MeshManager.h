#pragma once
#include "Mesh.h"
#include "PerspectiveCamera.h"
#include <vector>
#include <memory>
#include "GUIMesh.h"


class MeshManager
{
private: 
	std::shared_ptr<VulkanEngine> engine;
	std::shared_ptr<InputManager> inputManager;
	std::shared_ptr<MaterialManager> materialManager;
public:
	std::vector<std::shared_ptr<Mesh>> MeshList;

	MeshManager();
	MeshManager(std::shared_ptr<VulkanEngine> Engine, std::shared_ptr<InputManager> inputManager, std::shared_ptr<MaterialManager> materialManager);
	~MeshManager();

	void AddMesh(std::shared_ptr<Mesh> mesh);
	void Update(std::shared_ptr<Camera> camera);
	void UpdateBufferIndex();
	void Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, VkPipelineLayout layout, RenderPassID RendererID, std::shared_ptr<Camera> CameraView);
	void Destroy();
		
	std::vector<VkDescriptorBufferInfo> GetVertexBufferListDescriptors();
	std::vector<VkDescriptorBufferInfo> GetIndexBufferListDescriptors();
	std::vector<VkDescriptorBufferInfo> GetTransformBufferListDescriptors();
	std::vector<VkDescriptorBufferInfo> GetMeshPropertiesListDescriptors();

	std::shared_ptr<Mesh> GetMeshIndex(uint32_t MeshID);
	uint32_t GetMeshDescriptorCount();
};
namespace GlobalPtr
{
	static std::shared_ptr<MeshManager> meshManagerPtr;
}
