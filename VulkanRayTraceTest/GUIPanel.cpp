#include "GUIPanel.h"

GUIPanel::GUIPanel() : GUIMesh()
{

}

GUIPanel::GUIPanel(VulkanEngine& engine, std::shared_ptr<Material> meshmaterial) : GUIMesh()
{
	std::vector<GUIVertex> VertexList = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f}}
	};

	std::vector<uint32_t> IndexList = { 0, 1, 2, 2, 3, 0 };

	VertexCount = VertexList.size();
	IndexCount = IndexList.size();

	VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, VertexList.size() * sizeof(GUIVertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexList.data());
	IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, IndexList.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, IndexList.data());

	material = meshmaterial;
}

GUIPanel::~GUIPanel()
{
}
