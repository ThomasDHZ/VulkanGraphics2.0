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
	AnimationPlayer2D AnimationPlayer;
public:
	uint32_t TileIndex;
	glm::ivec2 TileOffset = glm::ivec2(0);
	glm::vec2 TileSize = glm::vec2(1.0f);
	glm::vec2 TileUVOffset = glm::vec2(0.0f);
	uint32_t TilesInSet = 0;
	int TilePropertiesFlags = 0;

	Tile();
	Tile(uint32_t tileIndex, glm::ivec2 tileOffset, glm::vec2& tileSize, uint32_t tilesInSet, int tilePropertiesFlags, std::shared_ptr<Animation2D> TileAnimation = nullptr);
	~Tile();
};