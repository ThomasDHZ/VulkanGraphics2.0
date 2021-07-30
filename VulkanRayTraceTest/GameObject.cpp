#include "GameObject.h"
#include "Level2D.h"

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
	ModelList.emplace_back(std::make_shared<Model>(Model(engine, materialManager, textureManager, VertexList, IndexList, DrawFlags)));
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		meshManager.AddMesh(mesh);
	}
}

void GameObject::AddChildModel(VulkanEngine& engine, MeshManager& meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshDrawFlags DrawFlags)
{
	ModelList.emplace_back(std::make_shared<Model>(Model(engine, VertexList, IndexList, material, DrawFlags)));
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		meshManager.AddMesh(mesh);
	}
}

void GameObject::AddChildModel(VulkanEngine& engine, MeshManager& meshManager, MaterialManager& materiallManager, TextureManager& textureManager, const std::string& FilePath, MeshDrawFlags DrawFlags)
{
	ModelList.emplace_back(std::make_shared<Model>(Model(engine, materiallManager, textureManager, FilePath, DrawFlags)));
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		meshManager.AddMesh(mesh);
	}
}

void GameObject::Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager)
{
	for (auto& mesh : MeshList)
	{
		mesh->Update(engine, inputManager, materialManager);
	}
	for (auto& model : ModelList)
	{
		switch (model->ModelType)
		{
		case ModelTypeEnum::Model_Type_Normal: model->Update(engine, inputManager, materialManager, false); break;
		case ModelTypeEnum::Model_Type_2D_Level: static_cast<Level2D*>(model.get())->Update(engine, inputManager, materialManager); break;
		default: model->Update(engine, inputManager, materialManager, false);
		};
	}
}

void GameObject::Update(VulkanEngine& engine, const glm::mat4& ModelMatrix, const std::vector<std::shared_ptr<Bone>>& BoneList, InputManager& inputManager, MaterialManager& materialManager, bool RayTraceModeFlag)
{
	for (auto& mesh : MeshList)
	{
		mesh->Update(engine, ModelMatrix, BoneList, inputManager, materialManager, RayTraceModeFlag);
	}
}

void GameObject::SubmitAnimationToCommandBuffer(VulkanEngine& engine, std::vector<VkCommandBuffer>& CMDBufferList, int imageIndex)
{
	for (auto& model : ModelList)
	{
		model->SubmitAnimationToCommandBuffer(engine, CMDBufferList, imageIndex);
	}
}

void GameObject::Destory(VulkanEngine& engine)
{
	for (auto& mesh : MeshList)
	{
		mesh->Destory(engine);
	}
}