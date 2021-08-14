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
	ObjectManager(std::shared_ptr<VulkanEngine> engine);
	~ObjectManager();

	void Update();
	void SubmitAnimationToCommandBuffer(std::vector<VkCommandBuffer>& CMDBufferList);
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
	static void SetUpPtr(std::shared_ptr<VulkanEngine> engine)
	{
		if (ObjManager == nullptr)
		{
			ObjManager = std::make_shared<ObjectManager>(ObjectManager(engine));
		}
		else
		{
			std::cout << "Object Manager has already been initialized." << std::endl;
		}
	}

	static std::shared_ptr<ObjectManager> GetObjManagerPtr()
	{
		return ObjManager;
	}
};