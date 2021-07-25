#pragma once
#include "VulkanEngine.h"
#include "Font.h"
#include "GUIMesh.h"
#include "GUIObject.h"

class GUIManager
{
private:
	

public:
	std::vector<std::shared_ptr<GUIMesh>> GuiMeshList;
	std::vector<std::shared_ptr<GUIObject>> GuiObjectList;
	std::vector<std::shared_ptr<Font>> FontList;

	GUIManager();
	GUIManager(VulkanEngine& engine, MaterialManager& materialManager, TextureManager& textureManager);
	~GUIManager();

	void Update(VulkanEngine& engine);
	void GUIDraw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, VkPipelineLayout layout);
	void LoadFont(VulkanEngine& engine, MaterialManager& materialManager, TextureManager& textureManager, const std::string FontLocation);
};

