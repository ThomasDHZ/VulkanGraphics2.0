#pragma once
#include "VulkanEngine.h"
#include "TileCollider.h"
#include "AnimationPlayer2D.h"

enum TileProperties
{
	TileNone = 1 << 0,
	TileColliable = 1 << 1,
	TileClimable = 1 << 2
};

class Tile
{
private:
protected:
public:
	uint32_t TileIndex;
	glm::ivec2 TileOffset = glm::ivec2(0);
	glm::vec2 TileSize = glm::vec2(1.0f);
	glm::vec2 TileUVSize = glm::vec2(0.0f);
	glm::vec2 LevelUVPosition = glm::vec2(0.0f);
	uint32_t TilesInSet = 0;
	int TilePropertiesFlags = 0;
	AnimationPlayer2D AnimationPlayer;

	Tile();
	Tile(uint32_t tileIndex, glm::ivec2 tileOffset, glm::vec2& tileSize, glm::vec2& tileUVSize, uint32_t tilesInSet, int tilePropertiesFlags);
	Tile(uint32_t tileIndex, std::shared_ptr<Animation2D> TileAnimation, glm::vec2& tileSize, glm::vec2& tileUVSize, uint32_t tilesInSet, int tilePropertiesFlags);
	~Tile();
};