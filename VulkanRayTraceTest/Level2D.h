#pragma once
#include "Mesh.h"
#include "Tile.h"

class Level2D : public Mesh
{
private:
protected:
	std::vector<uint32_t> IndexList;
	uint32_t TilesInTileSheet = 0;
	glm::ivec2 LevelBounds = glm::ivec2(0);
	glm::vec2 TileSize = glm::vec2(1.0f);
	glm::vec2 TileUVSize = glm::vec2(1.0f, 1.0f);
	std::vector<uint32_t> TileLevelLayout;
	glm::vec3 LightDirection = glm::vec3(1.0f);

	void LoadTiles(VulkanEngine& engine);
public:
	std::vector<std::shared_ptr<Tile>> TileList;
	std::vector<TileProperties> TilePropertiesList;
	Level2D();
	Level2D(VulkanEngine& engine, uint32_t TilesInTileSheet, glm::ivec2 LevelBounds, glm::vec2 TileSize, glm::vec2 TileUVSize, std::vector<uint32_t> TileLevelLayout, glm::vec3 lightDirection);
	~Level2D();
};

