#pragma once
#include "VulkanEngine.h"
#include "Object.h"

class ObjectManager
{
private:
public:
	std::vector<std::shared_ptr<Object>> ObjectList;

	ObjectManager();
	ObjectManager(VulkanEngine& engine, MaterialManager& materialManager, TextureManager& textureManager);
	~ObjectManager();

	void Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager);
	void SubmitAnimationToCommandBuffer(VulkanEngine& engine, std::vector<VkCommandBuffer>& CMDBufferList, int imageIndex);
	void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout, std::shared_ptr<Camera> CameraView);
	void GUIDraw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	void Destory(VulkanEngine& engine);
	std::shared_ptr<Model> GetModel(uint32_t ModelID);
};

