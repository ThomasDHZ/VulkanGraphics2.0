#include "RayTraceModel.h"

RayTraceModel::RayTraceModel()
{
}

RayTraceModel::RayTraceModel(VkDevice& device, VkPhysicalDevice& physicalDevice, MeshDetails meshContainer)
{
	MeshList.emplace_back(RayTraceMesh(device, physicalDevice, meshContainer, glm::mat4(1.0f)));
}

RayTraceModel::RayTraceModel(VkDevice& device, VkPhysicalDevice& physicalDevice, const std::string& FilePath)
{
	Assimp::Importer ModelImporter;

	const aiScene* Scene = ModelImporter.ReadFile(FilePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		std::cout << "Error loading model: " << ModelImporter.GetErrorString() << std::endl;
		return;
	}

	LoadMesh(device, physicalDevice, FilePath, Scene->mRootNode, Scene);

	for (auto& mesh : MeshInfoList)
	{
		MeshList.emplace_back(RayTraceMesh(device, physicalDevice, mesh, glm::mat4(1.0f)));
	}
}

RayTraceModel::~RayTraceModel()
{
}

void RayTraceModel::LoadMesh(VkDevice& device, VkPhysicalDevice& physicalDevice, const std::string& FilePath, aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		MeshDetails meshInfo = {};
		meshInfo.vertices = LoadVertices(mesh);
		meshInfo.indices = LoadIndices(mesh);
		MeshInfoList.emplace_back(meshInfo);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		LoadMesh(device, physicalDevice, FilePath, node->mChildren[i], scene);
	}
}

std::vector<RTVertex> RayTraceModel::LoadVertices(aiMesh* mesh)
{
	std::vector<RTVertex> VertexList;

	for (int x = 0; x < mesh->mNumVertices; x++)
	{
		RTVertex vertex;
		vertex.Position = glm::vec3{ mesh->mVertices[x].x, mesh->mVertices[x].y, mesh->mVertices[x].z };
		vertex.Normal = glm::vec3{ mesh->mNormals[x].x, mesh->mNormals[x].y, mesh->mNormals[x].z };

		if (mesh->mTangents)
		{
			vertex.Tangant = glm::vec4{ mesh->mTangents[x].x, mesh->mTangents[x].y, mesh->mTangents[x].z, 0.0f };
			vertex.BiTangant = glm::vec4{ mesh->mBitangents[x].x, mesh->mBitangents[x].y, mesh->mBitangents[x].z, 0.0f };
		}
		else
		{
			vertex.Tangant = glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f };
			vertex.BiTangant = glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f };
		}

		if (mesh->mTextureCoords[0])
		{
			vertex.TexureCoord = glm::vec2{ mesh->mTextureCoords[0][x].x, mesh->mTextureCoords[0][x].y };
		}
		else
		{
			vertex.TexureCoord = glm::vec2{ 0.0f, 0.0f };
		}

		vertex.BoneID = glm::vec4(0.0f);
		vertex.BoneWeights = glm::vec4(0.0f);

		VertexList.emplace_back(vertex);
	}

	return VertexList;
}

std::vector<uint32_t> RayTraceModel::LoadIndices(aiMesh* mesh)
{
	std::vector<uint32_t> IndexList;

	for (int x = 0; x < mesh->mNumFaces; x++)
	{
		aiFace Faces = mesh->mFaces[x];
		for (int y = 0; y < Faces.mNumIndices; y++)
		{
			IndexList.emplace_back(Faces.mIndices[y]);
		}
	}

	return IndexList;
}

void RayTraceModel::Update()
{
}
