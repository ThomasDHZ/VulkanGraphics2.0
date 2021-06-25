#include "Level2D.h"

Level2D::Level2D() : Mesh()
{
}

Level2D::Level2D(VulkanEngine& engine, uint32_t tilesInTileSheet, glm::ivec2 levelBounds, glm::vec2 tileSize, glm::vec2 tileUVSize, std::vector<uint32_t> tileLevelLayout, glm::vec3 lightDirection) : Mesh()
{
	TilesInTileSheet = tilesInTileSheet;
	LevelBounds = levelBounds;
	TileSize = tileSize;
	TileUVSize = tileUVSize;
	TileLevelLayout = tileLevelLayout;
	LightDirection = lightDirection;

	LoadTiles(engine);

    MeshID = engine.GenerateID();
	MeshType = MeshTypeFlag::Mesh_Type_2D_Level;
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
	for (unsigned int x = 0; x < LevelBounds.x - 1; x++)
	{
		for (unsigned int y = 0; y < LevelBounds.y - 1; y++)
		{
			const uint32_t LevelTile = TileLevelLayout[(y * LevelBounds.x) + x] + 1;
			const float LefttSideUV = (LevelTile - 1) * TileUVSize.x;
			const float RightSideUV = LevelTile * TileUVSize.x;

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

			const std::vector<Vertex> CollisionVertices
			{
				BottomLeftVertex,
				BottomRightVertex,
				TopRightVertex,
				TopLeftVertex
			};

			const std::vector<uint32_t> CollisionIndices
			{
				0, 1, 2, 2, 3, 0
			};
			
			TileList.emplace_back(std::make_shared<Tile>(Tile(CollisionVertices, CollisionIndices)));
		}
	}
}

//void Level2D::Update(VulkanEngine& engine, std::vector<std::shared_ptr<Mesh>> SpriteList)
//{
//	for(auto & mesh : SpriteList)
//	{
//		if (mesh->MeshType == MeshTypeFlag::Mesh_Type_2D_Sprite)
//		{
//			const auto sprite = static_cast<Sprite*>(mesh.get());
//			for (auto& tile : TileList)
//			{
//				if (sprite->tileCollider.CheckCollision(tile->Collider))
//				{
//					sprite->MeshPosition.x -= sprite->Velocity.x;
//					sprite->MeshPosition.y -= sprite->Velocity.y;
//					sprite->Velocity = glm::vec2(0.0f);
//				}
//			}
//		}
//	}
//}