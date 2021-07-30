#pragma once
#include "MeshManager.h"
#include "Object.h"
class GameObject : public Object
{
private:
	std::vector<std::shared_ptr<Model>> ModelList;
	std::vector<std::shared_ptr<Mesh>> MeshList;
protected:

public:
	GameObject();
	GameObject(VulkanEngine& engine);
	GameObject(VulkanEngine& engine, glm::vec3 position);
	~GameObject();

	void AddChildMesh(std::shared_ptr<MeshManager> meshManager, std::shared_ptr<Mesh> mesh);
	void AddChildMesh(VulkanEngine& engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, MeshDrawFlags MeshDrawFlags = Mesh_Draw_All);
	void AddChildMesh(VulkanEngine& engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, MeshDrawFlags MeshDrawFlags = Mesh_Draw_All);
	void AddChildMesh(VulkanEngine& engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshDrawFlags MeshDrawFlags = Mesh_Draw_All);
	void AddChildMesh(VulkanEngine& engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshTypeFlag MeshType = Mesh_Type_Normal, MeshDrawFlags MeshDrawFlags = Mesh_Draw_All);
	void AddChildModel(MeshManager&, std::shared_ptr<Model> model);
	void AddChildModel(VulkanEngine& engine, MeshManager& meshManager, MaterialManager& materialManager, TextureManager& textureManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, MeshDrawFlags DrawFlags = Mesh_Draw_All);
	void AddChildModel(VulkanEngine& engine, MeshManager& meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshDrawFlags DrawFlags = Mesh_Draw_All);
	void AddChildModel(VulkanEngine& engine, MeshManager& meshManager, MaterialManager& materiallManager, TextureManager& textureManager, const std::string& FilePath, MeshDrawFlags DrawFlags = Mesh_Draw_All);

	virtual void Update(VulkanEngine& engine, InputManager& inputManager) override;
	virtual void Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager);
	virtual void Update(VulkanEngine& engine, const glm::mat4& ModelMatrix, const std::vector<std::shared_ptr<Bone>>& BoneList, InputManager& inputManager, MaterialManager& materialManager, bool RayTraceModeFlag);
	virtual void Draw(VkCommandBuffer& commandBuffer) override;
	virtual void Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout, std::shared_ptr<Camera> CameraView) override;
	virtual void Destory(VulkanEngine& engine);
};