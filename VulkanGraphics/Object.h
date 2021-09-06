#pragma once
#include "Vulkanengine.h"
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
	uint32_t ObjID = 0;
	ObjectType ObjType;
	glm::vec3 Position;

	Object();
	Object(ObjectType objType);
	Object(glm::vec2 position, ObjectType objType);
	Object(glm::vec3 position, ObjectType objType);
	~Object();

	void AddChildObject(std::shared_ptr<Object> obj);

	virtual void Update() = 0;
	virtual void Draw(VkCommandBuffer& commandBuffer);
	virtual void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout& layout);
	virtual void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout& layout, std::shared_ptr<Camera> CameraView);
	virtual void Destory() = 0;
};