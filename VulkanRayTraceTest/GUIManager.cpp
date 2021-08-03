#include "GUIManager.h"

GUIManager::GUIManager()
{
}

GUIManager::GUIManager(std::shared_ptr<VulkanEngine> engine, MaterialManager& materialManager, TextureManager& textureManager)
{
	FontList.emplace_back(std::make_shared<Font>(Font(engine, materialManager, textureManager, "C:/Users/dotha/source/repos/VulkanGraphics/fonts/Antonio-Regular.ttf")));
}

GUIManager::~GUIManager()
{
}

void GUIManager::Update(std::shared_ptr<VulkanEngine> engine, InputManager& inputManager, MaterialManager& materialManager)
{
	for (auto& obj : GuiObjectList)
	{
		obj->Update(engine, inputManager, materialManager);
	}
}

void GUIManager::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout)
{
	for (auto& obj : GuiObjectList)
	{
		obj->Draw(commandBuffer, layout);
	}
}

void GUIManager::Destory(std::shared_ptr<VulkanEngine> engine)
{
	for (auto& obj : GuiObjectList)
	{
		obj->Destory(engine);
	}
}

void GUIManager::LoadFont(std::shared_ptr<VulkanEngine> engine, MaterialManager& materialManager, TextureManager& textureManager, const std::string FontLocation)
{
	FontList.emplace_back(std::make_shared<Font>(Font(engine, materialManager, textureManager, FontLocation)));
}
