#include "GUIManager.h"

GUIManager::GUIManager()
{
}

GUIManager::GUIManager(std::shared_ptr<VulkanEngine> Engine, std::shared_ptr<MaterialManager> materialmanager, std::shared_ptr<TextureManager> texturemanager, std::shared_ptr<InputManager> inputmanager)
{
	engine = Engine;
	materialManager = materialmanager;
	textureManager = texturemanager;
	inputManager = inputmanager;

	FontList.emplace_back(std::make_shared<Font>(Font(engine, materialManager, textureManager, "C:/Users/dotha/source/repos/VulkanGraphics/fonts/Antonio-Regular.ttf")));
}

GUIManager::~GUIManager()
{
}

void GUIManager::Update()
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

void GUIManager::Destory()
{
	for (auto& obj : GuiObjectList)
	{
		obj->Destory(engine);
	}
}

void GUIManager::LoadFont(const std::string FontLocation)
{
	FontList.emplace_back(std::make_shared<Font>(Font(engine, materialManager, textureManager, FontLocation)));
}
