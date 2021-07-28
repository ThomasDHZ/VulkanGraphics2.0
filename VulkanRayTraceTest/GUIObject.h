#pragma once
#include "VulkanEngine.h"
#include "GUIMesh.h"

class GUIObject
{
private:
	std::vector<std::shared_ptr<GUIMesh>> GuiMeshList;
	std::vector<std::shared_ptr<GUIObject>> GuiObjectList;
	GUIObject* ParentObject;
protected:


public:
	glm::vec2 Position;

	GUIObject();
	GUIObject(VulkanEngine& engine, glm::vec2 position);
	~GUIObject();

	void AddGUIMesh(std::shared_ptr<GUIMesh> mesh);
	void AddChildObject(std::shared_ptr<GUIObject> obj);

	virtual void Update(VulkanEngine& engine, InputManager& inputManager);
	virtual void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	virtual void Destory(VulkanEngine& engine);
};

