#pragma once
#include "VulkanEngine.h"
#include "Model.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "ModelManager.h"
#include "LightManager.h"
#include "CameraManager.h"
#include "InputManager.h"
class AssetManager
{
private:

public:
	std::shared_ptr<SceneDataUniformBuffer> SceneData;
	std::shared_ptr<UniformData<SkyboxUniformBuffer>> SkyUniformBuffer;

	InputManager inputManager;
	CameraManager cameraManager;
	MaterialManager materialManager;
	TextureManager textureManager;
	MeshManager meshManager;
	ModelManager modelManager;
	LightManager lightManager;

	AssetManager();
	AssetManager(VulkanEngine& engine);
	~AssetManager();

	void AddModel();
	void AddModel(VulkanEngine& engine, const std::string& FilePath);
	void AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);
	void AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material>);

	void Update(VulkanEngine& engine, VulkanWindow& window);
	void Draw(VkCommandBuffer commandBuffer, VkRenderPassBeginInfo renderPassInfo, VkPipelineLayout layout, RenderPassID renderPassID, std::shared_ptr<Camera> CameraView);
	void Delete(VulkanEngine& engine);

	std::vector<std::shared_ptr<Mesh>> GetMeshByType(MeshTypeFlag type);

	std::vector<VkDescriptorBufferInfo> GetVertexBufferListDescriptors() { return meshManager.GetVertexBufferListDescriptors(); }
	std::vector<VkDescriptorBufferInfo> GetIndexBufferListDescriptors() { return meshManager.GetIndexBufferListDescriptors(); }
	std::vector<VkDescriptorBufferInfo> GetTransformBufferListDescriptors() { return meshManager.GetTransformBufferListDescriptors(); }
	std::vector<VkDescriptorBufferInfo> GetMeshPropertiesListDescriptors() { return meshManager.GetMeshPropertiesListDescriptors(); }
	std::vector<VkDescriptorBufferInfo> GetMaterialBufferListDescriptor() { return materialManager.GetMaterialBufferListDescriptor(); }
	std::vector<VkDescriptorImageInfo>  GetTextureBufferListDescriptor() { return textureManager.GetTextureBufferListDescriptor(); }
	std::vector<VkDescriptorImageInfo>  Get3DTextureBufferListDescriptor() { return textureManager.Get3DTextureBufferListDescriptor(); }
	VkDescriptorImageInfo  GetSkyBoxTextureBufferListDescriptor() { return textureManager.GetSkyBoxTextureBufferListDescriptor(); }

	uint32_t GetMeshDescriptorCount() { return meshManager.GetMeshDescriptorCount(); }
	uint32_t GetMaterialDescriptorCount() { return materialManager.GetMaterialDescriptorCount(); }
	uint32_t GetTextureBufferDescriptorCount() { return textureManager.GetTextureBufferDescriptorCount(); }
	uint32_t Get3DTextureBufferDescriptorCount() { return textureManager.Get3DTextureBufferDescriptorCount(); }
};

