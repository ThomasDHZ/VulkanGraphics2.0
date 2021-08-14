#include "AnimatedTileMesh.h"

AnimatedTileMesh::AnimatedTileMesh() : Mesh()
{
}

AnimatedTileMesh::AnimatedTileMesh(std::vector<Vertex>& VertexList, std::vector<uint32_t> IndexList, std::shared_ptr<Material> material, std::shared_ptr<LevelTile> tilePtr) : Mesh(VertexList, IndexList, material, MeshTypeFlag::Mesh_Type_2D_Level_Animated_Tile, MeshDrawFlags::Mesh_Draw_All)
{
	tile = tilePtr;
}

AnimatedTileMesh::~AnimatedTileMesh()
{
}

void AnimatedTileMesh::Update(std::shared_ptr<Timer> timer)
{
	tile->Update(timer);
	UVOffset = tile->TileProperties->AnimationPlayer.GetFrame();
	Mesh::Update();
}
