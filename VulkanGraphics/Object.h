#pragma once
#include "BaseMesh.h"

class Object
{
private:
public:
	std::shared_ptr<BaseMesh> ObjectMesh;

	Object();
	Object(VulkanEngine& engine, std::shared_ptr<TextureManager>textureManager, std::vector<Vertex> vertexdata, std::vector<uint16_t> indicesdata, MeshTextures textures, int cubemap, VkDescriptorSetLayout& descriptorSetLayout, int renderBit);
	~Object();
};