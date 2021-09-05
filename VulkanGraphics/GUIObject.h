#pragma once
#include "object.h"
#include "GUIMesh.h"
class GUIObject : public Object
{
private:
	std::vector<std::shared_ptr<GUIMesh>> GuiMeshList;

protected:


public:
	GUIObject();
	GUIObject(std::shared_ptr<VulkanEngine> engine);
	GUIObject(glm::vec2 position);
	~GUIObject();

	void AddGUIMesh(std::shared_ptr<GUIMesh> mesh);

	virtual void Update() override;
	virtual void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	virtual void Destory();
};

