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
	GUIManager(std::shared_ptr<VulkanEngine> engine);
	~GUIManager();

	void Update();
	void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	void Destory();

	void LoadFont(const std::string FontLocation);
};

class GuiManagerPtr
{
private:
	static std::shared_ptr<GUIManager> guiManager;
public:
	static void SetUpPtr(std::shared_ptr<VulkanEngine> engine)
	{
		if (guiManager == nullptr)
		{
			guiManager = std::make_shared<GUIManager>(GUIManager(engine));
		}
		else
		{
			std::cout << "Gui Manager has already been initialized." << std::endl;
		}
	}

	static std::shared_ptr<GUIManager> GetGuiManagerPtr()
	{
		return guiManager;
	}
};