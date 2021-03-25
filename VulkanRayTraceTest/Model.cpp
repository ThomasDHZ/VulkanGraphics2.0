#include "Model.h"
#include <chrono>


Model::Model()
{
}

Model::Model(VulkanEngine& engine, MeshManager& meshManager, TextureManager& textureManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList)
{
	ModelID = engine.GenerateID();
	meshManager.AddMesh(std::make_shared<Mesh>(engine, VertexList, IndexList, 0));
	meshManager.MeshList.back()->ParentModelID = ModelID;
	meshManager.MeshList.back()->VertexList = VertexList;
	meshManager.MeshList.back()->MeshTransform = glm::mat4(1.0f);
	MeshList.emplace_back(meshManager.MeshList.back());
}

Model::Model(VulkanEngine& engine, MeshManager& meshManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, uint32_t materialID)
{
	ModelID = engine.GenerateID();
	meshManager.AddMesh(std::make_shared<Mesh>(engine, VertexList, IndexList, materialID));
	meshManager.MeshList.back()->ParentModelID = ModelID;
	meshManager.MeshList.back()->VertexList = VertexList;
	meshManager.MeshList.back()->MeshTransform = glm::mat4(1.0f);
	MeshList.emplace_back(meshManager.MeshList.back());
}

Model::Model(VulkanEngine& engine, MeshManager& meshManager, MaterialManager& materiallManager, TextureManager& textureManager, const std::string& FilePath)
{
	ModelID = engine.GenerateID();
	Assimp::Importer ModelImporter;

	const aiScene* Scene = ModelImporter.ReadFile(FilePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		std::cout << "Error loading model: " << ModelImporter.GetErrorString() << std::endl;
		return;
	}

	GlobalInverseTransformMatrix = AssimpToGLMMatrixConverter(Scene->mRootNode->mTransformation.Inverse());
	LoadNodeTree(Scene->mRootNode);
	LoadAnimations(Scene);
	LoadMesh(engine, meshManager, materiallManager, textureManager, FilePath, Scene->mRootNode, Scene);

	LoadMeshTransform(0, ModelTransform);

	if (AnimationList.size() > 0)
	{
		AnimationPlayer = AnimationPlayer3D(BoneList, NodeMapList, GlobalInverseTransformMatrix, AnimationList[0]);
	}

	ModelTransform = AssimpToGLMMatrixConverter(Scene->mRootNode->mTransformation.Inverse());
}

Model::~Model()
{
}

void Model::LoadNodeTree(const aiNode* Node, int parentNodeID)
{
	NodeMap node;
	node.NodeID = NodeMapList.size();
	node.ParentNodeID = parentNodeID;
	node.NodeString = Node->mName.C_Str();
	node.NodeTransform = Node->mTransformation;
	if (parentNodeID != -1)
	{
		NodeMapList[parentNodeID].ChildNodeList.emplace_back(node.NodeID);
	}
	NodeMapList.emplace_back(node);

	for (int x = 0; x < Node->mNumChildren; x++)
	{
		LoadNodeTree(Node->mChildren[x], node.NodeID);
	}
}

void Model::LoadAnimations(const aiScene* scene)
{
	for (int x = 0; x < scene->mNumAnimations; x++)
	{
		aiAnimation* assImpAnimation = scene->mAnimations[x];

		Animation3D animation = Animation3D();
		animation.TicksPerSec = assImpAnimation->mTicksPerSecond;
		animation.AnimationTime = assImpAnimation->mDuration * animation.TicksPerSec;

		for (int y = 0; y < assImpAnimation->mNumChannels; y++)
		{
			KeyFrame keyframe;
			aiNodeAnim* channel = assImpAnimation->mChannels[y];

			for (auto bone : BoneList)
			{
				if (channel->mNodeName.C_Str() == bone->BoneName)
				{
					keyframe.BoneName = channel->mNodeName.C_Str();
					keyframe.BoneId = bone->BoneID;
					break;
				}
			}

			for (int z = 0; z < channel->mNumPositionKeys; z++)
			{
				KeyFrameInfo PosKeyFrame;
				PosKeyFrame.Time = channel->mPositionKeys[z].mTime;
				PosKeyFrame.AnimationInfo = aiVector3D(channel->mPositionKeys[z].mValue.x, channel->mPositionKeys[z].mValue.y, channel->mPositionKeys[z].mValue.z);
				keyframe.BonePosition.emplace_back(PosKeyFrame);
			}

			for (int z = 0; z < channel->mNumRotationKeys; z++)
			{
				KeyFrameRotationInfo RotKeyFrame;
				RotKeyFrame.Time = channel->mRotationKeys[z].mTime;
				RotKeyFrame.AnimationInfo = aiQuaternion(channel->mRotationKeys[z].mValue.w, channel->mRotationKeys[z].mValue.x, channel->mRotationKeys[z].mValue.y, channel->mRotationKeys[z].mValue.z);
				keyframe.BoneRotation.emplace_back(RotKeyFrame);
			}

			for (int z = 0; z < channel->mNumScalingKeys; z++)
			{
				KeyFrameInfo ScaleKeyFrame;
				ScaleKeyFrame.Time = channel->mScalingKeys[z].mTime;
				ScaleKeyFrame.AnimationInfo = aiVector3D(channel->mScalingKeys[z].mValue.x, channel->mScalingKeys[z].mValue.y, channel->mScalingKeys[z].mValue.z);
				keyframe.BoneScale.emplace_back(ScaleKeyFrame);
			}

			animation.AddBoneKeyFrame(keyframe);
		}

		AnimationList.emplace_back(animation);
	}
}

