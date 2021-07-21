#pragma once
#include "GUIMesh.h"

class GUIPanel : public GUIMesh
{
private:
protected:
public:
	GUIPanel();
	GUIPanel(VulkanEngine& engine, std::shared_ptr<Material> meshmaterial);
	~GUIPanel();
};

