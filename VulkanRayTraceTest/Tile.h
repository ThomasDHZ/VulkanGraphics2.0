#pragma once
#include "VulkanEngine.h"
#include "TileCollider.h"
#include "AnimationPlayer2D.h"

struct TileProperties
{
	uint32_t TileIndex;
	glm::vec2 TileOffset = glm::vec2(0.0f);
	bool Collidable = true;
	bool Climable = false;
	bool Animated = false;
	std::vector<std::shared_ptr<Animation2D>> AnimationList;
};

class Tile
{
private:
	std::vector<Vertex> VertexList;
	std::vector<uint32_t> IndexList;
	AnimationPlayer2D AnimationPlayer;
public:
	uint32_t TileIndex;
	glm::vec2 TileOffset = glm::vec2(0.0f);
	TileCollider Collider;
	bool Collidable = true;
	bool Climable = false;
	bool Animated = false;

	Tile();
	Tile(std::vector<Vertex>& vertexList, std::vector<uint32_t>& indexList, TileProperties& tileProperties);
	~Tile();
};