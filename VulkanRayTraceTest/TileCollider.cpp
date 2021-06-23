#include "TileCollider.h"

TileCollider::TileCollider()
{
}

TileCollider::TileCollider(const std::vector<Vertex>& vertexList, const std::vector<uint32_t>& indicesdata)
{
	VertexList = vertexList;
	Indicesdata = indicesdata;
	Position = glm::vec3(0.0f);

	const glm::vec3 BottomLeftVertex = Position + VertexList[0].Position;
	const glm::vec3 BottomRightVertex = Position + VertexList[1].Position;
	const glm::vec3 TopRightVertex = Position + VertexList[2].Position;
	const glm::vec3 TopLeftVertex = Position + VertexList[3].Position;
	Left = TopLeftVertex.x;
	Right = TopRightVertex.x;
	Top = TopRightVertex.y;
	Bottom = BottomRightVertex.y;
}

TileCollider::TileCollider(const std::vector<Vertex>& vertexList, const std::vector<uint32_t>& indicesdata, glm::vec3 position)
{
	VertexList = vertexList;
	Indicesdata = indicesdata;
	Position = position;

	const glm::vec3 BottomLeftVertex = Position + VertexList[0].Position;
	const glm::vec3 BottomRightVertex = Position + VertexList[1].Position;
	const glm::vec3 TopRightVertex = Position + VertexList[2].Position;
	const glm::vec3 TopLeftVertex = Position + VertexList[3].Position;
	Left = TopLeftVertex.x;
	Right = TopRightVertex.x;
	Top = TopRightVertex.y;
	Bottom = BottomRightVertex.y;
}

TileCollider::~TileCollider()
{
}

void TileCollider::Update(glm::vec2 position)
{
	Position = glm::vec3(position, Position.z);

	const glm::vec3 BottomLeftVertex = Position + VertexList[0].Position;
	const glm::vec3 BottomRightVertex = Position + VertexList[1].Position;
	const glm::vec3 TopRightVertex = Position + VertexList[2].Position;
	const glm::vec3 TopLeftVertex = Position + VertexList[3].Position;
	Left = TopLeftVertex.x;
	Right = TopRightVertex.x;
	Top = TopRightVertex.y;
	Bottom = BottomRightVertex.y;
}

void TileCollider::Update(glm::vec3 position)
{
	Position = position;

	const glm::vec3 BottomLeftVertex = Position + VertexList[0].Position;
	const glm::vec3 BottomRightVertex = Position + VertexList[1].Position;
	const glm::vec3 TopRightVertex = Position + VertexList[2].Position;
	const glm::vec3 TopLeftVertex = Position + VertexList[3].Position;
	Left = TopLeftVertex.x;
	Right = TopRightVertex.x;
	Top = TopRightVertex.y;
	Bottom = BottomRightVertex.y;
}

bool TileCollider::CheckCollision(std::shared_ptr<TileCollider> otherCollider)
{
	const bool a = Right > otherCollider->Left;
	const bool b = Left < otherCollider->Right;
	const bool c = Bottom < otherCollider->Top;
	const bool d = Top > otherCollider->Bottom;
	return Right  > otherCollider->Left  &&
		   Left   < otherCollider->Right &&
		   Bottom < otherCollider->Top   &&
		   Top    > otherCollider->Bottom;
}

bool TileCollider::CheckCollision(std::shared_ptr<TileCollider> collider1, std::shared_ptr<TileCollider> collider2)
{
	return true;
}
