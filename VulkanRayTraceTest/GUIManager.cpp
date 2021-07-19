#include "GUIManager.h"

GUIManager::GUIManager()
{
}

GUIManager::GUIManager(VulkanEngine& engine, MaterialManager& materialManager, TextureManager& textureManager)
{
	FontList.emplace_back(std::make_shared<Font>(Font(engine, materialManager, textureManager, "C:/Users/dotha/source/repos/VulkanGraphics/fonts/Antonio-Regular.ttf")));
}

GUIManager::~GUIManager()
{
}

void GUIManager::Update(VulkanEngine& engine)
{
}

void GUIManager::LoadFont(VulkanEngine& engine, MaterialManager& materialManager, TextureManager& textureManager, const std::string FontLocation)
{
	FontList.emplace_back(std::make_shared<Font>(Font(engine, materialManager, textureManager, FontLocation)));
}
