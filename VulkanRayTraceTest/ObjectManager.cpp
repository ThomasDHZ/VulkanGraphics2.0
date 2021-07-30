#include "ObjectManager.h"

ObjectManager::ObjectManager()
{
}

ObjectManager::ObjectManager(VulkanEngine& engine, MaterialManager& materialManager, TextureManager& textureManager)
{
}

ObjectManager::~ObjectManager()
{
}

void ObjectManager::Update(VulkanEngine& engine, InputManager& inputManager)
{
	for (auto& obj : ObjectList)
	{
		obj->Update(engine, inputManager);
	}
}

void ObjectManager::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout, std::shared_ptr<Camera> CameraView)
{
	for (auto& obj : ObjectList)
	{
		if (obj->ObjType == ObjectType::Obj_GameObject)
		{
			obj->Draw(commandBuffer, layout, CameraView);
		}
	}
}

void ObjectManager::GUIDraw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout)
{
	for (auto& obj : ObjectList)
	{
		if (obj->ObjType == ObjectType::Obj_GuiObject)
		{
			obj->Draw(commandBuffer, layout);
		}
	}
}

void ObjectManager::Destory(VulkanEngine& engine)
{
	for (auto& obj : ObjectList)
	{
		obj->Destory(engine);
	}
}
