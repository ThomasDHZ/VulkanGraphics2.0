#include "ObjectManager.h"
#include "GameObject.h"

ObjectManager::ObjectManager()
{
}

ObjectManager::ObjectManager(std::shared_ptr<VulkanEngine> engine, MaterialManager& materialManager, TextureManager& textureManager)
{
}

ObjectManager::~ObjectManager()
{
}

void ObjectManager::Update(std::shared_ptr<VulkanEngine> engine, InputManager& inputManager, MaterialManager& materialManager)
{
	for (auto& obj : ObjectList)
	{
		obj->Update(engine, inputManager, materialManager);
	}
}

void ObjectManager::SubmitAnimationToCommandBuffer(std::shared_ptr<VulkanEngine> engine, std::vector<VkCommandBuffer>& CMDBufferList, int imageIndex)
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

void ObjectManager::Destory(std::shared_ptr<VulkanEngine> engine)
{
	for (auto& obj : ObjectList)
	{
		obj->Destory(engine);
	}
}

std::shared_ptr<Model> ObjectManager::GetModel(uint32_t ModelID)
{
	std::shared_ptr<Model> modelptr = nullptr;
	for (auto& obj : ObjectList)
	{
		if (obj->ObjType == ObjectType::Obj_GameObject)
		{
			modelptr = static_cast<GameObject*>(obj.get())->GetModel(ModelID);
			if (modelptr != nullptr)
			{
				break;
			}
		}
	}
	return modelptr;
}
