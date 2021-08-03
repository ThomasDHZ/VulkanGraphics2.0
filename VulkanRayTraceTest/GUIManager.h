#pragma once
#include "Vulkanengine.h"
#include "Font.h"
#include "GUIMesh.h"
#include "GUIObject.h"

class GUIManager
{
private:
	

public:
	std::vector<std::shared_ptr<GUIObject>> GuiObjectList;
	std::vector<std::shared_ptr<Font>> FontList;

	GUIManager();
	GUIManager(std::shared_ptr<VulkanEngine> engine, MaterialManager& materialManager, TextureManager& textureManager);
	~GUIManager();

	void Update(std::shared_ptr<VulkanEngine> engine, InputManager& inputManager, MaterialManager& materialManager);
	void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	void Destory(std::shared_ptr<VulkanEngine> engine);

	void LoadFont(std::shared_ptr<VulkanEngine> engine, MaterialManager& materialManager, TextureManager& textureManager, const std::string FontLocation);
};

