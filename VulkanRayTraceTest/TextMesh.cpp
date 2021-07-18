#include "TextMesh.h"

TextMesh::TextMesh() : Mesh()
{
}

TextMesh::TextMesh(VulkanEngine& engine, TextureManager& textureManager, std::shared_ptr<Font> fontPTR, const std::string Text) : Mesh()
{
	font = fontPTR;
	MeshID = engine.GenerateID();
	MeshProperties = MeshPropertiesUniformBuffer(engine);
	DrawFlags = MeshDrawFlags::Mesh_Draw_All;
	MeshMaterial = font->GetFontMaterial();

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::transpose(MeshTransform);

	BuildTextMesh(engine, Text);
	VertexCount = VertexList.size();
	IndexCount = IndexList.size();
	PrimitiveCount = static_cast<uint32_t>(IndexList.size()) / 3;

	BottomLevelAccelerationBuffer = AccelerationStructure(engine);
	SetUpMesh(engine, VertexList, IndexList);
}

TextMesh::~TextMesh()
{
}

void TextMesh::BuildTextMesh(VulkanEngine& engine, const std::string Text)
{
	float counter = 0;
	float StartPos = 0.0f;
	float EndPos = 0.0f;
	float scale = 1.0f;
	glm::vec2 LetterSize = glm::vec2(5.0f);
	for (std::string::const_iterator x = Text.begin(); x != Text.end(); x++)
	{
		Character ch = font->GetChar(*x);
		
		float xpos = StartPos + ch.Bearing.x * scale;
		float ypos = EndPos - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		VertexCount = VertexList.size();
		IndexCount = IndexList.size();

		const Vertex BottomLeftVertex =  { { xpos + w, ypos,     0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {1.0f,  0.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f } };
		const Vertex BottomRightVertex = { { xpos,     ypos,     0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.0f,  0.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f } };
		const Vertex TopRightVertex =    { { xpos,	   ypos + h, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {0.0f, -1.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f } };
		const Vertex TopLeftVertex =     { { xpos + w, ypos + h, 0.0f}, { 0.0f }, {0.0f, 0.0f, 1.0f}, { 0.0f }, {1.0f, -1.0f}, { 0.0f, 0.0f }, {-1.0f, 0.0f, 0.0f}, { 0.0f }, {0.0f, -1.0f, 0.0f}, { 0.0f } };

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

		counter++;
		StartPos += (ch.Advance >> 6) * scale;
	}
}
