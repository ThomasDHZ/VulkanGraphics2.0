#pragma once
#include "GUIMesh.h"
#include "GUIObject.h"
#include "Font.h"
#include <map>

class TextMesh : public GUIObject
{
private:
	std::shared_ptr<Font> font;
	std::shared_ptr<Material> material;
	void BuildTextMesh(std::shared_ptr<VulkanEngine> engine, MaterialManager& materialManager, TextureManager& textureManager, const std::string Text);
public:

	TextMesh();
	TextMesh(std::shared_ptr<VulkanEngine> engine, MaterialManager& materialManager, TextureManager& textureManager, std::shared_ptr<Font> fontPTR, const std::string Text, glm::vec2 position);
	~TextMesh();
};

