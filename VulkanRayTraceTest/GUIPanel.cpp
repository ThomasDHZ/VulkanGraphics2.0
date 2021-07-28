#include "GUIPanel.h"

GUIPanel::GUIPanel() : GUIObject()
{

}

GUIPanel::GUIPanel(VulkanEngine& engine, std::shared_ptr<Material> meshmaterial, GuiRectangle rectangle, glm::vec2 position) : GUIObject(engine, position)
{
	std::vector<GUIVertex> VertexList = {
		{{rectangle.UpperLeftVertex.x, rectangle.UpperLeftVertex.y}, {1.0f, 0.0f}},
		{{rectangle.UpperRightVertex.x, rectangle.UpperRightVertex.y}, {0.0f, 0.0f}},
		{{rectangle.LowerRightVertex.x, rectangle.LowerRightVertex.y}, {0.0f, 1.0f}},
		{{rectangle.LowerLeftVertex.x, rectangle.LowerLeftVertex.y}, {1.0f, 1.0f}}
	};

	std::vector<uint32_t> IndexList = { 0, 1, 2, 2, 3, 0 };

	AddGUIMesh(std::make_shared<GUIMesh>(GUIMesh(engine, VertexList, IndexList, meshmaterial)));
}

GUIPanel::~GUIPanel()
{
}
