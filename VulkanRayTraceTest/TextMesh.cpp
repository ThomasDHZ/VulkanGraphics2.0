#include "TextMesh.h"
#include <ft2build.h>
#include FT_FREETYPE_H

TextMesh::TextMesh() : Mesh()
{
}

TextMesh::TextMesh(VulkanEngine& engine, TextureManager& textureManager, const std::string HeightMapLocation, std::shared_ptr<Material> material) : Mesh()
{
	MeshID = engine.GenerateID();
	MeshProperties = MeshPropertiesUniformBuffer(engine);
	DrawFlags = MeshDrawFlags::Mesh_Draw_All;
	

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::transpose(MeshTransform);

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		// return -1;
	}

	std::string font_name = "C:/Users/dotha/source/repos/VulkanGraphics/fonts/Antonio-Regular.ttf";
	FT_Face face;
	if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}
	else
	{
		FT_Set_Pixel_Sizes(face, 0, 48);

		for (unsigned char c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}

			if (c != 32)
			{
				uint32_t size = face->glyph->bitmap.width * face->glyph->bitmap.rows;
				byte* buff = static_cast<byte*>(face->glyph->bitmap.buffer);

				std::vector<Pixel> PixelList;
				for (int y = 0; y < face->glyph->bitmap.rows; y++)
				{
					for (int x = 0; x < face->glyph->bitmap.width; x++)
					{
						const uint32_t index = x + (face->glyph->bitmap.width * y);
						PixelList.emplace_back(Pixel(buff[index], buff[index], buff[index], 0xff));
					}
				}

				Character character =
				{
					//textureManager.LoadTextTexture(engine, face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows),
					textureManager.LoadTexture2D(engine, face->glyph->bitmap.width, face->glyph->bitmap.rows, PixelList, VK_FORMAT_R8G8B8A8_SRGB),
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<unsigned int>(face->glyph->advance.x)
				};
				Characters.insert(std::pair<char, Character>(c, character));
			
			}

		}
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	material->materialTexture.DiffuseMap = Characters[88].CharTexture;
	MeshMaterial = material;

	BuildTextMesh(engine, HeightMapLocation);
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
		Character ch = Characters[*x];
		
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
