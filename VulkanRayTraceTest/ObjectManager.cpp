#include "ObjectManager.h"
#include "GameObject.h"

std::shared_ptr<ObjectManager> ObjManagerPtr::ObjManager = nullptr;

ObjectManager::ObjectManager()
{
}

ObjectManager::ObjectManager(std::shared_ptr<VulkanEngine> Engine)
{
}

ObjectManager::~ObjectManager()
{
}

void ObjectManager::Update()
{
	for (auto& obj : ObjectList)
	{
		obj->Update();
	}
}

void ObjectManager::SubmitAnimationToCommandBuffer(std::vector<VkCommandBuffer>& CMDBufferList, int imageIndex)
{
	for (auto& obj : ObjectList)
	{
		if (obj->ObjType == ObjectType::Obj_GameObject)
		{
			static_cast<GameObject*>(obj.get())->SubmitAnimationToCommandBuffer(CMDBufferList, imageIndex);
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

void ObjectManager::Destory()
{
	for (auto& obj : ObjectList)
	{
		obj->Destory();
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
