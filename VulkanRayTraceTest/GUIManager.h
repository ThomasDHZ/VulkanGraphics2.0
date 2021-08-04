#pragma once
#include "Vulkanengine.h"
#include "Font.h"
#include "GUIMesh.h"
#include "GUIObject.h"

class GUIManager
{
private:
	std::shared_ptr<VulkanEngine> engine;
	std::shared_ptr<MaterialManager> materialManager;
	std::shared_ptr<TextureManager> textureManager;
	std::shared_ptr<InputManager> inputManager;
public:
	std::vector<std::shared_ptr<GUIObject>> GuiObjectList;
	std::vector<std::shared_ptr<Font>> FontList;

	GUIManager();
	GUIManager(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MaterialManager> materialManager, std::shared_ptr<TextureManager> textureManager, std::shared_ptr<InputManager> inputManager);
	~GUIManager();

	void Update();
	void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	void Destory();

	void LoadFont(const std::string FontLocation);
};

