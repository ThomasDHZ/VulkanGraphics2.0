#include "ObjectManager.h"
#include "GameObject.h"

ObjectManager::ObjectManager()
{
}

ObjectManager::ObjectManager(VulkanEngine& engine, MaterialManager& materialManager, TextureManager& textureManager)
{
}

ObjectManager::~ObjectManager()
{
}

void ObjectManager::Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager)
{
	for (auto& obj : ObjectList)
	{
		obj->Update(engine, inputManager, materialManager);
	}
}

void ObjectManager::SubmitAnimationToCommandBuffer(VulkanEngine& engine, std::vector<VkCommandBuffer>& CMDBufferList, int imageIndex)
{
	for (auto& obj : ObjectList)
	{
		if (obj->ObjType == ObjectType::Obj_GameObject)
		{
			static_cast<GameObject*>(obj.get())->SubmitAnimationToCommandBuffer(engine, CMDBufferList, imageIndex);
		}
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
