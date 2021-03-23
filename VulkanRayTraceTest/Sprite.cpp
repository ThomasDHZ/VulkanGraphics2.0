//#include "Sprite.h"
//
//Sprite::Sprite() : Mesh()
//{
//
//}
//
//Sprite::Sprite(VulkanEngine& engine, glm::vec2 SpriteSize, glm::vec3 Position, std::shared_ptr<Material> material) : Mesh()
//{
//    std::vector<Vertex> SpriteVertices =
//    {
//        {{ 0.0f,         0.0f,         0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {1.0f,  0.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
//        {{ SpriteSize.x, 0.0f,         0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.0f,  0.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
//        {{ SpriteSize.x, SpriteSize.y, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.0f, -1.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}},
//        {{ 0.0f,         SpriteSize.y, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {1.0f, -1.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f }, {0.0f, 0.0f, 0.0f, 1.0f}, {0, 0, 1, 0}, {0.0f, 0.0f, 1.0f, 0.0f}}
//    };
//
//    std::vector<uint32_t> SpriteIndices =
//    {
//        0, 1, 3,
//        1, 2, 3
//    };
//
//	//MeshIndex = meshID;
//	MeshProperties = MeshPropertiesUniformBuffer(engine);
//
//	MeshTransform = glm::mat4(1.0f);
//	MeshTransform = glm::transpose(MeshTransform);
//
//	VertexCount = SpriteVertices.size();
//	IndexCount = SpriteIndices.size();
//	PrimitiveCount = static_cast<uint32_t>(SpriteIndices.size()) / 3;
//
//	BottomLevelAccelerationBuffer = AccelerationStructure(engine);
//	SetUpMesh(engine, SpriteVertices, SpriteIndices);
//}
//
//Sprite::~Sprite()
//{
//
//}