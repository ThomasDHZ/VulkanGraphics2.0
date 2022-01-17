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
	void Draw(RendererDrawFlag renderPass, VkCommandBuffer commandBuffer, VkPipelineLayout& ShaderLayout, std::shared_ptr<Camera> CameraView);
	void Draw(RendererDrawFlag renderPass, VkCommandBuffer& commandBuffer, VkPipelineLayout& ShaderLayout, ConstMeshInfo& a, std::shared_ptr<Camera> CameraView);
	void GUIDraw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	void Delete();

	std::vector<std::shared_ptr<Mesh>> GetMeshByType(MeshTypeFlag type);

	std::vector<VkDescriptorBufferInfo> GetVertexBufferDescriptorsList() { return meshManager->GetVertexBufferDescriptorsList(); }
	std::vector<VkDescriptorBufferInfo> GetIndexBufferDescriptorsList() { return meshManager->GetIndexBufferDescriptorsList(); }
	std::vector<VkDescriptorBufferInfo> GetTransformBufferDescriptorsList() { return meshManager->GetTransformBufferDescriptorsList(); }
	std::vector<VkDescriptorBufferInfo> GetMeshPropertiesDescriptorsList() { return meshManager->GetMeshPropertiesDescriptorsList(); }
	std::vector<VkDescriptorBufferInfo> GetMaterialBufferDescriptorList() { return materialManager->GetMaterialBufferDescriptorList(); }
	std::vector<VkDescriptorImageInfo>  GetTexture2DBufferDescriptorList() { return textureManager->GetTexture2DBufferDescriptorList(); }
	std::vector<VkDescriptorImageInfo>  GetTexture3DBufferDescriptorList() { return textureManager->GetTexture3DBufferDescriptorList(); }
	std::vector<VkDescriptorImageInfo>  GetCubeMapTextureBufferDescriptorList() { return textureManager->GetCubeMapTextureBufferDescriptorList(); }
	std::vector<VkDescriptorBufferInfo> GetDirectionalLightBufferDescriptorList() { return lightManager->GetDirectionalLightBufferDescriptorList(); }
	std::vector<VkDescriptorBufferInfo> GetPointLightBufferDescriptorList() { return lightManager->GetPointLightBufferDescriptorList(); }
	std::vector<VkDescriptorBufferInfo> GetSpotLightBufferDescriptorList() { return lightManager->GetSpotLightBufferDescriptorList(); }
	std::vector<VkDescriptorBufferInfo> GetSphereAreaLightDescriptorList() { return lightManager->GetSphereAreaLightDescriptorList(); }
	std::vector<VkDescriptorBufferInfo> GetTubeAreaLightDescriptorList() { return lightManager->GetTubeAreaLightDescriptorList(); }
	std::vector<VkDescriptorBufferInfo> GetRectangleAreaLightDescriptorList() { return lightManager->GetRectangleAreaLightDescriptorList(); }

	uint32_t GetMeshDescriptorCount() { return meshManager->GetMeshDescriptorCount(); }
	uint32_t GetMaterialDescriptorCount() { return materialManager->GetMaterialDescriptorCount(); }
	uint32_t GetTextureBufferDescriptorCount() { return textureManager->GetTextureBufferDescriptorCount(); }
	uint32_t Get3DTextureBufferDescriptorCount() { return textureManager->Get3DTextureBufferDescriptorCount(); }
	uint32_t GetCubeMapBufferDescriptorCount() { return textureManager->GetCubeMapBufferDescriptorCount(); }
	uint32_t GetDirectionalLightCount() { return lightManager->DirectionalLightList.size(); }
	uint32_t GetPointLightCount() { return lightManager->PointLightList.size(); }
	uint32_t GetSpotLightCount() { return lightManager->SpotLightList.size(); }
	uint32_t GetSphereAreaLightCount() { return lightManager->SphereAreaLightList.size(); }
	uint32_t GetTubeAreaLightCount() { return lightManager->TubeAreaLightList.size(); }
	uint32_t GetRectangleAreaLightCount() { return lightManager->RectangleAreaLightList.size(); }
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
