#pragma once
#include "GUIMesh.h"
#include "GUIObject.h"
#include "GuiRectangle.h"

class GUIPanel : public GUIObject
{
private:
protected:
public:
	GUIPanel();
	GUIPanel(VulkanEngine& engine, std::shared_ptr<Material> meshmaterial, GuiRectangle rectangle, glm::vec2 position);
	~GUIPanel();
};

