#pragma once
#include "GUIObject.h"
#include "Font.h"
#include <map>

class TextMesh : public GUIObject
{
private:
	std::shared_ptr<Font> font;
	std::shared_ptr<Material> material;
	void BuildTextMesh(VulkanEngine& engine, MaterialManager& materialManager, TextureManager& textureManager, const std::string Text);
public:

	TextMesh();
	TextMesh(VulkanEngine& engine, MaterialManager& materialManager, TextureManager& textureManager, std::shared_ptr<Font> fontPTR, const std::string Text);
	~TextMesh();
};

