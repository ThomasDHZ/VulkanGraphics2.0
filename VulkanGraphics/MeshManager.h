#pragma once
#include "Mesh.h"
#include "PerspectiveCamera.h"
#include <vector>
#include <memory>
#include "GUIMesh.h"
#include "MaterialManager.h"


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
	void DeleteMesh(uint32_t MeshBufferIndex);

	void Update(std::shared_ptr<Camera> camera);
	void UpdateBufferIndex();
	void Draw(RendererDrawFlag renderPass, VkCommandBuffer& commandBuffer, VkPipelineLayout& ShaderLayout, std::shared_ptr<Camera> CameraView);
	void ClearMeshs();
	void Destroy();
		
	std::vector<VkDescriptorBufferInfo> GetVertexBufferDescriptorsList();
	std::vector<VkDescriptorBufferInfo> GetIndexBufferDescriptorsList();
	std::vector<VkDescriptorBufferInfo> GetTransformBufferDescriptorsList();
	std::vector<VkDescriptorBufferInfo> GetMeshPropertiesDescriptorsList();

	std::shared_ptr<Mesh> GetMeshIndex(uint32_t MeshID);
	std::vector<std::shared_ptr<Mesh>> GetMeshByType(MeshTypeFlag type);

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

	static std::shared_ptr<MeshManager> GetMeshManagerPtr()
	{
		return meshManager;
	}
};