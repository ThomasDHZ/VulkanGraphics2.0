#pragma once
#include "Mesh.h"
#include "PerspectiveCamera.h"
#include "Animaton2D.h"
class BillboardMesh : public Mesh
{
private:

public:
	BillboardMesh();
	BillboardMesh(VulkanEngine& engine, glm::vec2 SpriteSize, glm::vec2 UVSize, glm::vec3 Position, uint32_t materialID);
	~BillboardMesh();

	Animator2D animator;
	virtual void Update(VulkanEngine& engine, MaterialManager& materialManager, std::shared_ptr<Camera> camera);
};

