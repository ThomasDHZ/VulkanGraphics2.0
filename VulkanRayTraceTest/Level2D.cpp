#include "Level2D.h"
#include "MegaMan.h"
#include "TileMesh.h"

Level2D::Level2D() : Model()
{
	timer2 = std::make_shared<Timer>(Timer());
}

Level2D::~Level2D()
{
}

void Level2D::AddSprite(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Sprite> sprite)
{
	MeshList.emplace_back(sprite);
}

void Level2D::AddTiles(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Material> material)
{
	std::vector<Vertex> VertexList;
	std::vector<uint32_t> IndexList;
	for (unsigned int x = 0; x < LevelBounds.x - 1; x++)
	{
		for (unsigned int y = 0; y < LevelBounds.y - 1; y++)
		{
			std::shared_ptr<Tile> tile = TileList[TileLevelLayout[(y * LevelBounds.x) + x]];
			const float LefttSideUV = tile->LevelUVPosition.x;
			const float RightSideUV = tile->LevelUVPosition.x + tile->TileUVSize.x;

			if(!tile->AnimationPlayer.AnimationExistFlag())
			{
				const unsigned int VertexCount = VertexList.size();
				const Vertex BottomLeftVertex =  { {x * TileSize.x,                 y * TileSize.y,               0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { RightSideUV,  0.0f         }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };
				const Vertex BottomRightVertex = { {(x * TileSize.x) + TileSize.x,  y * TileSize.y,               0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { LefttSideUV,  0.0f         }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };
				const Vertex TopRightVertex =    { {(x * TileSize.x) + TileSize.x, (y * TileSize.y) + TileSize.y, 0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { LefttSideUV, -TileUVSize.y }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };
				const Vertex TopLeftVertex =     { {x * TileSize.x,                (y * TileSize.y) + TileSize.y, 0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { RightSideUV, -TileUVSize.y }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };

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

				LevelTileLayout.emplace_back(std::make_shared<LevelTile>(LevelTile(tile, glm::vec3((float)x, (float)y, 0.0f), CollisionVertices, CollisionIndices)));
			}
		}
	}

	if (VertexList.size() != 0)
	{
		MeshList.emplace_back(std::make_shared<TileMesh>(TileMesh(engine, VertexList, IndexList, material)));
		MeshList.back()->ParentModelID = ModelID;
	}
}

void Level2D::AddAnimatedTile(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Material> material, uint32_t TileIndex)
{
	std::vector<Vertex> VertexList;
	std::vector<uint32_t> IndexList;
	for (unsigned int x = 0; x < LevelBounds.x - 1; x++)
	{
		for (unsigned int y = 0; y < LevelBounds.y - 1; y++)
		{
			std::shared_ptr<Tile> tile = TileList[TileLevelLayout[(y * LevelBounds.x) + x]];
			if (tile->TileIndex == TileIndex)
			{
				const float LefttSideUV = tile->LevelUVPosition.x;
				const float RightSideUV = tile->LevelUVPosition.x + tile->TileUVSize.x;
			
				const unsigned int VertexCount = VertexList.size();
				const Vertex BottomLeftVertex =  { {x * TileSize.x,                 y * TileSize.y,               0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { RightSideUV,  0.0f         }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };
				const Vertex BottomRightVertex = { {(x * TileSize.x) + TileSize.x,  y * TileSize.y,               0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { LefttSideUV,  0.0f         }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };
				const Vertex TopRightVertex =    { {(x * TileSize.x) + TileSize.x, (y * TileSize.y) + TileSize.y, 0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { LefttSideUV, -TileUVSize.y }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };
				const Vertex TopLeftVertex =     { {x * TileSize.x,                (y * TileSize.y) + TileSize.y, 0.0f}, {0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f}, { RightSideUV, -TileUVSize.y }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f} };

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

				LevelTileLayout.emplace_back(std::make_shared<LevelTile>(LevelTile(tile, glm::vec3((float)x, (float)y, 0.0f), CollisionVertices, CollisionIndices)));
			}
		}
	}
	if (VertexList.size() != 0)
	{
		assetManager->meshManager->AddMesh(std::make_shared<AnimatedTileMesh>(AnimatedTileMesh(engine, VertexList, IndexList, material, LevelTileLayout.back())));
		assetManager->meshManager->MeshList.back()->ParentModelID = ModelID;
		MeshList.emplace_back(assetManager->meshManager->MeshList.back());
	}
}

void Level2D::Update(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<InputManager> inputManager, std::shared_ptr<MaterialManager> materialManager)
{
	ModelTransform = glm::mat4(1.0f);
	ModelTransform = glm::translate(ModelTransform, ModelPosition);
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	ModelTransform = glm::scale(ModelTransform, ModelScale);

	for (auto& mesh : MeshList)
	{
		switch (mesh->MeshType)
		{
			case MeshTypeFlag::Mesh_Type_Normal:  mesh->Update(engine, inputManager, materialManager); break;
			case MeshTypeFlag::Mesh_Type_2D_Level_Tile:  mesh->Update(engine, inputManager, materialManager); break;
			case MeshTypeFlag::Mesh_Type_2D_Level_Animated_Tile:  static_cast<AnimatedTileMesh*>(mesh.get())->Update(engine, inputManager, materialManager, timer2); break;
			case MeshTypeFlag::Mesh_Type_2D_Sprite: static_cast<Sprite*>(mesh.get())->Update(engine, inputManager, materialManager, LevelTileLayout, MeshList); break;
			default: mesh->Update(engine, inputManager, materialManager);
		};
	}
}

void Level2D::LoadLevel(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Material> material)
{
	ModelID = engine->GenerateID();
	ModelType = ModelTypeEnum::Model_Type_2D_Level;

	AddTiles(engine, assetManager, material);

	const std::vector<uint32_t> UniqueAnimatedTileList = GetUniqueAnimatedTiles();
	for (auto& tileIndex : UniqueAnimatedTileList)
	{
		AddAnimatedTile(engine, assetManager, material, tileIndex);
	}
}

std::vector<uint32_t> Level2D::GetUniqueAnimatedTiles()
{
	std::vector<uint32_t> UniqueAnimatedTileList;

	std::vector<uint32_t> tempList = TileLevelLayout;
	std::sort(tempList.begin(), tempList.end());
	auto iterator = std::unique(tempList.begin(), tempList.end());
	tempList.erase(iterator, tempList.end());

	for (int x = 0; x < tempList.size(); x++)
	{
		const std::shared_ptr<Tile> tile = TileList[x];
		if (tile->AnimationPlayer.AnimationExistFlag())
		{
			UniqueAnimatedTileList.emplace_back(tempList[x]);
		}
	}

	return UniqueAnimatedTileList;
}
