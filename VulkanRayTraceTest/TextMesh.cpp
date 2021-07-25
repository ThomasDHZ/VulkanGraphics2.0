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
	std::vector<GUIVertex> VertexList;
	std::vector<uint32_t> IndexList;

	float counter = 0;
	float StartXPos = 0.0f;
	float StartYPos = 0.0f;
	float scale = 0.01f;
	for (std::string::const_iterator x = Text.begin(); x != Text.end(); x++)
	{
		const Character ch = font->GetChar(*x);

		float xpos = StartXPos + ch.Bearing.x * scale;
		float ypos = StartYPos - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		VertexCount = VertexList.size();
		IndexCount = IndexList.size();

		VertexList.emplace_back(GUIVertex({ { xpos + w, ypos - h  }, {font->GetChar(*x + 1).UVOffset.x, 0.0f} }));
		VertexList.emplace_back(GUIVertex({ { xpos,     ypos - h  }, {font->GetChar(*x    ).UVOffset.x, 0.0f} }));
		VertexList.emplace_back(GUIVertex({ { xpos,	    ypos    }, {font->GetChar(*x    ).UVOffset.x, 0.5f} }));
		VertexList.emplace_back(GUIVertex({ { xpos + w, ypos    }, {font->GetChar(*x + 1).UVOffset.x, 0.5f} }));

		IndexList.emplace_back(VertexCount);
		IndexList.emplace_back(VertexCount + 1);
		IndexList.emplace_back(VertexCount + 2);
		IndexList.emplace_back(VertexCount + 2);
		IndexList.emplace_back(VertexCount + 3);
		IndexList.emplace_back(VertexCount);

		StartXPos += ((ch.Advance >> 6)/2.5f) * scale;
		counter++;
	}

	VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, VertexList.size() * sizeof(GUIVertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, VertexList.data());
	IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, IndexList.size() * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, IndexList.data());

}
