#include "GUIMesh.h"

GUIMesh::GUIMesh()
{
}

GUIMesh::GUIMesh(VulkanEngine& engine, std::vector<GUIVertex>& VertexList, std::vector<uint32_t>& IndexList)
{
	VertexCount = VertexList.size();
	IndexCount = IndexList.size();

	VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, VertexList.size() * sizeof(GUIVertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexList.data());
	IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, IndexList.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, IndexList.data());
}

GUIMesh::GUIMesh(VulkanEngine& engine, std::vector<GUIVertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> meshmaterial)
{
	VertexCount = VertexList.size();
	IndexCount = IndexList.size();

	VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, VertexList.size() * sizeof(GUIVertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexList.data());
	IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, IndexList.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, IndexList.data());

	material = meshmaterial;
}

GUIMesh::~GUIMesh()
{
}

void GUIMesh::Update(VulkanEngine& engine, InputManager& inputManager)
{
}

void GUIMesh::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout)
{
	GUIMeshProperties GUIInfo;
	GUIInfo.MaterialID = material->MaterialBufferIndex;
	GUIInfo.Color = Color;
	GUIInfo.Translation = Translation;
	GUIInfo.Scale = Scale;

	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &VertexBuffer.Buffer, offsets);
	vkCmdPushConstants(commandBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(GUIMeshProperties), &GUIInfo);
	vkCmdBindIndexBuffer(commandBuffer, IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(commandBuffer, IndexCount, 1, 0, 0, 0);
}

void GUIMesh::Destory(VulkanEngine& engine)
{
	VertexBuffer.DestoryBuffer(engine.Device);
	IndexBuffer.DestoryBuffer(engine.Device);
}