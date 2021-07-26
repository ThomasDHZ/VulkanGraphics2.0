#pragma once
#include "VulkanEngine.h"
#include "MaterialManager.h"
#include "Texture2D.h"
#include "Material.h"
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character
{
	std::shared_ptr<Texture2D> CharTexture;
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class Font
{
private:
	std::map<char, Character> Characters;

public:
	Font();
	Font(VulkanEngine& engine, MaterialManager& materialManager, TextureManager& textureManager, const std::string FontLocation);
	~Font();

	Character GetChar(uint32_t index) { return Characters[index]; }
};

