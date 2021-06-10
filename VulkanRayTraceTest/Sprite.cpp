#include "Sprite.h"

Sprite::Sprite() : Mesh()
{

}

Sprite::Sprite(VulkanEngine& engine, glm::vec2 SpriteSize, glm::vec2 UVSize, glm::vec3 Position, std::shared_ptr<Material> material) : Mesh()
{
    std::vector<Vertex> SpriteVertices =
    {
        {{ 0.0f,         0.0f,         0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {UVSize.x,  0.0f},     { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
        {{ SpriteSize.x, 0.0f,         0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.0f,      0.0f},     { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
        {{ SpriteSize.x, SpriteSize.y, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.0f,     -UVSize.y}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
        {{ 0.0f,         SpriteSize.y, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {UVSize.x, -UVSize.y}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}}
    };

    std::vector<uint32_t> SpriteIndices =
    {
        0, 1, 3,
        1, 2, 3
    };

    MeshID = engine.GenerateID();
    MeshMaterial = material;

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

void Sprite::Update(VulkanEngine& engine, MaterialManager& materialManager, float timer)
{
    AnimationPlayer.Update(timer);
    Mesh::Update(engine, materialManager, timer);
}
