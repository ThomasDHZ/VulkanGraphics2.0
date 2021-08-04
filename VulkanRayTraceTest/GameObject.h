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
	GameObject(std::shared_ptr<VulkanEngine> engine, glm::vec3 position);
	~GameObject();

	void AddChildMesh(std::shared_ptr<MeshManager> meshManager, std::shared_ptr<Mesh> mesh);
	void AddChildMesh(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, MeshDrawFlags MeshDrawFlags = Mesh_Draw_All);
	void AddChildMesh(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, MeshDrawFlags MeshDrawFlags = Mesh_Draw_All);
	void AddChildMesh(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshDrawFlags MeshDrawFlags = Mesh_Draw_All);
	void AddChildMesh(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshTypeFlag MeshType = Mesh_Type_Normal, MeshDrawFlags MeshDrawFlags = Mesh_Draw_All);
	void AddChildModel(std::shared_ptr<MeshManager> meshManager, std::shared_ptr<Model> model);
	void AddChildModel(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MeshManager> meshManager, std::shared_ptr<MaterialManager> materialManager, std::shared_ptr<TextureManager> textureManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, MeshDrawFlags DrawFlags = Mesh_Draw_All);
	void AddChildModel(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshDrawFlags DrawFlags = Mesh_Draw_All);
	void AddChildModel(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MeshManager> meshManager, std::shared_ptr<MaterialManager> materiallManager, std::shared_ptr<TextureManager> textureManager, const std::string& FilePath, MeshDrawFlags DrawFlags = Mesh_Draw_All);

	virtual void Update(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<InputManager> inputManager, std::shared_ptr<MaterialManager> materialManager) override;
	virtual void Update(std::shared_ptr<VulkanEngine> engine, const glm::mat4& ModelMatrix, const std::vector<std::shared_ptr<Bone>>& BoneList, std::shared_ptr<InputManager> inputManager, std::shared_ptr<MaterialManager> materialManager, bool RayTraceModeFlag);
	virtual void SubmitAnimationToCommandBuffer(std::shared_ptr<VulkanEngine> engine, std::vector<VkCommandBuffer>& CMDBufferList, int imageIndex);
	virtual void Destory(std::shared_ptr<VulkanEngine> engine);

	std::shared_ptr<Model> GetModel(uint32_t ModelID);
};