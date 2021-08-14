#include "GUIManager.h"

std::shared_ptr<GUIManager> GuiManagerPtr::guiManager = nullptr;

GUIManager::GUIManager()
{
}

GUIManager::GUIManager(std::shared_ptr<VulkanEngine> Engine)
{
	FontList.emplace_back(std::make_shared<Font>(Font(EnginePtr::GetEnginePtr(), MaterialManagerPtr::GetMaterialManagerPtr(), TextureManagerPtr::GetTextureManagerPtr(), "C:/Users/dotha/source/repos/VulkanGraphics/fonts/Antonio-Regular.ttf")));
}

GUIManager::~GUIManager()
{
}

void GUIManager::Update()
{
	for (auto& obj : GuiObjectList)
	{
		obj->Update(EnginePtr::GetEnginePtr(), InputManagerPtr::GetInputManagerPtr(), MaterialManagerPtr::GetMaterialManagerPtr());
	}
}

void GUIManager::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout)
{
	for (auto& obj : GuiObjectList)
	{
		obj->Draw(commandBuffer, layout);
	}
}

void GUIManager::Destory()
{
	for (auto& obj : GuiObjectList)
	{
		obj->Destory(EnginePtr::GetEnginePtr());
	}
}

void GUIManager::LoadFont(const std::string FontLocation)
{
	FontList.emplace_back(std::make_shared<Font>(Font(EnginePtr::GetEnginePtr(), MaterialManagerPtr::GetMaterialManagerPtr(), TextureManagerPtr::GetTextureManagerPtr(), FontLocation)));
}
