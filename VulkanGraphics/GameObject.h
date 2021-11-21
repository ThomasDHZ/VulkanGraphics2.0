#pragma once
#include "MeshManager.h"
#include "Object.h"
class GameObject : public Object
{
private:
protected:

public:

	std::vector<std::shared_ptr<Model>> ModelList;
	std::vector<std::shared_ptr<Mesh>> MeshList;

	GameObject();
	GameObject(std::shared_ptr<VulkanEngine> engine);
	GameObject(glm::vec3 position);
	~GameObject();

	void AddChildMesh(std::shared_ptr<Mesh> mesh);
	void AddChildMesh(std::vector<Vertex>& VertexList);
	void AddChildMesh(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);
	void AddChildMesh(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material);
	void AddChildModel(std::shared_ptr<Model> model);
	void AddChildModel(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList);
	void AddChildModel(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material);
	void AddChildModel(const std::string& FilePath);

	virtual void Update() override;
	virtual void Update(const glm::mat4& ModelMatrix, const std::vector<std::shared_ptr<Bone>>& BoneList);
	virtual void SubmitAnimationToCommandBuffer(std::vector<VkCommandBuffer>& CMDBufferList);
	virtual void Destory();

	std::shared_ptr<Model> GetModel(uint32_t ModelID);
};