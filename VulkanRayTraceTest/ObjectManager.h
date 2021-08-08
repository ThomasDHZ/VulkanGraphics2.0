#pragma once
#include "Vulkanengine.h"
#include "Object.h"

class ObjectManager
{
private:
	std::shared_ptr<VulkanEngine> engine;

public:
	std::vector<std::shared_ptr<Object>> ObjectList;

	ObjectManager();
	ObjectManager(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MaterialManager> materialManager, std::shared_ptr<TextureManager> textureManager);
	~ObjectManager();

	void Update(std::shared_ptr<InputManager> inputManager, std::shared_ptr<MaterialManager> materialManager);
	void SubmitAnimationToCommandBuffer(std::vector<VkCommandBuffer>& CMDBufferList, int imageIndex);
	void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout, std::shared_ptr<Camera> CameraView);
	void GUIDraw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	void Destory();
	std::shared_ptr<Model> GetModel(uint32_t ModelID);
};

class ObjManagerPtr
{
private:
	static std::shared_ptr<ObjectManager> ObjManager;
public:
	static void SetUpPtr(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MaterialManager> materialManager, std::shared_ptr<InputManager> inputManager, std::shared_ptr<TextureManager> textureManager)
	{
		ObjManager = std::make_shared<ObjectManager>(ObjectManager(engine, materialManager, textureManager));
	}

	static std::shared_ptr<ObjectManager> GetObjManagerPtr()
	{
		return ObjManager;
	}
};