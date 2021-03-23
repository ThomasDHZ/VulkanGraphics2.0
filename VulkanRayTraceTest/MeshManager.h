#pragma once
#include "Mesh.h"
#include <vector>
#include <memory>

class MeshManager
{
private: 
protected:

public:
	std::vector<std::shared_ptr<Mesh>> MeshList;

	MeshManager();
	~MeshManager();

	void AddMesh(std::shared_ptr<Mesh> mesh);
	void Update(VulkanEngine& engine);
	void Draw(VkCommandBuffer& commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline);
};

