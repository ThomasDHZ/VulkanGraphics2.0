#pragma once
#include "VulkanEngine.h"
#include "Font.h"
class GUIManager
{
private:
public:
	std::vector<std::shared_ptr<Font>> FontList;

	GUIManager();
	GUIManager(VulkanEngine& engine, MaterialManager& materialManager, TextureManager& textureManager);
	~GUIManager();

	void Update(VulkanEngine& engine);
	void LoadFont(VulkanEngine& engine, MaterialManager& materialManager, TextureManager& textureManager, const std::string FontLocation);
};

