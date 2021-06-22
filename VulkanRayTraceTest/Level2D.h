#pragma once
#include "Mesh.h"

typedef glm::ivec2 Tile;
class Level2D : public Mesh
{
private:
	std::vector<uint32_t> IndexList;

protected:
	uint32_t TilesInTileSheet = 0;
	glm::ivec2 LevelBounds = glm::ivec2(0);
	glm::vec2 TileSize = glm::vec2(1.0f);
	glm::vec2 TileUVSize = glm::vec2(1.0f, 1.0f);
	std::vector<uint32_t> TileLevelLayout;
	glm::vec3 LightDirection = glm::vec3(1.0f);
	void LoadTiles(VulkanEngine& engine);
public:
	Level2D();
	Level2D(VulkanEngine& engine, uint32_t TilesInTileSheet, glm::ivec2 LevelBounds, glm::vec2 TileSize, glm::vec2 TileUVSize, std::vector<uint32_t> TileLevelLayout, glm::vec3 lightDirection);
	~Level2D();

	virtual void Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager, float timer) override;
};

