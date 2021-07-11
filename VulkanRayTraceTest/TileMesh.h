#pragma once
#include "Mesh.h"
class TileMesh : public Mesh
{
public:
	TileMesh();
	TileMesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t> IndexList, std::shared_ptr<Material> material);
	~TileMesh();

};

