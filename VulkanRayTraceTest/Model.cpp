#include "Model.h"
#include <chrono>


Model::Model()
{
}

Model::Model(std::shared_ptr<VulkanEngine> engine, MaterialManager& materialManager, TextureManager& textureManager, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, MeshDrawFlags DrawFlags)
{
	ModelID = engine->GenerateID();
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, VertexList, IndexList, materialManager.GetDefaultMaterial(), DrawFlags)));
	MeshList.back()->ParentModelID = ModelID;
	MeshList.back()->VertexList = VertexList;
	MeshList.back()->MeshTransform = glm::mat4(1.0f);
}

Model::Model(std::shared_ptr<VulkanEngine> engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material, MeshDrawFlags DrawFlags)
{
	ModelID = engine->GenerateID();
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, VertexList, IndexList, material, DrawFlags)));
	MeshList.back()->ParentModelID = ModelID;
	MeshList.back()->VertexList = VertexList;
	MeshList.back()->MeshTransform = glm::mat4(1.0f);
}

Model::Model(std::shared_ptr<VulkanEngine> engine, MaterialManager& materiallManager, TextureManager& textureManager, const std::string& FilePath, MeshDrawFlags DrawFlags)
{
	ModelID = engine->GenerateID();
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
	LoadMesh(engine, materiallManager, textureManager, FilePath, Scene->mRootNode, Scene, DrawFlags);

	LoadMeshTransform(0, ModelTransform);

	if (AnimationList.size() > 0)
	{
		AnimatedModel = true;
		AnimationPlayer = AnimationPlayer3D(BoneList, NodeMapList, GlobalInverseTransformMatrix, AnimationList[0]);
		AnimationRenderer = AnimatorCompute(engine, MeshList[0]);
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

void Model::LoadBones(std::shared_ptr<VulkanEngine> engine, const aiNode* RootNode, const aiMesh* mesh, std::vector<Vertex>& VertexList)
{
	for (int x = 0; x < mesh->mNumBones; x++)
	{
		bool Exists = false;
		auto node = RootNode->FindNode(mesh->mBones[x]->mName.data);
		BoneList.emplace_back(std::make_shared<Bone>(mesh->mBones[x]->mName.data, x, AssimpToGLMMatrixConverter(mesh->mBones[x]->mOffsetMatrix)));
	}
}

void Model::LoadMesh(std::shared_ptr<VulkanEngine> engine, MaterialManager& materialManager, TextureManager& textureManager, const std::string& FilePath, aiNode* node, const aiScene* scene, MeshDrawFlags DrawFlags)
{
	uint32_t TotalVertex = 0;
	uint32_t TotalIndex = 0;

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		 auto vertices = LoadVertices(mesh);
		 auto boneWeights = LoadBoneWeights(mesh, vertices);
		 auto indices = LoadIndices(mesh);
		 auto materialID = LoadMaterial(engine, materialManager, textureManager, FilePath, mesh, scene);
		
		LoadBones(engine, scene->mRootNode, mesh, vertices);

		MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, vertices, indices, materialID, boneWeights, BoneList.size(), DrawFlags)));
		MeshList.back()->ParentModelID = ModelID;
		MeshList.back()->VertexList = vertices;
		MeshList.back()->MeshTransform = AssimpToGLMMatrixConverter(node->mTransformation);

		TotalVertex += MeshList.back()->VertexCount;
		TotalIndex += MeshList.back()->IndexCount;
		for (auto nodeMap : NodeMapList)
		{
			if (nodeMap.NodeString == node->mName.C_Str())
			{
				MeshList.back()->MeshID = nodeMap.NodeID;
			}
		}
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		LoadMesh(engine, materialManager, textureManager, FilePath, node->mChildren[i], scene, DrawFlags);
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

		VertexList.emplace_back(vertex);
		ModelVertices.emplace_back(vertex);
	}

	return VertexList;
}