void Model::LoadBones(VulkanEngine& engine, const aiNode* RootNode, const aiMesh* mesh, std::vector<Vertex>& VertexList)
{
	for (int x = 0; x < mesh->mNumBones; x++)
	{
		bool Exists = false;
		auto node = RootNode->FindNode(mesh->mBones[x]->mName.data);
		BoneList.emplace_back(std::make_shared<Bone>(mesh->mBones[x]->mName.data, x, AssimpToGLMMatrixConverter(mesh->mBones[x]->mOffsetMatrix)));
	}
}

void Model::LoadMesh(VulkanEngine& engine, MeshManager& meshManager, MaterialManager& materialManager, TextureManager& textureManager, const std::string& FilePath, aiNode* node, const aiScene* scene)
{
	uint32_t TotalVertex = 0;
	uint32_t TotalIndex = 0;

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		 auto vertices = LoadVertices(mesh);
		 auto indices = LoadIndices(mesh);
		 auto materialID = LoadMaterial(engine, materialManager, textureManager, FilePath, mesh, scene);
		
		LoadBones(engine, scene->mRootNode, mesh, vertices);

		meshManager.AddMesh(std::make_shared<Mesh>(engine, vertices, indices, materialID));
		meshManager.MeshList.back()->ParentModelID = ModelID;
		meshManager.MeshList.back()->VertexList = vertices;
		meshManager.MeshList.back()->MeshTransform = AssimpToGLMMatrixConverter(node->mTransformation);
		TotalVertex += meshManager.MeshList.back()->VertexCount;
		TotalIndex += meshManager.MeshList.back()->IndexCount;
		for (auto nodeMap : NodeMapList)
		{
			if (nodeMap.NodeString == node->mName.C_Str())
			{
				meshManager.MeshList.back()->MeshID = nodeMap.NodeID;
			}
		}
		MeshList.emplace_back(meshManager.MeshList.back());
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		LoadMesh(engine, meshManager, materialManager, textureManager, FilePath, node->mChildren[i], scene);
	}
}

std::vector<Vertex> Model::LoadVertices(aiMesh* mesh)
{
	std::vector<Vertex> VertexList;

	for (int x = 0; x < mesh->mNumVertices; x++)
	{
		Vertex vertex;
		vertex.Position = glm::vec3{ mesh->mVertices[x].x, mesh->mVertices[x].y, mesh->mVertices[x].z };
		vertex.Normal = glm::vec3{ mesh->mNormals[x].x, mesh->mNormals[x].y, mesh->mNormals[x].z };

	
		//if(mesh->mColors[x])
		//{
		//	vertex.Color = glm::vec4{ mesh->mColors[x]->r, mesh->mColors[x]->g, mesh->mColors[x]->b, mesh->mColors[x]->a };
		//}
		//else
		//{
			vertex.Color = glm::vec4{ 0.6f, 0.6f, 0.6f, 1.0f };
		//}
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
		ModelVertices.emplace_back(vertex);
	}

	for (int x = 0; x < mesh->mNumBones; x++)
	{
		std::vector<unsigned int> AffectedVertices;
		AffectedVertices.resize(VertexList.size(), 0);

		aiBone* bone = mesh->mBones[x];
		for (int y = 0; y < bone->mNumWeights; y++)
		{
			unsigned int vertexID = bone->mWeights[y].mVertexId;
			float weight = bone->mWeights[y].mWeight;
			BoneWeightPlacement(VertexList, vertexID, x, weight);
		}
	}

	for (int x = 0; x < VertexList.size(); x++)
	{
		float Weight = VertexList[x].BoneWeights.x +
			VertexList[x].BoneWeights.y +
			VertexList[x].BoneWeights.z +
			VertexList[x].BoneWeights.w;
		if (Weight != 1.0f)
		{
			VertexList[x].BoneWeights = glm::vec4(
				VertexList[x].BoneWeights.x / Weight,
				VertexList[x].BoneWeights.y / Weight,
				VertexList[x].BoneWeights.z / Weight,
				VertexList[x].BoneWeights.w / Weight);
		}
	}

	return VertexList;
}

std::vector<uint32_t> Model::LoadIndices(aiMesh* mesh)
{
	std::vector<uint32_t> IndexList;

	for (int x = 0; x < mesh->mNumFaces; x++)
	{
		aiFace Faces = mesh->mFaces[x];
		for (int y = 0; y < Faces.mNumIndices; y++)
		{
			IndexList.emplace_back(Faces.mIndices[y]);
			ModelIndices.emplace_back(Faces.mIndices[y]);
		}
	}

	return IndexList;
}

