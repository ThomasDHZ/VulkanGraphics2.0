#pragma once
#include "Mesh.h"
#include "TextTexture.h"
#include <map>

struct Character {
	std::shared_ptr<Texture2D> CharTexture;
	glm::ivec2   Size;      // Size of glyph
	glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};


class TextMesh : public Mesh
{
private:
	std::map<char, Character> Characters;
	std::vector<uint32_t> IndexList;
	void BuildTextMesh(VulkanEngine& engine, const std::string Text);
public:
	std::shared_ptr<Texture2D> FontTexture;
	TextMesh();
	TextMesh(VulkanEngine& engine, TextureManager& textureManager, const std::string HeightMapLocation, std::shared_ptr<Material> material);
	~TextMesh();
};

