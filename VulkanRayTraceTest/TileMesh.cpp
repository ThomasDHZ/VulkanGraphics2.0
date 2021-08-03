#include "TileMesh.h"

TileMesh::TileMesh()
{
}

TileMesh::TileMesh(std::shared_ptr<VulkanEngine> engine, std::vector<Vertex>& VertexList, std::vector<uint32_t> IndexList, std::shared_ptr<Material> material) : Mesh(engine, VertexList, IndexList, material, MeshTypeFlag::Mesh_Type_2D_Level_Tile, MeshDrawFlags::Mesh_Draw_All)
{
}

TileMesh::~TileMesh()
{
}
