#include "Model.h"
#include <chrono>


Model::Model()
{
}

Model::Model(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList)
{
	MeshList.emplace_back(Mesh(engine, VertexList, IndexList, 0));

}

Model::Model(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, Material& material)
{
	MeshList.emplace_back(Mesh(engine, VertexList, IndexList, material, 0));
}

Model::Model(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath)
{
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
	LoadMesh(engine, textureManager, FilePath, Scene->mRootNode, Scene);

	LoadMeshTransform(0, ModelTransform);

	if (AnimationList.size() > 0)
	{
		AnimationPlayer = AnimationPlayer3D(BoneList, NodeMapList, GlobalInverseTransformMatrix, AnimationList[0]);
	}

	ModelTransform = AssimpToGLMMatrixConverter(Scene->mRootNode->mTransformation.Inverse());
	ModelVertexCount = ModelVertices.size();
	ModelIndexCount = ModelIndices.size();
	ModelTriangleCount = static_cast<uint32_t>(ModelIndices.size()) / 3;

	ModelVertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, ModelVertexCount * sizeof(Vertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ModelVertices.data());
	ModelIndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, ModelIndexCount * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ModelIndices.data());
	ModelTransformBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &ModelTransform);

	BottomLevelAccelerationBuffer = AccelerationStructure(engine);
	BottomLevelAccelerationStructure(engine);
}

Model::~Model()
{
}

