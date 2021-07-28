#include "Object.h"

Object::Object()
{
}

Object::Object(VulkanEngine& engine, glm::vec2 position)
{
	Position = glm::vec3(position.x, position.y, 0.0f);
}

Object::Object(VulkanEngine& engine, glm::vec3 position)
{
	Position = position;
}

Object::~Object()
{
}

void Object::AddChildModel(std::shared_ptr<Model> model)
{
	ModelList.emplace_back(model);
}

void Object::AddChildMesh(std::shared_ptr<Mesh> mesh)
{
	MeshList.emplace_back(mesh);
}

void Object::AddChildObject(std::shared_ptr<Object> obj)
{
	ParentObject = this;
	ObjectList.emplace_back(obj);
}

void Object::Update(VulkanEngine& engine, InputManager& inputManager)
{
}

void Object::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout)
{

}

void Object::Destory(VulkanEngine& engine)
{
	for (auto& mesh : MeshList)
	{
		mesh->Destory(engine);
	}
}
