#include "BillboardMesh.h"

BillboardMesh::BillboardMesh() : Mesh()
{
}

BillboardMesh::BillboardMesh(VulkanEngine& engine, glm::vec2 SpriteSize, glm::vec2 UVSize, glm::vec3 Position, std::shared_ptr<Material> material) : Mesh()
{
    std::vector<Vertex> SpriteVertices =
    {
        {{  SpriteSize.x / 2.0f,  SpriteSize.y / 2.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {-0.1f, -0.166f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
        {{  SpriteSize.x / 2.0f, -SpriteSize.y / 2.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {-0.1f,  0.0f  }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
        {{ -SpriteSize.x / 2.0f, -SpriteSize.y / 2.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.0f,   0.0f  }, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
        {{ -SpriteSize.x / 2.0f,  SpriteSize.y / 2.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.0f,  -0.166f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}}
    };

    std::vector<uint32_t> SpriteIndices =
    {
        0, 1, 3,
        1, 2, 3
    };

    MeshID = engine.GenerateID();
    MeshMaterial = material;
    MeshType = Mesh_Type_Billboard;

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

    animator = Animator2D(10, 6, glm::vec2(0.1f, -0.166f));
    BottomLevelAccelerationBuffer = AccelerationStructure(engine);
    SetUpMesh(engine, VertexList, SpriteIndices);
}

BillboardMesh::~BillboardMesh()
{
}

void BillboardMesh::Update(VulkanEngine& engine, MaterialManager& materialManager, std::shared_ptr<Camera> camera)
{
    MeshProperties.UniformDataInfo.MaterialBufferIndex = MeshMaterial->MaterialBufferIndex;

    MeshTransform = glm::mat4(1.0f);
    MeshTransform = glm::translate(MeshTransform, MeshPosition);
    MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    MeshTransform = glm::rotate(MeshTransform, glm::radians(MeshRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    MeshTransform = glm::scale(MeshTransform, MeshScale);

    MeshProperties.UniformDataInfo.ModelTransform = glm::mat4(1.0f);
    glm::mat4 FinalTransform = MeshTransform;
    glm::mat4 transformMatrix2 = glm::transpose(MeshTransform);

    VkTransformMatrixKHR transformMatrix = GLMToVkTransformMatrix(transformMatrix2);

    TransformBuffer.CopyBufferToMemory(engine.Device, &FinalTransform, sizeof(FinalTransform));
    TransformInverseBuffer.CopyBufferToMemory(engine.Device, &transformMatrix, sizeof(transformMatrix));

    MeshProperties.UniformDataInfo.UVOffset = animator.GetFrameUVs(animator.CurrentFrame);
    MeshProperties.Update(engine);


    if (IndexCount != 0)
    {
        MeshBottomLevelAccelerationStructure(engine);
    }
}