uint32_t Model::LoadMaterial(VulkanEngine& engine, MaterialManager& materailManager, TextureManager& textureManager, const std::string& FilePath, aiMesh* mesh, const aiScene* scene)
{
	std::shared_ptr<Material> ModelMaterial = std::make_shared<Material>(engine, textureManager);

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	auto directory = FilePath.substr(0, FilePath.find_last_of('/')) + '/';

	aiColor3D color(0.f, 0.f, 0.f);
	material->Get(AI_MATKEY_COLOR_AMBIENT, color);
	ModelMaterial->materialTexture.Ambient.r = color.r;
	ModelMaterial->materialTexture.Ambient.g = color.g;
	ModelMaterial->materialTexture.Ambient.b = color.b;

	material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	ModelMaterial->materialTexture.Diffuse.r = color.r;
	ModelMaterial->materialTexture.Diffuse.g = color.g;
	ModelMaterial->materialTexture.Diffuse.b = color.b;

	material->Get(AI_MATKEY_COLOR_SPECULAR, color);
	ModelMaterial->materialTexture.Specular.r = color.r;
	ModelMaterial->materialTexture.Specular.g = color.g;
	ModelMaterial->materialTexture.Specular.b = color.b;


	if (AI_SUCCESS != aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &ModelMaterial->materialTexture.Shininess))
	{
		ModelMaterial->materialTexture.Shininess = 32.0f;
	}

	if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_REFLECTIVITY, &ModelMaterial->materialTexture.Reflectivness))
	{
		ModelMaterial->materialTexture.Reflectivness = 0.0f;
	}

	aiString TextureLocation;
	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_DIFFUSE); x++)
	{
		material->GetTexture(aiTextureType_DIFFUSE, x, &TextureLocation);
		ModelMaterial->materialTexture.DiffuseMap = textureManager.LoadTexture2D(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_SPECULAR); x++)
	{
		material->GetTexture(aiTextureType_SPECULAR, x, &TextureLocation);
		ModelMaterial->materialTexture.SpecularMap = textureManager.LoadTexture2D(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_NORMALS); x++)
	{
		material->GetTexture(aiTextureType_NORMALS, x, &TextureLocation);
		ModelMaterial->materialTexture.NormalMap = textureManager.LoadTexture2D(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_HEIGHT); x++)
	{
		material->GetTexture(aiTextureType_HEIGHT, x, &TextureLocation);
		ModelMaterial->materialTexture.DepthMap = textureManager.LoadTexture2D(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_OPACITY); x++)
	{
		material->GetTexture(aiTextureType_OPACITY, x, &TextureLocation);
		ModelMaterial->materialTexture.AlphaMap = textureManager.LoadTexture2D(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_EMISSIVE); x++)
	{
		material->GetTexture(aiTextureType_EMISSIVE, x, &TextureLocation);
		ModelMaterial->materialTexture.EmissionMap = textureManager.LoadTexture2D(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
	}

	return materailManager.LoadMaterial(engine, "sfsd", ModelMaterial);
}

void Model::BoneWeightPlacement(std::vector<Vertex>& VertexList, unsigned int vertexID, unsigned int bone_id, float weight)
{
	for (unsigned int i = 0; i < MAX_BONE_VERTEX_COUNT; i++)
	{
		if (VertexList[vertexID].BoneWeights[i] == 0.0)
		{
			VertexList[vertexID].BoneID[i] = bone_id;
			VertexList[vertexID].BoneWeights[i] = weight;
			return;
		}
	}
}

void Model::LoadMeshTransform(const int NodeID, const glm::mat4 ParentMatrix)
{
	glm::mat4 glmTransform = AssimpToGLMMatrixConverter(NodeMapList[NodeID].NodeTransform);
	glm::mat4 GlobalTransform = ParentMatrix * glmTransform;

	for (int x = 0; x < NodeMapList[NodeID].ChildNodeList.size(); x++)
	{
		LoadMeshTransform(NodeMapList[NodeID].ChildNodeList[x], GlobalTransform);
	}
}

void Model::Update(VulkanEngine& engine, MaterialManager& materialManager)
{
	ModelTransform = glm::mat4(1.0f);
	ModelTransform = glm::translate(ModelTransform, ModelPosition);
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	ModelTransform = glm::scale(ModelTransform, ModelScale);

	if (BoneList.size() > 0)
	{
		AnimationPlayer.Update();
	}

	for (auto& mesh : MeshList)
	{
		mesh->Update(engine, ModelTransform, BoneList, materialManager);
	}
}

void Model::AddMesh(VulkanEngine& engine, std::shared_ptr<Mesh> mesh)
{
	mesh->ParentModelID = ModelID;
	MeshList.emplace_back(mesh);
}

glm::mat4 Model::AssimpToGLMMatrixConverter(aiMatrix4x4 AssMatrix)
{
	glm::mat4 GLMMatrix;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			GLMMatrix[x][y] = AssMatrix[y][x];
		}
	}
	return GLMMatrix;
}

VkTransformMatrixKHR Model::GLMToVkTransformMatrix(glm::mat4 matrix)
{
	return VkTransformMatrixKHR
	{
		matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
		matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
		matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
	};
}