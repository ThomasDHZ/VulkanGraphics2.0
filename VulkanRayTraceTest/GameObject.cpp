#include "GameObject.h"

GameObject::GameObject() : Object()
{
}

GameObject::GameObject(VulkanEngine& engine) : Object(engine, ObjectType::Obj_GuiObject)
{
}

GameObject::GameObject(VulkanEngine& engine, glm::vec3 position) : Object(engine, position, ObjectType::Obj_GuiObject)
{
}

GameObject::~GameObject()
{
}

void GameObject::AddChildMesh(std::shared_ptr<Mesh> mesh)
{
	MeshList.emplace_back(mesh);
}

void GameObject::AddChildMesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, MeshDrawFlags MeshDrawFlags)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, VertexList, MeshDrawFlags)));
}

void GameObject::AddChildMesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, MeshDrawFlags MeshDrawFlags)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, VertexList, IndexList, MeshDrawFlags)));
}

void GameObject::AddChildMesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshDrawFlags MeshDrawFlags)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, VertexList, IndexList, material, MeshDrawFlags)));
}

void GameObject::AddChildMesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshTypeFlag MeshType, MeshDrawFlags MeshDrawFlags)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, VertexList, IndexList, material, MeshType, MeshDrawFlags)));
}

void GameObject::AddChildModel(std::shared_ptr<Model> model)
{
	ModelList.emplace_back(model);
}

void GameObject::AddChildModel(VulkanEngine& engine, MeshManager& meshManager, MaterialManager& materialManager, TextureManager& textureManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, MeshDrawFlags DrawFlags)
{
	ModelList.emplace_back(std::make_shared<Model>(Model(engine, meshManager, materialManager, textureManager, VertexList, IndexList, DrawFlags)));
}

void GameObject::AddChildModel(VulkanEngine& engine, MeshManager& meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshDrawFlags DrawFlags)
{
	ModelList.emplace_back(std::make_shared<Model>(Model(engine, meshManager, VertexList, IndexList, material, DrawFlags)));
}

void GameObject::AddChildModel(VulkanEngine& engine, MeshManager& meshManager, MaterialManager& materiallManager, TextureManager& textureManager, const std::string& FilePath, MeshDrawFlags DrawFlags)
{
	ModelList.emplace_back(std::make_shared<Model>(Model(engine, meshManager, materiallManager, textureManager, FilePath, DrawFlags)));
}

void GameObject::Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager)
{
	for (auto& mesh : MeshList)
	{
		mesh->Update(engine, inputManager, materialManager);
	}
}

void GameObject::Update(VulkanEngine& engine, const glm::mat4& ModelMatrix, const std::vector<std::shared_ptr<Bone>>& BoneList, InputManager& inputManager, MaterialManager& materialManager, bool RayTraceModeFlag)
{
	for (auto& mesh : MeshList)
	{
		mesh->Update(engine, ModelMatrix, BoneList, inputManager, materialManager, RayTraceModeFlag);
	}
}

void GameObject::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout, RenderPassID RendererID, std::shared_ptr<Camera> CameraView)
{
	for (auto& mesh : MeshList)
	{
		mesh->Draw(commandBuffer, layout, RendererID, CameraView);
	}
}

void GameObject::Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, RenderPassID RendererID)
{
	for (auto& mesh : MeshList)
	{
		mesh->Draw(commandBuffer, renderPassInfo, RendererID);
	}
}

void GameObject::Destory(VulkanEngine& engine)
{
	for (auto& mesh : MeshList)
	{
		mesh->Destory(engine);
	}
}