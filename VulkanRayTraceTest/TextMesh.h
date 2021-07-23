#pragma once
#include "GUIMesh.h"
#include "Font.h"
#include <map>

class TextMesh : public GUIMesh
{
private:
	std::shared_ptr<Font> font;
	void BuildTextMesh(VulkanEngine& engine, const std::string Text);
public:

	TextMesh();
	TextMesh(VulkanEngine& engine, TextureManager& textureManager, std::shared_ptr<Font> fontPTR, const std::string Text);
	~TextMesh();
};

