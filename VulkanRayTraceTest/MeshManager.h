#pragma once
#include "Mesh.h"
#include <vector>
#include <memory>

class MeshManager
{
private: 
public:
	std::vector<std::shared_ptr<Mesh>> MeshList;

	MeshManager();
	~MeshManager();

	void Draw(VkCommandBuffer& commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline);
};

