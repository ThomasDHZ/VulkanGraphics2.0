#pragma once
#include "VulkanEngine.h"
#include "InputManager.h"
#include "Model.h"

enum ObjectType
{
	Obj_GameObject,
	Obj_GuiObject
};

class Object
{
private:
protected:
	Object* ParentObject;
	std::vector<std::shared_ptr<Object>> ObjectList;

public:
	glm::vec3 Position;
	ObjectType ObjType;

	Object();
	Object(VulkanEngine& engine, ObjectType objType);
	Object(VulkanEngine& engine, glm::vec2 position, ObjectType objType);
	Object(VulkanEngine& engine, glm::vec3 position, ObjectType objType);
	~Object();

	void AddChildObject(std::shared_ptr<Object> obj);

	virtual void Update(VulkanEngine& engine, InputManager& inputManager) = 0;
	virtual void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout) = 0;
	virtual void Destory(VulkanEngine& engine) = 0;
};