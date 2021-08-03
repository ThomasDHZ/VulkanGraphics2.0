#include "TextMesh.h"

TextMesh::TextMesh() : GUIObject()
{
}

TextMesh::TextMesh(std::shared_ptr<VulkanEngine> engine, MaterialManager& materialManager, TextureManager& textureManager, std::shared_ptr<Font> fontPTR, const std::string Text, glm::vec2 position) : GUIObject(engine, position)
{
	font = fontPTR;
	BuildTextMesh(engine, materialManager, textureManager, Text);
}

TextMesh::~TextMesh()
{
}

void TextMesh::BuildTextMesh(std::shared_ptr<VulkanEngine> engine, MaterialManager& materialManager, TextureManager& textureManager, const std::string Text)
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

		VertexList.emplace_back(GUIVertex({ { xpos + w, ypos - h }, {1.0f,  0.0f} }));
		VertexList.emplace_back(GUIVertex({ { xpos,     ypos - h }, {0.0f,  0.0f} }));
		VertexList.emplace_back(GUIVertex({ { xpos,	    ypos     }, {0.0f, 1.0f} }));
		VertexList.emplace_back(GUIVertex({ { xpos + w, ypos     }, {1.0f, 1.0f} }));

		IndexList.emplace_back(VertexCount);
		IndexList.emplace_back(VertexCount + 1);
		IndexList.emplace_back(VertexCount + 2);
		IndexList.emplace_back(VertexCount + 2);
		IndexList.emplace_back(VertexCount + 3);
		IndexList.emplace_back(VertexCount);

		StartXPos += (ch.Advance >> 6) * scale;
		counter++;

		MaterialTexture materialTexture;
		materialTexture.DiffuseMap = ch.CharTexture;

		material = materialManager.LoadMaterial(engine, "z", materialTexture);
		AddGUIMesh(std::make_shared<GUIMesh>(GUIMesh(engine, VertexList, IndexList, material)));
	}
}
