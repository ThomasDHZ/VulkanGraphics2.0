#pragma once
#include "object.h"

class GUIObject : public Object
{
private:
	std::vector<std::shared_ptr<GUIMesh>> GuiMeshList;

protected:


public:
	GUIObject();
	GUIObject(VulkanEngine& engine);
	GUIObject(VulkanEngine& engine, glm::vec2 position);
	~GUIObject();

	void AddGUIMesh(std::shared_ptr<GUIMesh> mesh);

	virtual void Update(VulkanEngine& engine, InputManager& inputManager);
	virtual void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	virtual void Destory(VulkanEngine& engine);
};

