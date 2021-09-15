#pragma once
#include "Mesh.h"
class TileMesh : public Mesh
{
public:
	TileMesh();
	TileMesh(std::vector<Vertex>& VertexList, std::vector<uint32_t> IndexList, std::shared_ptr<Material> material);
	~TileMesh();

};

