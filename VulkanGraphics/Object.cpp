#include "Object.h"

Object::Object()
{
}

Object::Object(ObjectType objType)
{
	ObjID = EnginePtr::GetEnginePtr()->GenerateObjID();
	ObjType = objType;
	glm::vec3(0.0f);
}

Object::Object(glm::vec2 position, ObjectType objType)
{
	ObjID = EnginePtr::GetEnginePtr()->GenerateObjID();
	ObjType = objType;
	Position = glm::vec3(position.x, position.y, 0.0f);
}

Object::Object(glm::vec3 position, ObjectType objType)
{
	ObjID = EnginePtr::GetEnginePtr()->GenerateObjID();
	ObjType = objType;
	Position = position;
}

Object::~Object()
{
}

void Object::AddChildObject(std::shared_ptr<Object> obj)
{
	ParentObject = this;
	ObjectList.emplace_back(obj);
}

void Object::Draw(VkCommandBuffer& commandBuffer)
{
	//Program shouldn't hit this line.
	assert(1 != 1);
}

void Object::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout& layout)
{
	//Program shouldn't hit this line.
	assert(1 != 1);
}

void Object::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout& layout, std::shared_ptr<Camera> CameraView)
{
	//Program shouldn't hit this line.
	assert(1 != 1);
}
