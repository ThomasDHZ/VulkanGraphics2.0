#include "TextMesh.h"

TextMesh::TextMesh() : GUIMesh()
{
}

TextMesh::TextMesh(VulkanEngine& engine, TextureManager& textureManager, std::shared_ptr<Font> fontPTR, const std::string Text) : GUIMesh()
{
	font = fontPTR;
	BuildTextMesh(engine, Text);
	material = font->GetFontMaterial();

}

TextMesh::~TextMesh()
{
}

void TextMesh::BuildTextMesh(VulkanEngine& engine, const std::string Text)
{
	float counter = 0;
	float StartPos = -5.0f;
	float EndPos = 0.0f;
	float scale = 0.009f;
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

		const GUIVertex BottomLeftVertex =  { { xpos + w, ypos,    }, {1.0f,  0.0f} };
		const GUIVertex BottomRightVertex = { { xpos,     ypos,    }, {0.0f,  0.0f} };
		const GUIVertex TopRightVertex =    { { xpos,	  ypos + h }, {0.0f, -1.0f} };
		const GUIVertex TopLeftVertex =     { { xpos + w, ypos + h }, {1.0f, -1.0f} };

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

	VertexCount = VertexList.size();
	IndexCount = IndexList.size();

	VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, VertexList.size() * sizeof(GUIVertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexList.data());
	IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, IndexList.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, IndexList.data());

}
