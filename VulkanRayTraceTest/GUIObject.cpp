#include "GUIObject.h"

GUIObject::GUIObject() : Object()
{
}

GUIObject::GUIObject(std::shared_ptr<VulkanEngine> engine) : Object(ObjectType::Obj_GuiObject)
{
}

GUIObject::GUIObject(glm::vec2 position) : Object(position, ObjectType::Obj_GuiObject)
{
}

GUIObject::~GUIObject()
{
}

void GUIObject::AddGUIMesh(std::shared_ptr<GUIMesh> mesh)
{
	GuiMeshList.emplace_back(mesh);
}

void GUIObject::Update()
{
	for (auto& mesh : GuiMeshList)
	{
		mesh->Update(glm::vec2(Position.x, Position.y));
	}
}

void GUIObject::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout)
{
	for (auto& mesh : GuiMeshList)
	{
		mesh->Draw(commandBuffer, layout);
	}
}

void GUIObject::Destory()
{
	for (auto& mesh : GuiMeshList)
	{
		mesh->Destory();
	}

}
