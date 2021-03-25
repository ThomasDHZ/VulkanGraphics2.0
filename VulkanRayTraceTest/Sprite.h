#pragma once
#include "Mesh.h"

class Sprite : public Mesh
{
private: 
public:
	Sprite();
	Sprite(VulkanEngine& engine, glm::vec2 SpriteSize, glm::vec2 UVSize, glm::vec3 Position, uint32_t materialID);
	~Sprite();
};

