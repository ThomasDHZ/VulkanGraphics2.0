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

void GUIManager::Update(VulkanEngine& engine, InputManager& inputManager)
{
	for (auto& obj : GuiObjectList)
	{
		obj->Update(engine, inputManager);
	}
}

void GUIManager::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout)
{
	for (auto& obj : GuiObjectList)
	{
		obj->Draw(commandBuffer, layout);
	}
}

void GUIManager::Destory(VulkanEngine& engine)
{
	for (auto& obj : GuiObjectList)
	{
		obj->Destory(engine);
	}
}

void GUIManager::LoadFont(VulkanEngine& engine, MaterialManager& materialManager, TextureManager& textureManager, const std::string FontLocation)
{
	FontList.emplace_back(std::make_shared<Font>(Font(engine, materialManager, textureManager, FontLocation)));
}
