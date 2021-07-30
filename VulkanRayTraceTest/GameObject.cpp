#include "GameObject.h"

GameObject::GameObject() : Object()
{
}

GameObject::GameObject(VulkanEngine& engine) : Object(engine, ObjectType::Obj_GameObject)
{
}

GameObject::GameObject(VulkanEngine& engine, glm::vec3 position) : Object(engine, position, ObjectType::Obj_GameObject)
{
}

GameObject::~GameObject()
{
}

void GameObject::AddChildMesh(std::shared_ptr<MeshManager> meshManager, std::shared_ptr<Mesh> mesh)
{
	MeshList.emplace_back(mesh);
	meshManager->AddMesh(MeshList.back());
}

void GameObject::AddChildMesh(VulkanEngine& engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, MeshDrawFlags MeshDrawFlags)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, VertexList, MeshDrawFlags)));
	meshManager->AddMesh(MeshList.back());
}

void GameObject::AddChildMesh(VulkanEngine& engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, MeshDrawFlags MeshDrawFlags)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, VertexList, IndexList, MeshDrawFlags)));
	meshManager->AddMesh(MeshList.back());
}

void GameObject::AddChildMesh(VulkanEngine& engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshDrawFlags MeshDrawFlags)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, VertexList, IndexList, material, MeshDrawFlags)));
	meshManager->AddMesh(MeshList.back());
}

void GameObject::AddChildMesh(VulkanEngine& engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshTypeFlag MeshType, MeshDrawFlags MeshDrawFlags)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, VertexList, IndexList, material, MeshType, MeshDrawFlags)));
	meshManager->AddMesh(MeshList.back());
}

void GameObject::AddChildModel(MeshManager& meshManager, std::shared_ptr<Model> model)
{
	ModelList.emplace_back(model);
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		meshManager.AddMesh(mesh);
	}
}

void GameObject::AddChildModel(VulkanEngine& engine, MeshManager& meshManager, MaterialManager& materialManager, TextureManager& textureManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, MeshDrawFlags DrawFlags)
{
	ModelList.emplace_back(std::make_shared<Model>(Model(engine, meshManager, materialManager, textureManager, VertexList, IndexList, DrawFlags)));
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		meshManager.AddMesh(mesh);
	}
}

void GameObject::AddChildModel(VulkanEngine& engine, MeshManager& meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshDrawFlags DrawFlags)
{
	ModelList.emplace_back(std::make_shared<Model>(Model(engine, meshManager, VertexList, IndexList, material, DrawFlags)));
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		meshManager.AddMesh(mesh);
	}
}

void GameObject::AddChildModel(VulkanEngine& engine, MeshManager& meshManager, MaterialManager& materiallManager, TextureManager& textureManager, const std::string& FilePath, MeshDrawFlags DrawFlags)
{
	ModelList.emplace_back(std::make_shared<Model>(Model(engine, meshManager, materiallManager, textureManager, FilePath, DrawFlags)));
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		meshManager.AddMesh(mesh);
	}
}

void GameObject::Update(VulkanEngine& engine, InputManager& inputManager)
{
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

void GameObject::Draw(VkCommandBuffer& commandBuffer)
{
	for (auto& mesh : MeshList)
	{
		mesh->Draw(commandBuffer);
	}
}

void GameObject::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout, std::shared_ptr<Camera> CameraView)
{

	for (auto& mesh : MeshList)
	{
		mesh->Draw(commandBuffer, layout, RenderPassID::Forward_Renderer, CameraView);
	}
}

void GameObject::Destory(VulkanEngine& engine)
{
	for (auto& mesh : MeshList)
	{
		mesh->Destory(engine);
	}
}