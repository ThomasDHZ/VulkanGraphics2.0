#pragma once
#include "VulkanEngine.h"
#include "GUIMesh.h"

class GUIObject
{
private:
	std::vector<std::shared_ptr<GUIMesh>> GuiMeshList;
protected:
	glm::vec2 Position;

public:
	GUIObject();
	GUIObject(VulkanEngine& engine, glm::vec2 position);
	~GUIObject();

	void AddGUIMesh(std::shared_ptr<GUIMesh> mesh);
	void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	void Destory(VulkanEngine& engine);

	uint32_t GetGUIMeshCount() { return GuiMeshList.size(); }
	std::shared_ptr<GUIMesh> GetGUIMesh(uint32_t index) { return GuiMeshList[index]; }
};

