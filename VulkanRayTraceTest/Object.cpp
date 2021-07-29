#include "Object.h"

Object::Object()
{
}

Object::Object(VulkanEngine& engine, ObjectType objType)
{
	ObjID = engine.GenerateObjID();
	ObjType = objType;
	glm::vec3(0.0f);
}

Object::Object(VulkanEngine& engine, glm::vec2 position, ObjectType objType)
{
	ObjID = engine.GenerateObjID();
	ObjType = objType;
	Position = glm::vec3(position.x, position.y, 0.0f);
}

Object::Object(VulkanEngine& engine, glm::vec3 position, ObjectType objType)
{
	ObjID = engine.GenerateObjID();
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