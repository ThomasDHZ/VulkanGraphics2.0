#pragma once
#include "Mesh.h"
#include "LevelTile.h"
class AnimatedTileMesh : public Mesh
{
private:
	std::shared_ptr<LevelTile> tile;
public:
	AnimatedTileMesh();
	AnimatedTileMesh(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t> IndexList, std::shared_ptr<Material> material, std::shared_ptr<LevelTile> tilePtr);
	~AnimatedTileMesh();

	void Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager, std::shared_ptr<Timer> timer);
};

