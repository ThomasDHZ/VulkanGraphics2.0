#include "TextMesh.h"

TextMesh::TextMesh() : GUIObject()
{
}

TextMesh::TextMesh(VulkanEngine& engine, TextureManager& textureManager, std::shared_ptr<Font> fontPTR, const std::string Text) : GUIObject()
{
	font = fontPTR;
	BuildTextMesh(engine, Text);
}

TextMesh::~TextMesh()
{
}

void TextMesh::BuildTextMesh(VulkanEngine& engine, const std::string Text)
{
	float counter = 0;
	float StartXPos = 0.0f;
	float StartYPos = 0.0f;
	float scale = 0.01f;
	for (std::string::const_iterator x = Text.begin(); x != Text.end(); x++)
	{
		std::vector<GUIVertex> VertexList;
		std::vector<uint32_t> IndexList;

		const Character ch = font->GetChar(*x);

		float xpos = StartXPos + ch.Bearing.x * scale;
		float ypos = StartYPos - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		uint32_t VertexCount = VertexList.size();
		uint32_t IndexCount = IndexList.size();

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

		AddGUIMesh(std::make_shared<GUIMesh>(GUIMesh(engine, VertexList, IndexList, font->GetFontMaterial())));
	}
}
