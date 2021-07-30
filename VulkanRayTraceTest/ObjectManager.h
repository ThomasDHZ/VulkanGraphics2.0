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

	void Update(VulkanEngine& engine, InputManager& inputManager);
	void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout, std::shared_ptr<Camera> CameraView);
	void GUIDraw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	void Destory(VulkanEngine& engine);
};

