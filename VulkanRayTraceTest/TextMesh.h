#pragma once
#include "GUIMesh.h"
#include "Font.h"
#include <map>

class TextMesh : public GUIMesh
{
private:
	std::shared_ptr<Font> font;
	std::vector<GUIVertex> VertexList;
	std::vector<uint32_t> IndexList;
	void BuildTextMesh(VulkanEngine& engine, const std::string Text);
public:
	std::shared_ptr<Texture2D> FontTexture;
	TextMesh();
	TextMesh(VulkanEngine& engine, TextureManager& textureManager, std::shared_ptr<Font> fontPTR, const std::string Text);
	~TextMesh();
};

