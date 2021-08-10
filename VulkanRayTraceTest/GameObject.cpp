#include "GameObject.h"
#include "Level2D.h"

GameObject::GameObject() : Object()
{
}

GameObject::GameObject(std::shared_ptr<VulkanEngine> engine) : Object(engine, ObjectType::Obj_GameObject)
{
}

GameObject::GameObject(std::shared_ptr<VulkanEngine> engine, glm::vec3 position) : Object(engine, position, ObjectType::Obj_GameObject)
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

void GameObject::AddChildMesh(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, MeshDrawFlags MeshDrawFlags)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, VertexList, MeshDrawFlags)));
	meshManager->AddMesh(MeshList.back());
}

void GameObject::AddChildMesh(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, MeshDrawFlags MeshDrawFlags)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, VertexList, IndexList, MeshDrawFlags)));
	meshManager->AddMesh(MeshList.back());
}

void GameObject::AddChildMesh(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshDrawFlags MeshDrawFlags)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, VertexList, IndexList, material, MeshDrawFlags)));
	meshManager->AddMesh(MeshList.back());
}

void GameObject::AddChildMesh(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshTypeFlag MeshType, MeshDrawFlags MeshDrawFlags)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, VertexList, IndexList, material, MeshType, MeshDrawFlags)));
	meshManager->AddMesh(MeshList.back());
}

void GameObject::AddChildModel(std::shared_ptr<MeshManager> meshManager, std::shared_ptr<Model> model)
{
	ModelList.emplace_back(model);
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		meshManager->AddMesh(mesh);
	}
}

void GameObject::AddChildModel(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MeshManager> meshManager, std::shared_ptr<MaterialManager> materialManager, std::shared_ptr<TextureManager> textureManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, MeshDrawFlags DrawFlags)
{
	ModelList.emplace_back(std::make_shared<Model>(Model(engine, materialManager, textureManager, VertexList, IndexList, DrawFlags)));
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		meshManager->AddMesh(mesh);
	}
}

void GameObject::AddChildModel(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MeshManager> meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshDrawFlags DrawFlags)
{
	ModelList.emplace_back(std::make_shared<Model>(Model(engine, VertexList, IndexList, material, DrawFlags)));
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		meshManager->AddMesh(mesh);
	}
}

void GameObject::AddChildModel(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<MeshManager> meshManager, std::shared_ptr<MaterialManager> materiallManager, std::shared_ptr<TextureManager> textureManager, const std::string& FilePath, MeshDrawFlags DrawFlags)
{
	ModelList.emplace_back(std::make_shared<Model>(Model(engine, materiallManager, textureManager, FilePath, DrawFlags)));
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		meshManager->AddMesh(mesh);
	}
}

void GameObject::Update(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<InputManager> inputManager, std::shared_ptr<MaterialManager> materialManager)
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

void GameObject::Update(std::shared_ptr<VulkanEngine> engine, const glm::mat4& ModelMatrix, const std::vector<std::shared_ptr<Bone>>& BoneList, std::shared_ptr<InputManager> inputManager, std::shared_ptr<MaterialManager> materialManager, bool RayTraceModeFlag)
{
	for (auto& mesh : MeshList)
	{
		mesh->Update(engine, ModelMatrix, BoneList, inputManager, materialManager, RayTraceModeFlag);
	}
}

void GameObject::SubmitAnimationToCommandBuffer(std::shared_ptr<VulkanEngine> engine, std::vector<VkCommandBuffer>& CMDBufferList, int imageIndex)
{
	for (auto& model : ModelList)
	{
		model->SubmitAnimationToCommandBuffer(engine, CMDBufferList, imageIndex);
	}
}

void GameObject::Destory(std::shared_ptr<VulkanEngine> engine)
{
	for (auto& model : ModelList)
	{
		model->Destory(engine);
	}
}

std::shared_ptr<Model> GameObject::GetModel(uint32_t ModelID)
{
	std::shared_ptr<Model> modelptr = nullptr;
	for (auto& model : ModelList)
	{
		if (ModelID == model->ModelID)
		{
			modelptr = model;
		}
	}

	return modelptr;
}