std::vector<MeshBoneWeights> Model::LoadBoneWeights(aiMesh* mesh, std::vector<Vertex>& VertexList)
{
	std::vector<MeshBoneWeights> BoneWeightList(VertexList.size());
	for (int x = 0; x < mesh->mNumBones; x++)
	{
		std::vector<unsigned int> AffectedVertices;
		AffectedVertices.resize(BoneWeightList.size(), 0);

		aiBone* bone = mesh->mBones[x];
		for (int y = 0; y < bone->mNumWeights; y++)
		{
			unsigned int vertexID = bone->mWeights[y].mVertexId;
			float weight = bone->mWeights[y].mWeight;
			BoneWeightPlacement(BoneWeightList, vertexID, x, weight);
		}
	}

	for (int x = 0; x < BoneWeightList.size(); x++)
	{
		float Weight = BoneWeightList[x].BoneWeights.x +
			BoneWeightList[x].BoneWeights.y +
			BoneWeightList[x].BoneWeights.z +
			BoneWeightList[x].BoneWeights.w;
		if (Weight != 1.0f)
		{
			BoneWeightList[x].BoneWeights = glm::vec4(
				BoneWeightList[x].BoneWeights.x / Weight,
				BoneWeightList[x].BoneWeights.y / Weight,
				BoneWeightList[x].BoneWeights.z / Weight,
				BoneWeightList[x].BoneWeights.w / Weight);
		}
	}

	return BoneWeightList;
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

std::shared_ptr<Material> Model::LoadMaterial(std::shared_ptr<VulkanEngine> engine, MaterialManager& materailManager, TextureManager& textureManager, const std::string& FilePath, aiMesh* mesh, const aiScene* scene)
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
		ModelMaterial->materialTexture.DiffuseMap = textureManager.LoadTexture2D(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_SRGB);
		ModelMaterial->materialTexture.AlbedoMap = ModelMaterial->materialTexture.DiffuseMap;
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_METALNESS); x++)
	{
		material->GetTexture(aiTextureType_METALNESS, x, &TextureLocation);
		ModelMaterial->materialTexture.MatallicMap = textureManager.LoadTexture2D(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS); x++)
	{
		material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, x, &TextureLocation);
		ModelMaterial->materialTexture.RoughnessMap = textureManager.LoadTexture2D(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION); x++)
	{
		material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, x, &TextureLocation);
		ModelMaterial->materialTexture.AOMap = textureManager.LoadTexture2D(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
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

void Model::BoneWeightPlacement(std::vector<MeshBoneWeights>& meshBoneWeight, unsigned int vertexID, unsigned int bone_id, float weight)
{
	for (unsigned int i = 0; i < MAX_BONE_VERTEX_COUNT; i++)
	{
		if (meshBoneWeight[vertexID].BoneWeights[i] == 0.0)
		{
			meshBoneWeight[vertexID].BoneID[i] = bone_id;
			meshBoneWeight[vertexID].BoneWeights[i] = weight;
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

void Model::Update(std::shared_ptr<VulkanEngine> engine, InputManager& inputManager, MaterialManager& materialManager, bool RayTraceFlag)
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
		mesh->Update(engine, ModelTransform, BoneList, inputManager, materialManager, RayTraceFlag);
	}
}

void Model::Update(std::shared_ptr<VulkanEngine> engine, InputManager& inputManager, MaterialManager& materialManager)
{
	for (auto& mesh : MeshList)
	{
		mesh->Update(engine, inputManager, materialManager);
	}
}

void Model::SubmitAnimationToCommandBuffer(std::shared_ptr<VulkanEngine> engine, std::vector<VkCommandBuffer>& CMDBufferList, int imageIndex)
{
	if (AnimatedModel)
	{
		AnimationRenderer.Compute(engine, imageIndex);
		CMDBufferList.emplace_back(AnimationRenderer.commandBuffer);
	}
}

void Model::AddMesh(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<Mesh> mesh)
{
	mesh->ParentModelID = ModelID;
	MeshList.emplace_back(mesh);
}

void Model::Destory(std::shared_ptr<VulkanEngine> engine)
{
	if (AnimatedModel)
	{
		AnimationRenderer.Destroy(engine);
	}
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