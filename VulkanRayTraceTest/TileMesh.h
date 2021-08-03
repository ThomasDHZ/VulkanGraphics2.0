#pragma once
#include "Mesh.h"
class TileMesh : public Mesh
{
public:
	TileMesh();
	TileMesh(std::shared_ptr<VulkanEngine> engine, std::vector<Vertex>& VertexList, std::vector<uint32_t> IndexList, std::shared_ptr<Material> material);
	~TileMesh();

};

