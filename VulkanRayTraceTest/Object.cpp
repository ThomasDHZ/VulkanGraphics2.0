#include "Object.h"

Object::Object()
{
}

Object::Object(VulkanEngine& engine, ObjectType objType)
{
	glm::vec3(0.0f);
	ObjType = objType;
}

Object::Object(VulkanEngine& engine, glm::vec2 position, ObjectType objType)
{
	Position = glm::vec3(position.x, position.y, 0.0f);
	ObjType = objType;
}

Object::Object(VulkanEngine& engine, glm::vec3 position, ObjectType objType)
{
	Position = position;
	ObjType = objType;
}

Object::~Object()
{
}

void Object::AddChildObject(std::shared_ptr<Object> obj)
{
	ParentObject = this;
	ObjectList.emplace_back(obj);
}