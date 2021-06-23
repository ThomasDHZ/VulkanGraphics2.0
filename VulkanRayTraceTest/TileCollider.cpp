#include "TileCollider.h"

TileCollider::TileCollider()
{
}

TileCollider::TileCollider(const std::vector<Vertex>& vertexList, const std::vector<uint32_t>& indicesdata, glm::vec3 position)
{
	VertexList = vertexList;
	Indicesdata = indicesdata;

	const glm::vec3 BottomLeftVertex = position + VertexList[0].Position;
	const glm::vec3 BottomRightVertex = position + VertexList[1].Position;
	const glm::vec3 TopRightVertex = position + VertexList[2].Position;
	const glm::vec3 TopLeftVertex = position + VertexList[3].Position;

	Position = position;
	//Collider = BoxCollider(TopLeftVertex.x, TopRightVertex.x, TopRightVertex.y, BottomRightVertex.y);
	//Type = type;
}

TileCollider::~TileCollider()
{
}

void TileCollider::Update(glm::vec2 position)
{
	Position = glm::vec3(position, Position.z);
}

void TileCollider::Update(glm::vec3 position)
{
	Position = position;
}

bool TileCollider::CheckCollision(std::shared_ptr<TileCollider> otherCollider)
{
	const glm::vec3 BottomLeftVertex = Position + VertexList[0].Position;
	const glm::vec3 BottomRightVertex = Position + VertexList[1].Position;
	const glm::vec3 TopRightVertex = Position + VertexList[2].Position;
	const glm::vec3 TopLeftVertex = Position + VertexList[3].Position;
	const float Left = TopLeftVertex.x;
	const float Right = TopRightVertex.x;
	const float Top = TopRightVertex.y;
	const float Bottom = BottomRightVertex.y;

	const glm::vec3 OtherBottomLeftVertex = otherCollider->GetPosition() + otherCollider->GetVertex(0).Position;
	const glm::vec3 OtherBottomRightVertex = otherCollider->GetPosition() + otherCollider->GetVertex(1).Position;
	const glm::vec3 OtherTopRightVertex = otherCollider->GetPosition() + otherCollider->GetVertex(2).Position;
	const glm::vec3 OtherTopLeftVertex = otherCollider->GetPosition() + otherCollider->GetVertex(3).Position;
	const float OtherLeft = OtherTopLeftVertex.x;
	const float OtherRight = OtherTopRightVertex.x;
	const float OtherTop = OtherTopRightVertex.y;
	const float OtherBottom = OtherBottomRightVertex.y;

	return Right  > OtherLeft    &&
		   Left   < OtherRight   &&
		   Bottom < OtherTop     &&
		   Top    > OtherBottom;
}

bool TileCollider::CheckCollision(std::shared_ptr<TileCollider> collider1, std::shared_ptr<TileCollider> collider2)
{
	return true;
}
