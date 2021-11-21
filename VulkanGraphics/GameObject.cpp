#include "GameObject.h"
//#include "Level2D.h"

GameObject::GameObject() : Object()
{
}

GameObject::GameObject(std::shared_ptr<VulkanEngine> engine) : Object(ObjectType::Obj_GameObject)
{
}

GameObject::GameObject(glm::vec3 position) : Object(position, ObjectType::Obj_GameObject)
{
}

GameObject::~GameObject()
{
}

void GameObject::AddChildMesh(std::shared_ptr<Mesh> mesh)
{
	MeshList.emplace_back(mesh);
	MeshManagerPtr::GetMeshManagerPtr()->AddMesh(MeshList.back());
}

void GameObject::AddChildMesh(std::vector<Vertex>& VertexList)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(VertexList)));
	MeshManagerPtr::GetMeshManagerPtr()->AddMesh(MeshList.back());
}

void GameObject::AddChildMesh(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(VertexList, IndexList)));
	MeshManagerPtr::GetMeshManagerPtr()->AddMesh(MeshList.back());
}

void GameObject::AddChildMesh(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(VertexList, IndexList, material)));
	MeshManagerPtr::GetMeshManagerPtr()->AddMesh(MeshList.back());
}

void GameObject::AddChildModel(std::shared_ptr<Model> model)
{
	ModelList.emplace_back(model);
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		MeshManagerPtr::GetMeshManagerPtr()->AddMesh(mesh);
	}
}

void GameObject::AddChildModel(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList)
{
	ModelList.emplace_back(std::make_shared<Model>(Model(VertexList, IndexList)));
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		MeshManagerPtr::GetMeshManagerPtr()->AddMesh(mesh);
	}
}

void GameObject::AddChildModel(std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material)
{
	ModelList.emplace_back(std::make_shared<Model>(Model(VertexList, IndexList, material)));
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		MeshManagerPtr::GetMeshManagerPtr()->AddMesh(mesh);
	}
}

void GameObject::AddChildModel(const std::string& FilePath)
{
	ModelList.emplace_back(std::make_shared<Model>(Model(FilePath)));
	for (auto& mesh : ModelList.back()->MeshList)
	{
		MeshList.emplace_back(mesh);
		MeshManagerPtr::GetMeshManagerPtr()->AddMesh(mesh);
	}
}

void GameObject::Update()
{
	for (auto& mesh : MeshList)
	{
		mesh->Update();
	}
	for (auto& model : ModelList)
	{
		switch (model->ModelType)
		{
		case ModelTypeEnum::Model_Type_Normal: model->Update(false); break;
		//case ModelTypeEnum::Model_Type_2D_Level: static_cast<Level2D*>(model.get())->Update(); break;
		default: model->Update(false);
		};
	}
}

void GameObject::Update(const glm::mat4& ModelMatrix, const std::vector<std::shared_ptr<Bone>>& BoneList)
{
	for (auto& mesh : MeshList)
	{
		mesh->Update(ModelMatrix, BoneList);
	}
}

void GameObject::SubmitAnimationToCommandBuffer(std::vector<VkCommandBuffer>& CMDBufferList)
{
	for (auto& model : ModelList)
	{
		model->SubmitAnimationToCommandBuffer(CMDBufferList);
	}
}

void GameObject::Destory()
{
	for (auto& model : ModelList)
	{
		model->Destory();
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