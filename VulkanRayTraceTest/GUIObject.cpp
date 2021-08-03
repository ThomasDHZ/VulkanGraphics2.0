#include "GUIObject.h"

GUIObject::GUIObject() : Object()
{
}

GUIObject::GUIObject(std::shared_ptr<VulkanEngine> engine) : Object(engine, ObjectType::Obj_GuiObject)
{
}

GUIObject::GUIObject(std::shared_ptr<VulkanEngine> engine, glm::vec2 position) : Object(engine, position, ObjectType::Obj_GuiObject)
{
}

GUIObject::~GUIObject()
{
}

void GUIObject::AddGUIMesh(std::shared_ptr<GUIMesh> mesh)
{
	GuiMeshList.emplace_back(mesh);
}

void GUIObject::Update(std::shared_ptr<VulkanEngine> engine, InputManager& inputManager, MaterialManager& materialManager)
{
	for (auto& mesh : GuiMeshList)
	{
		mesh->Update(engine, inputManager, glm::vec2(Position.x, Position.y));
	}
}

void GUIObject::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout)
{
	for (auto& mesh : GuiMeshList)
	{
		mesh->Draw(commandBuffer, layout);
	}
}

void GUIObject::Destory(std::shared_ptr<VulkanEngine> engine)
{
	for (auto& mesh : GuiMeshList)
	{
		mesh->Destory(engine);
	}
}
