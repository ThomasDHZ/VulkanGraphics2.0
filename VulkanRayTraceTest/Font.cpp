#include "Font.h"

Font::Font()
{
}

Font::Font(VulkanEngine& engine, MaterialManager& materialManager, TextureManager& textureManager, const std::string FontLocation)
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "Could not load FreeType Library" << std::endl;
	}

	FT_Face face;
	if (FT_New_Face(ft, FontLocation.c_str(), 0, &face)) {
		std::cout << "Failed to load font" << std::endl;
	}
	else
	{
		FT_Set_Pixel_Sizes(face, 0, 48);

		for (unsigned char c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "Failed to load Glyph" << std::endl;
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
						const byte ByteColor = buff[index];
						PixelList.emplace_back(Pixel(ByteColor, ByteColor, ByteColor, ByteColor));
					}
				}

				Character character =
				{
					//textureManager.LoadTextTexture(engine, face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows),
					textureManager.LoadTexture2D(engine, face->glyph->bitmap.width, face->glyph->bitmap.rows, PixelList, VK_FORMAT_R8G8B8A8_SRGB),
					glm::vec2(0.0f),
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

	MaterialTexture material;
	material.DiffuseMap = Characters[83].CharTexture;

	FontMaterial = materialManager.LoadMaterial(engine, "FontMaterial", material);
}

Font::~Font()
{
}
