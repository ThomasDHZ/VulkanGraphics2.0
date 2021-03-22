#include "MeshManager.h"

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
}

void MeshManager::Draw(VkCommandBuffer& commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline)
{
	for (auto& mesh : MeshList)
	{
		mesh->Draw(commandBuffer, pipeline);
	}
}
