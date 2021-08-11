#include "TerrainMesh.h"

TerrainMesh::TerrainMesh() : Mesh()
{
}

TerrainMesh::TerrainMesh(std::shared_ptr<VulkanEngine> engine, const std::string HeightMapLocation, std::shared_ptr<Material> material) : Mesh()
{

	MeshID = engine->GenerateID();
	MeshProperties = MeshPropertiesUniformBuffer(engine);
	DrawFlags = MeshDrawFlags::Mesh_Draw_All;
	MeshMaterial = material;

	MeshTransform = glm::mat4(1.0f);
	MeshTransform = glm::transpose(MeshTransform);

	BuildTerrainMesh(engine, HeightMapLocation);
	VertexCount = VertexList.size();
	IndexCount = IndexList.size();
	PrimitiveCount = static_cast<uint32_t>(IndexList.size()) / 3;
	
	BottomLevelAccelerationBuffer = AccelerationStructure(engine);
	SetUpMesh(engine, VertexList, IndexList);
}

TerrainMesh::~TerrainMesh()
{
}

void TerrainMesh::Destory(std::shared_ptr<VulkanEngine> engine)
{
	HeightMap.Delete();
	Mesh::Destory(engine);
}

void TerrainMesh::BuildTerrainMesh(std::shared_ptr<VulkanEngine> engine, const std::string HeightMapLocation)
{
	HeightMap = HeightMapTexture(engine, HeightMapLocation);
	const uint32_t Width = HeightMap.Width;
	const uint32_t Height = HeightMap.Height;

	const float uDiffrence = 1.0f / Width;
	const float vDiffrence = 1.0f / Height;

	VertexList.resize(Width*Height);

	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			const uint32_t index = x + (Width * y);
			VertexList[index].Position = glm::vec3(x, HeightMap.GetPixel(x, y).Red / 100.0f, y);

			VertexList[index].TexureCoord = glm::vec2(x * uDiffrence, y * vDiffrence);
		}
	}

	for (int x = 0; x < Width - 1; x++)
	{
		for (int y = 0; y < Height - 1; y++)
		{
			const uint32_t index = x + (Width * y);
			IndexList.emplace_back(index);
			IndexList.emplace_back(index + 1);
			IndexList.emplace_back(index + Width + 1);

			IndexList.emplace_back(index + Width + 1);
			IndexList.emplace_back(index + Width);
			IndexList.emplace_back(index);
		}
	}

	for (int x = 0; x < IndexList.size(); x += 3)
	{
		const glm::vec3 v1 = VertexList[IndexList[x]].Position;
		const glm::vec3 v2 = VertexList[IndexList[x + 1]].Position;
		const glm::vec3 v3 = VertexList[IndexList[x + 2]].Position;

		const glm::vec3 vu = v3 - v1;
		const glm::vec3 vt = v2 - v1;
		glm::vec3 normal = glm::cross(vu, vt);

		VertexList[IndexList[x]].Normal = normal;
		VertexList[IndexList[x + 1]].Normal = normal;
		VertexList[IndexList[x + 2]].Normal = normal;
	}

	for (int x = 0; x < VertexList.size(); x ++)
	{
		VertexList[IndexList[x]].Normal = glm::normalize(VertexList[IndexList[x]].Normal);
	}

	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			const uint32_t index = x + (Width * y);
			glm::vec3 v1 = VertexList[index].Position;

			if (x < Width - 1)
			{
				glm::vec3 v2 = VertexList[index + 1].Position;
				VertexList[index].Tangant = (v2 - v1);
			}
			else
			{
				glm::vec3 v2 = VertexList[index - 1].Position;
				VertexList[index].Tangant = (v1 - v2);
			}

			VertexList[index].Tangant = glm::normalize(VertexList[index].Tangant);
			VertexList[index].BiTangant = glm::cross(VertexList[index].Tangant, VertexList[index].Normal);
		}
	}
}