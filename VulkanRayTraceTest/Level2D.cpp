#include "Level2D.h"

Level2D::Level2D() : Mesh()
{
}

Level2D::~Level2D()
{
}

void Level2D::LoadTiles(VulkanEngine& engine)
{
	for (unsigned int x = 0; x < LevelBounds.x - 1; x++)
	{
		for (unsigned int y = 0; y < LevelBounds.y - 1; y++)
		{
			const uint32_t LevelTile = TileLevelLayout[(y * LevelBounds.x) + x];
			const float LefttSideUV = LevelTile * TileUVSize.x;
			const float RightSideUV = (LevelTile * TileUVSize.x) + TileUVSize.x;

			const unsigned int VertexCount = VertexList.size();
			const Vertex BottomLeftVertex =  { {x,        y,        0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { RightSideUV,  0.0f         }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };
			const Vertex BottomRightVertex = { {x + 1.0f, y,        0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { LefttSideUV,  0.0f         }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };
			const Vertex TopRightVertex =    { {x + 1.0f, y + 1.0f, 0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { LefttSideUV, -TileUVSize.y }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };
			const Vertex TopLeftVertex =     { {x,        y + 1.0f, 0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { RightSideUV, -TileUVSize.y }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };

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

			std::vector<Vertex> CollisionVertices
			{
				BottomLeftVertex,
				BottomRightVertex,
				TopRightVertex,
				TopLeftVertex
			};

			std::vector<uint32_t> CollisionIndices
			{
				0, 1, 2, 2, 3, 0
			};
			
			TileList.emplace_back(std::make_shared<Tile>(Tile(CollisionVertices, CollisionIndices, TilePropertiesList[LevelTile])));
		}
	}
}
