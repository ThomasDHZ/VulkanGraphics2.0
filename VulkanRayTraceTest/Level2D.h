#pragma once
#include "Mesh.h"
#include "TileCollider.h"

struct Tile
{
	std::vector<Vertex> VertexList;
	std::vector<uint32_t> IndexList;
	TileCollider Collider;

	Tile()
	{}

	Tile(std::vector<Vertex> vertexList, std::vector<uint32_t> indexList)
	{
		VertexList = vertexList;
		IndexList = indexList;
		Collider = TileCollider(vertexList, indexList);
	}
};
class Level2D : public Mesh
{
private:
	std::vector<uint32_t> IndexList;
	//std::vector<std::shared_ptr<TileCollider>> ColliderList;
protected:
	uint32_t TilesInTileSheet = 0;
	glm::ivec2 LevelBounds = glm::ivec2(0);
	glm::vec2 TileSize = glm::vec2(1.0f);
	glm::vec2 TileUVSize = glm::vec2(1.0f, 1.0f);
	std::vector<uint32_t> TileLevelLayout;
	glm::vec3 LightDirection = glm::vec3(1.0f);

	void LoadTiles(VulkanEngine& engine);
public:
	std::vector<std::shared_ptr<Tile>> TileList;
	Level2D();
	Level2D(VulkanEngine& engine, uint32_t TilesInTileSheet, glm::ivec2 LevelBounds, glm::vec2 TileSize, glm::vec2 TileUVSize, std::vector<uint32_t> TileLevelLayout, glm::vec3 lightDirection);
	~Level2D();

	/*void Update(VulkanEngine& engine, std::vector<std::shared_ptr<Mesh>> SpriteList);*/
};

