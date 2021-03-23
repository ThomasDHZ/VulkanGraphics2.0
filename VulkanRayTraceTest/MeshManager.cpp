#include "MeshManager.h"

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
}

void MeshManager::AddMesh(std::shared_ptr<Mesh> mesh)
{
	MeshList.emplace_back(mesh);
}

void MeshManager::Update(VulkanEngine& engine)
{
	for (auto& mesh : MeshList)
	{
		if (mesh->ParentMesh != 0)
		{
			mesh->Update(engine);
		}
	}
}

void MeshManager::UpdateMeshBufferIndex(VulkanEngine& engine)
{
	for (int x = 0; x < MeshList.size(); x++)
	{
		MeshList[x]->MeshBufferIndex = x;
	}
}

void MeshManager::Draw(VkCommandBuffer& commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline)
{
	for (auto& mesh : MeshList)
	{
		mesh->Draw(commandBuffer, pipeline);
	}
}
