#pragma once
#include "Vulkanengine.h"
#include "Model.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "LightManager.h"
#include "CameraManager.h"
#include "InputManager.h"
#include "GUIManager.h"
#include "ObjectManager.h"

class AssetManager
{
private:

public:
	std::shared_ptr<SceneDataUniformBuffer> SceneData;

	std::shared_ptr<InputManager> inputManager;
	std::shared_ptr<CameraManager> cameraManager;
	std::shared_ptr<MaterialManager> materialManager;
	std::shared_ptr<TextureManager> textureManager;
	std::shared_ptr<MeshManager> meshManager;
	std::shared_ptr<LightManager> lightManager;
	std::shared_ptr<GUIManager> guiManager;
	std::shared_ptr<ObjectManager> ObjManager;

	AssetManager();
	AssetManager(std::shared_ptr<VulkanEngine> engine);
	~AssetManager();

	void LoadFont(std::shared_ptr<VulkanEngine> engine, const std::string FontLocation);

	void Update();
	void Draw(VkCommandBuffer commandBuffer, VkPipelineLayout& ShaderLayout, std::shared_ptr<Camera> CameraView);
	void GUIDraw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	void Delete();

	std::vector<std::shared_ptr<Mesh>> GetMeshByType(MeshTypeFlag type);

	std::vector<VkDescriptorBufferInfo> GetVertexBufferListDescriptors() { return meshManager->GetVertexBufferListDescriptors(); }
	std::vector<VkDescriptorBufferInfo> GetIndexBufferListDescriptors() { return meshManager->GetIndexBufferListDescriptors(); }
	std::vector<VkDescriptorBufferInfo> GetTransformBufferListDescriptors() { return meshManager->GetTransformBufferListDescriptors(); }
	std::vector<VkDescriptorBufferInfo> GetMeshPropertiesListDescriptors() { return meshManager->GetMeshPropertiesListDescriptors(); }
	std::vector<VkDescriptorBufferInfo> GetMaterialBufferListDescriptor() { return materialManager->GetMaterialBufferListDescriptor(); }
	std::vector<VkDescriptorImageInfo>  GetTextureBufferListDescriptor() { return textureManager->GetTextureBufferListDescriptor(); }
	std::vector<VkDescriptorImageInfo>  Get3DTextureBufferListDescriptor() { return textureManager->Get3DTextureBufferListDescriptor(); }
	VkDescriptorImageInfo  GetSkyBoxTextureBufferListDescriptor() { return textureManager->GetSkyBoxTextureBufferListDescriptor(); }

	uint32_t GetMeshDescriptorCount() { return meshManager->GetMeshDescriptorCount(); }
	uint32_t GetMaterialDescriptorCount() { return materialManager->GetMaterialDescriptorCount(); }
	uint32_t GetTextureBufferDescriptorCount() { return textureManager->GetTextureBufferDescriptorCount(); }
	uint32_t Get3DTextureBufferDescriptorCount() { return textureManager->Get3DTextureBufferDescriptorCount(); }

	template <class T>
	void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout& ShaderLayout, T ShaderConstBuffer)
	{
		meshManager->Draw(commandBuffer, ShaderLayout, ShaderConstBuffer);
	}
};

class AssetManagerPtr
{
private:
	static std::shared_ptr<AssetManager> assetPtr;
public:
	static void SetUpPtr(std::shared_ptr<VulkanEngine> engine)
	{
		if (assetPtr == nullptr)
		{
			assetPtr = std::make_shared<AssetManager>(AssetManager(engine));
		}
		else
		{
			std::cout << "Asset Manager has already been initialized." << std::endl;
		}
	}

	static std::shared_ptr<AssetManager> GetAssetPtr()
	{
		return assetPtr;
	}
};