void Model::BottomLevelAccelerationStructure(VulkanEngine& engine)
{
	std::vector<uint32_t> PrimitiveCountList;
	std::vector<VkAccelerationStructureGeometryKHR> AccelerationStructureGeometryList;
	std::vector<VkAccelerationStructureBuildRangeInfoKHR> AccelerationBuildStructureRangeInfos;
	for (auto x = 0; x < MeshList.size(); x++)
	{

		VkDeviceOrHostAddressConstKHR ModelVertexBufferDeviceAddress{};
		VkDeviceOrHostAddressConstKHR ModelIndexBufferDeviceAddress{};
		VkDeviceOrHostAddressConstKHR ModelTransformBufferDeviceAddress{};
		ModelVertexBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(ModelVertexBuffer.Buffer);
		ModelIndexBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(ModelIndexBuffer.Buffer);
		ModelTransformBufferDeviceAddress.deviceAddress = engine.GetBufferDeviceAddress(ModelTransformBuffer.Buffer);

		const Mesh mesh = MeshList[x];
		PrimitiveCountList.emplace_back(mesh.IndexCount / 3);

		VkAccelerationStructureGeometryKHR AccelerationStructureGeometry = {};
		AccelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		AccelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
		AccelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		AccelerationStructureGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		AccelerationStructureGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		AccelerationStructureGeometry.geometry.triangles.vertexData = ModelVertexBufferDeviceAddress;
		AccelerationStructureGeometry.geometry.triangles.maxVertex = mesh.VertexCount;
		AccelerationStructureGeometry.geometry.triangles.vertexStride = sizeof(Vertex);
		AccelerationStructureGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
		AccelerationStructureGeometry.geometry.triangles.indexData = ModelIndexBufferDeviceAddress;
		AccelerationStructureGeometryList.emplace_back(AccelerationStructureGeometry);

		VkAccelerationStructureBuildRangeInfoKHR AccelerationStructureBuildRangeInfo{};
		AccelerationStructureBuildRangeInfo.primitiveCount = PrimitiveCountList[x];
		AccelerationStructureBuildRangeInfo.primitiveOffset = mesh.FirstIndex * sizeof(uint32_t);
		AccelerationStructureBuildRangeInfo.firstVertex = mesh.VertexOffset;
		AccelerationStructureBuildRangeInfo.transformOffset = 0;
		AccelerationBuildStructureRangeInfos.emplace_back(AccelerationStructureBuildRangeInfo);
	}

	VkAccelerationStructureBuildGeometryInfoKHR AccelerationStructureBuildGeometryInfo = {};
	AccelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	AccelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	AccelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	AccelerationStructureBuildGeometryInfo.geometryCount = static_cast<uint32_t>(AccelerationStructureGeometryList.size());
	AccelerationStructureBuildGeometryInfo.pGeometries = AccelerationStructureGeometryList.data();

	PrimitiveCountList.resize(AccelerationBuildStructureRangeInfos.size());
	for (auto x = 0; x < AccelerationBuildStructureRangeInfos.size(); x++)
	{
		PrimitiveCountList[x] = AccelerationBuildStructureRangeInfos[x].primitiveCount;
	}

	VkAccelerationStructureBuildSizesInfoKHR AccelerationStructureBuildSizesInfo = {};
	AccelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	engine.vkGetAccelerationStructureBuildSizesKHR(engine.Device, VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &AccelerationStructureBuildGeometryInfo, PrimitiveCountList.data(), &AccelerationStructureBuildSizesInfo);

	BottomLevelAccelerationBuffer.CreateAccelerationStructure(engine, VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR, AccelerationStructureBuildSizesInfo);

	VulkanBuffer scratchBuffer = VulkanBuffer(engine.Device, engine.PhysicalDevice, AccelerationStructureBuildSizesInfo.buildScratchSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	scratchBuffer.BufferDeviceAddress = engine.GetBufferDeviceAddress(scratchBuffer.Buffer);

	VkAccelerationStructureBuildGeometryInfoKHR AccelerationBuildGeometryInfo = {};
	AccelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	AccelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	AccelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	AccelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	AccelerationBuildGeometryInfo.dstAccelerationStructure = BottomLevelAccelerationBuffer.handle;
	AccelerationBuildGeometryInfo.geometryCount = static_cast<uint32_t>(AccelerationStructureGeometryList.size());
	AccelerationBuildGeometryInfo.pGeometries = AccelerationStructureGeometryList.data();
	AccelerationBuildGeometryInfo.srcAccelerationStructure = nullptr;
	AccelerationBuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.BufferDeviceAddress;

	BottomLevelAccelerationBuffer.AcclerationCommandBuffer(engine, AccelerationBuildGeometryInfo, AccelerationBuildStructureRangeInfos);

	scratchBuffer.DestoryBuffer(engine.Device);
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

void Model::LoadMesh(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath, aiNode* node, const aiScene* scene)
{
	uint32_t TotalVertex = 0;
	uint32_t TotalIndex = 0;

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		 auto vertices = LoadVertices(mesh);
		 auto indices = LoadIndices(mesh);
		 auto material = LoadMaterial(engine, textureManager, FilePath, mesh, scene);
		
		LoadBones(engine, scene->mRootNode, mesh, vertices);

		MeshList.emplace_back(Mesh(engine, vertices, indices, material, MeshList.size()));
		MeshList.back().VertexList = vertices;
		MeshList.back().MeshTransform = AssimpToGLMMatrixConverter(node->mTransformation);
		MeshList.back().VertexOffset = TotalVertex;
		MeshList.back().FirstIndex = TotalIndex;
		TotalVertex += MeshList.back().VertexCount;
		TotalIndex += MeshList.back().IndexCount;
		for (auto nodeMap : NodeMapList)
		{
			if (nodeMap.NodeString == node->mName.C_Str())
			{
				MeshList.back().NodeID = nodeMap.NodeID;
				nodeMap.MeshID = MeshList.back().MeshID;
			}
		}
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		LoadMesh(engine, textureManager, FilePath, node->mChildren[i], scene);
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

	
		if(mesh->mColors[x])
		{
			//vertex.Color = glm::vec4{ mesh->mColors[x]->r, mesh->mColors[x]->g, mesh->mColors[x]->b, mesh->mColors[x]->a };
		}
		else
		{
			vertex.Color = glm::vec4{ 0.6f, 0.6f, 0.6f, 1.0f };
		}
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

Material Model::LoadMaterial(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath, aiMesh* mesh, const aiScene* scene)
{
	Material ModelMaterial;

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	auto directory = FilePath.substr(0, FilePath.find_last_of('/')) + '/';

	aiColor3D color(0.f, 0.f, 0.f);
	material->Get(AI_MATKEY_COLOR_AMBIENT, color);
	ModelMaterial.Ambient.r = color.r;
	ModelMaterial.Ambient.g = color.g;
	ModelMaterial.Ambient.b = color.b;

	material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	ModelMaterial.Diffuse.r = color.r;
	ModelMaterial.Diffuse.g = color.g;
	ModelMaterial.Diffuse.b = color.b;

	material->Get(AI_MATKEY_COLOR_SPECULAR, color);
	ModelMaterial.Specular.r = color.r;
	ModelMaterial.Specular.g = color.g;
	ModelMaterial.Specular.b = color.b;


	if (AI_SUCCESS != aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &ModelMaterial.Shininess))
	{
		ModelMaterial.Shininess = 32.0f;
	}

	if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_REFLECTIVITY, &ModelMaterial.Reflectivness))
	{
		ModelMaterial.Reflectivness = 0.0f;
	}

	aiString TextureLocation;
	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_DIFFUSE); x++)
	{
		material->GetTexture(aiTextureType_DIFFUSE, x, &TextureLocation);
		ModelMaterial.DiffuseMapID = textureManager.LoadTexture(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_SPECULAR); x++)
	{
		material->GetTexture(aiTextureType_SPECULAR, x, &TextureLocation);
		ModelMaterial.SpecularMapID = textureManager.LoadTexture(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_NORMALS); x++)
	{
		material->GetTexture(aiTextureType_NORMALS, x, &TextureLocation);
		ModelMaterial.NormalMapID = textureManager.LoadTexture(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_HEIGHT); x++)
	{
		material->GetTexture(aiTextureType_HEIGHT, x, &TextureLocation);
		ModelMaterial.DepthMapID = textureManager.LoadTexture(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_OPACITY); x++)
	{
		material->GetTexture(aiTextureType_OPACITY, x, &TextureLocation);
		ModelMaterial.AlphaMapID = textureManager.LoadTexture(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_EMISSIVE); x++)
	{
		material->GetTexture(aiTextureType_EMISSIVE, x, &TextureLocation);
		ModelMaterial.EmissionMapID = textureManager.LoadTexture(engine, directory + TextureLocation.C_Str(), VK_FORMAT_R8G8B8A8_UNORM);
	}

	return ModelMaterial;
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

	for (auto mesh : MeshList)
	{
		if (mesh.NodeID == NodeID)
		{
			mesh.MeshTransform = GlobalTransform;
		}
	}

	for (int x = 0; x < NodeMapList[NodeID].ChildNodeList.size(); x++)
	{
		LoadMeshTransform(NodeMapList[NodeID].ChildNodeList[x], GlobalTransform);
	}
}

void Model::Update(VulkanEngine& engine, std::shared_ptr<SceneDataStruct> scenedata)
{
	ModelTransform = glm::mat4(1.0f);
	ModelTransform = glm::translate(ModelTransform, ModelPosition);
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	ModelTransform = glm::scale(ModelTransform, ModelScale);


	LoadMeshTransform(0, ModelTransform);
	AnimationPlayer.Update();

	for (auto& mesh : MeshList)
	{
		mesh.Update(engine, BoneList, scenedata);
	}
}

void Model::Draw(VkCommandBuffer commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline)
{
	for (auto mesh : MeshList)
	{
		mesh.Draw(commandBuffer, pipeline);
	}
}

void Model::Destory(VulkanEngine& engine)
{
	for (auto& mesh : MeshList)
	{
		mesh.Destory(engine);
	}

	 ModelIndexBuffer.DestoryBuffer(engine.Device);
	 ModelVertexBuffer.DestoryBuffer(engine.Device);
	 ModelTransformBuffer.DestoryBuffer(engine.Device);

	 if (BottomLevelAccelerationBuffer.handle != VK_NULL_HANDLE)
	 {
		 BottomLevelAccelerationBuffer.Destroy(engine);
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