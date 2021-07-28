#include "GUIObject.h"

GUIObject::GUIObject()
{
}

GUIObject::GUIObject(VulkanEngine& engine, glm::vec2 position)
{
	Position = position;
}

GUIObject::~GUIObject()
{
}

void GUIObject::AddGUIMesh(std::shared_ptr<GUIMesh> mesh)
{
	GuiMeshList.emplace_back(mesh);
}

void GUIObject::AddChildObject(std::shared_ptr<GUIObject> obj)
{
	ParentObject = this;
	GuiObjectList.emplace_back(obj);
}

void GUIObject::Update(VulkanEngine& engine, InputManager& inputManager)
{
	for (auto& mesh : GuiMeshList)
	{
		mesh->Update(engine, inputManager, Position);
	}
}

void GUIObject::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout)
{
	for (auto& mesh : GuiMeshList)
	{
		mesh->Draw(commandBuffer, layout);
	}
}

void GUIObject::Destory(VulkanEngine& engine)
{
	for (auto& mesh : GuiMeshList)
	{
		mesh->Destory(engine);
	}
}
