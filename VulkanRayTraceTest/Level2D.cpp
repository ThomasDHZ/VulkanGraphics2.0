#include "Level2D.h"

Level2D::Level2D() : Mesh()
{
}

Level2D::Level2D(VulkanEngine& engine, uint32_t tilesInTileSheet, glm::ivec2 levelBounds, glm::vec2 tileSize, glm::vec2 tileUVSize, std::vector<uint32_t> tileLevelLayout) : Mesh()
{
	TilesInTileSheet = tilesInTileSheet;
	LevelBounds = levelBounds;
	TileSize = tileSize;
	TileUVSize = tileUVSize;
	TileLevelLayout = tileLevelLayout;
	LoadTiles(engine);

    MeshID = engine.GenerateID();
    MeshMaterial = 0;

    MeshProperties = MeshPropertiesUniformBuffer(engine);
    ParentModelID = 0;

    MeshPosition = glm::vec3(0.0f);
    MeshTransform = glm::mat4(1.0f);
    MeshTransform = glm::translate(MeshTransform, MeshPosition);
    MeshTransform = glm::transpose(MeshTransform);

    VertexCount = VertexList.size();
    IndexCount = IndexList.size();
    PrimitiveCount = static_cast<uint32_t>(IndexList.size()) / 3;

    BottomLevelAccelerationBuffer = AccelerationStructure(engine);
    SetUpMesh(engine, VertexList, IndexList);
}

Level2D::~Level2D()
{
}

void Level2D::LoadTiles(VulkanEngine& engine)
{
	for (unsigned int x = 1; x < LevelBounds.x; x++)
	{
		for (unsigned int y = 1; y < LevelBounds.y; y++)
		{
			const unsigned int VertexCount = VertexList.size();
			const Vertex BottomLeftVertex =  { {x,        y,        0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { TileUVSize.x,  0.0f }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };
			const Vertex BottomRightVertex = { {x + 1.0f, y,        0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { 0.0f,          0.0f }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };
			const Vertex TopRightVertex =    { {x + 1.0f, y + 1.0f, 0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { 0.0f,         -TileUVSize.y }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };
			const Vertex TopLeftVertex =     { {x,        y + 1.0f, 0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { TileUVSize.x, -TileUVSize.y }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };

			VertexList.emplace_back(BottomLeftVertex);
			VertexList.emplace_back(BottomRightVertex);
			VertexList.emplace_back(TopRightVertex);
			VertexList.emplace_back(TopLeftVertex);

			IndexList.emplace_back(VertexCount);
			IndexList.emplace_back(VertexCount + 1);
			IndexList.emplace_back(VertexCount + 2);
			IndexList.emplace_back(VertexCount + 2);
			IndexList.emplace_back(VertexCount + 3);
			IndexList.emplace_back(VertexCount);

			const std::vector<Vertex> CollisionVertices
			{
				BottomLeftVertex,
				BottomRightVertex,
				TopRightVertex,
				TopLeftVertex
			};
			
		}
	}
}