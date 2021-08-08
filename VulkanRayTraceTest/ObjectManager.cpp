#include "ObjectManager.h"
#include "GameObject.h"

std::shared_ptr<ObjectManager> ObjManagerPtr::ObjManager = nullptr;

ObjectManager::ObjectManager()
{
}

ObjectManager::ObjectManager(std::shared_ptr<VulkanEngine> Engine, std::shared_ptr<MaterialManager> materialManager, std::shared_ptr<TextureManager> textureManager)
{
	engine = Engine;
}

ObjectManager::~ObjectManager()
{
}

void ObjectManager::Update(std::shared_ptr<InputManager> inputManager, std::shared_ptr<MaterialManager> materialManager)
{
	for (auto& obj : ObjectList)
	{
		obj->Update(engine, inputManager, materialManager);
	}
}

void ObjectManager::SubmitAnimationToCommandBuffer(std::vector<VkCommandBuffer>& CMDBufferList, int imageIndex)
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

void ObjectManager::Destory()
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
