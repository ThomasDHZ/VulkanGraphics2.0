#include "Sprite.h"

Sprite::Sprite() : Mesh()
{

}

Sprite::Sprite(std::shared_ptr<VulkanEngine> engine, glm::vec2 spriteSize, glm::vec2 UVSize, glm::vec3 Position, std::shared_ptr<Material> material) : Mesh()
{
    SpriteSize = spriteSize;

    std::vector<Vertex> SpriteVertices =
    {
        {{ 0.0f,         0.0f,         0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {UVSize.x,  0.0f},     { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }},
        {{ SpriteSize.x, 0.0f,         0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.0f,      0.0f},     { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }},
        {{ SpriteSize.x, SpriteSize.y, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.0f,     -UVSize.y}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }},
        {{ 0.0f,         SpriteSize.y, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {UVSize.x, -UVSize.y}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }}
    };

    std::vector<uint32_t> SpriteIndices =
    {
        0, 1, 3,
        1, 2, 3
    };

    MeshID = engine->GenerateID();
    MeshType = MeshTypeFlag::Mesh_Type_2D_Sprite;
    MeshMaterial = material;
    AnimationPlayer = AnimationPlayer2D();

    MeshProperties = MeshPropertiesUniformBuffer(engine);
    ParentModelID = 0;
    VertexList = SpriteVertices;

    MeshPosition = Position;
    MeshTransform = glm::mat4(1.0f);
    MeshTransform = glm::translate(MeshTransform, MeshPosition);
    MeshTransform = glm::transpose(MeshTransform);

    VertexCount = SpriteVertices.size();
    IndexCount = SpriteIndices.size();
    PrimitiveCount = static_cast<uint32_t>(SpriteIndices.size()) / 3;

    tileCollider = TileCollider(SpriteVertices, SpriteIndices, Position);
    MeshTimer = std::make_shared<Timer>(Timer());

    BottomLevelAccelerationBuffer = AccelerationStructure(engine);
    SetUpMesh(engine, VertexList, SpriteIndices);
}

Sprite::~Sprite()
{

}

void Sprite::AddAnimation(std::shared_ptr<Animation2D> animation)
{
    AnimationPlayer.AddAnimation(animation);
}

void Sprite::AddAnimation(std::vector<std::shared_ptr<Animation2D>> animation)
{
    AnimationPlayer.AddAnimation(animation);
}

void Sprite::SetAnimation(uint32_t AnimationIndex)
{
    AnimationPlayer.SetAnimation(AnimationIndex);
}

void Sprite::Update(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<InputManager> inputManager, std::shared_ptr<MaterialManager> materialManager, std::vector<std::shared_ptr<LevelTile>> LevelTileLayout, std::vector<std::shared_ptr<Mesh>> MeshList)
{
    Velocity.y = -0.01f;

    if (FlipSpriteX)
    {
        UVFlip.x = 1.0f;
    }
    else
    {
        UVFlip.x = 0.0f;
    }
    AnimationPlayer.Update(FlipSpriteX);
    tileCollider.Update(MeshPosition);

    for(auto & mesh : MeshList)
	{
		if (mesh->MeshType == MeshTypeFlag::Mesh_Type_2D_Level_Tile ||
            mesh->MeshType == MeshTypeFlag::Mesh_Type_2D_Level_Animated_Tile)
		{
            for (auto& tile : LevelTileLayout)
            {
                if (tileCollider.CheckCollision(tile->Collider, Velocity) &&
                    tile->TileProperties->TilePropertiesFlags & TileProperties::TileColliable)
                {
                    if (tileCollider.CheckCollision(tile->Collider, Velocity) &&
                        Velocity.y < 0.0f)
                    {
                        Velocity.y = 0.0f;
                    }
                    else if (tileCollider.CheckCollision(tile->Collider, Velocity) &&
                        Velocity.y > 0.0f)
                    {
                        Velocity.y = 0.0f;
                    }

                    if (tileCollider.CheckCollision(tile->Collider, Velocity) &&
                        Velocity.x > 0.0f)
                    {
                        Velocity.x = 0.0f;
                    }
                    else if (tileCollider.CheckCollision(tile->Collider, Velocity) &&
                        Velocity.x < 0.0f)
                    {
                        Velocity.x = 0.0f;
                    }
                }
            }
		}
	}

    MeshPosition.x += Velocity.x;
    MeshPosition.y += Velocity.y;
    UVOffset = AnimationPlayer.GetFrame();
    Mesh::Update(engine, inputManager, materialManager);
}
