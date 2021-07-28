#pragma once
#include "VulkanEngine.h"
#include "InputManager.h"
#include "Model.h"

class Object
{
private:
protected:
	Object* ParentObject;
	std::vector<std::shared_ptr<Model>> ModelList;
	std::vector<std::shared_ptr<Mesh>> MeshList;
	std::vector<std::shared_ptr<Object>> ObjectList;

public:
	glm::vec3 Position;

	Object();
	Object(VulkanEngine& engine, glm::vec2 position);
	Object(VulkanEngine& engine, glm::vec3 position);
	~Object();

	void AddChildModel(std::shared_ptr<Model> model);
	void AddChildMesh(std::shared_ptr<Mesh> mesh);
	void AddChildObject(std::shared_ptr<Object> obj);

	virtual void Update(VulkanEngine& engine, InputManager& inputManager);
	virtual void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout);
	virtual void Destory(VulkanEngine& engine);
};