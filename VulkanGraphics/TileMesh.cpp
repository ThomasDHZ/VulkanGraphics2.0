#include "TileMesh.h"

TileMesh::TileMesh()
{
}

TileMesh::TileMesh(std::vector<Vertex>& VertexList, std::vector<uint32_t> IndexList, std::shared_ptr<Material> material) : Mesh(VertexList, IndexList, material, MeshTypeFlag::Mesh_Type_2D_Level_Tile, MeshDrawFlags::Mesh_Draw_All)
{
}

TileMesh::~TileMesh()
{
}
