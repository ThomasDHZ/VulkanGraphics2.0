#pragma once
#include "GUIObject.h"

class GUIPanel : public GUIObject
{
private:
protected:
public:
	GUIPanel();
	GUIPanel(VulkanEngine& engine, std::shared_ptr<Material> meshmaterial, glm::vec2 position);
	~GUIPanel();
};

