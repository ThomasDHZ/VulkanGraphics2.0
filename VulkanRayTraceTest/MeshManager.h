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
	void Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, VkPipelineLayout layout, std::shared_ptr<Camera> CameraView);
	void Destroy();
		
	std::vector<VkDescriptorBufferInfo> GetVertexBufferListDescriptors();
	std::vector<VkDescriptorBufferInfo> GetIndexBufferListDescriptors();
	std::vector<VkDescriptorBufferInfo> GetTransformBufferListDescriptors();
	std::vector<VkDescriptorBufferInfo> GetMeshPropertiesListDescriptors();

	std::shared_ptr<Mesh> GetMeshIndex(uint32_t MeshID);
	uint32_t GetMeshDescriptorCount();
};

class MeshManagerPtr
{
private:
	static std::shared_ptr<MeshManager> meshManager;
public:
	static void SetUpPtr(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<InputManager> inputManager, std::shared_ptr<MaterialManager> materialManager)
	{
		if (meshManager == nullptr)
		{
			meshManager = std::make_shared<MeshManager>(MeshManager(engine, inputManager, materialManager));
		}
		else
		{
			std::cout << "Mesh Manager has already been initialized." << std::endl;
		}
	}

	static std::shared_ptr<MeshManager> GetCameraManagerPtr()
	{
		return meshManager;
	}
};