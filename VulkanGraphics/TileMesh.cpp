#include "TileMesh.h"

TileMesh::TileMesh()
{
}

TileMesh::TileMesh(std::vector<Vertex>& VertexList, std::vector<uint32_t> IndexList, std::shared_ptr<Material> material) : Mesh(VertexList, IndexList, material)
{
}

TileMesh::~TileMesh()
{
}
