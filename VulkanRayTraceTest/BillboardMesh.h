#pragma once
#include "Mesh.h"
#include "PerspectiveCamera.h"
#include "Animaton2D.h"
class BillboardMesh : public Mesh
{
private:

public:
	BillboardMesh();
	BillboardMesh(glm::vec2 SpriteSize, glm::vec2 UVSize, glm::vec3 Position, std::shared_ptr<Material> material);
	~BillboardMesh();

	Animator2D animator;
	virtual void Update(std::shared_ptr<Camera> camera);
};

