#pragma once
#include "Mesh.h"

class TileCollider
{
private:
	std::vector<Vertex> VertexList;
	std::vector<uint32_t> Indicesdata;
	glm::vec3 Position = glm::vec3(0.0f);
public:
	TileCollider();
	TileCollider(const std::vector<Vertex>& vertexList, const std::vector<uint32_t>& indicesdata);
	TileCollider(const std::vector<Vertex>& vertexList, const std::vector<uint32_t>& indicesdata, glm::vec3 position);
	~TileCollider();

	float Left = 0.0f;
	float Right = 0.0f;
	float Top = 0.0f;
	float Bottom = 0.0f;

	void Update(glm::vec2 position);
	void Update(glm::vec3 position);
	bool CheckCollision(std::shared_ptr<TileCollider> otherCollider);
	static bool CheckCollision(std::shared_ptr<TileCollider> collider1, std::shared_ptr<TileCollider> collider2);

	glm::vec3 GetPosition() { return Position; }
	Vertex GetVertex(uint32_t VertexIndex) { return VertexList[VertexIndex]; }
};

