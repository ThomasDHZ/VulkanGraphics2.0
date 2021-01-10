#include "Model.h"
#include "ImGui/imgui.h"

Model::Model()
{
}

Model::Model(VulkanEngine& engine, std::shared_ptr<TextureManager>& textureManager, const std::vector<Vertex>& vertexdata, const std::vector<uint32_t>& indicesdata, MeshTextures textures, VkDescriptorSetLayout& layout, int renderFlags)
{
	MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, textureManager, vertexdata, indicesdata, textures, layout, renderFlags)));
}

Model::Model(VulkanEngine& engine, std::shared_ptr<TextureManager>& textureManager, const std::string& FilePath, VkDescriptorSetLayout layout, int renderFlags, std::shared_ptr<Texture> shadowTexture)
{
	RenderFlags = renderFlags;
	Assimp::Importer ModelImporter;

	const aiScene* Scene = ModelImporter.ReadFile(FilePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		std::cout << "Error loading model: " << ModelImporter.GetErrorString() << std::endl;
		return;
	}

	GlobalInverseTransformMatrix = AssimpToGLMMatrixConverter(Scene->mRootNode->mTransformation.Inverse());

	LoadNodeTree(Scene->mRootNode);
//	LoadAnimations(Scene);
	LoadMesh(engine, textureManager, FilePath, Scene->mRootNode, Scene, shadowTexture);
//
//	for (auto mesh : SubMeshList)
//	{
//		MeshList.emplace_back(std::make_shared<Mesh>(Mesh(engine, textureManager, mesh, layout, renderFlags)));
//	}
//
//	LoadMeshTransform(0, ModelTransformMatrix);
//
//	if (AnimationList.size() > 0)
//	{
//		AnimationPlayer = AnimationPlayer3D(BoneList, NodeMapList, GlobalInverseTransformMatrix, AnimationList[0]);
//	}
}

Model::~Model()
{

}

void Model::LoadMesh(VulkanEngine& renderer, std::shared_ptr<TextureManager>& textureManager, const std::string& FilePath, aiNode* node, const aiScene* scene, std::shared_ptr<Texture> shadowTexture)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		MeshData NewMesh;
		NewMesh.MeshID = SubMeshList.size();
		NewMesh.NodeName = node->mName.C_Str();
		NewMesh.VertexList = LoadVertices(mesh);
		NewMesh.IndexList = LoadIndices(mesh);
		LoadTextures(renderer, textureManager, NewMesh, FilePath, mesh, scene, shadowTexture);
		NewMesh.TransformMatrix = AssimpToGLMMatrixConverter(node->mTransformation);
		LoadBones(scene->mRootNode, mesh, NewMesh.VertexList);
		for (auto nodeMap : NodeMapList)
		{
			if (nodeMap.NodeString == node->mName.C_Str())
			{
				NewMesh.NodeID = nodeMap.NodeID;
				nodeMap.MeshID = NewMesh.MeshID;
			}
		}
		SubMeshList.emplace_back(NewMesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		LoadMesh(renderer, textureManager, FilePath, node->mChildren[i], scene, shadowTexture);
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
			vertex.Tangant = glm::vec3{ mesh->mTangents[x].x, mesh->mTangents[x].y, mesh->mTangents[x].z };
			vertex.BiTangant = glm::vec3{ mesh->mBitangents[x].x, mesh->mBitangents[x].y, mesh->mBitangents[x].z };
		}
		else
		{
			vertex.Tangant = glm::vec3{ 0.0f, 0.0f, 0.0f };
			vertex.BiTangant = glm::vec3{ 0.0f, 0.0f, 0.0f };
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
		}
	}

	return IndexList;
}

void Model::LoadBones(const aiNode* RootNode, const aiMesh* mesh, std::vector<Vertex>& VertexList)
{
	for (int x = 0; x < mesh->mNumBones; x++)
	{
		bool Exists = false;
		auto node = RootNode->FindNode(mesh->mBones[x]->mName.data);
		BoneList.emplace_back(std::make_shared<Bone>(mesh->mBones[x]->mName.data, x, AssimpToGLMMatrixConverter(mesh->mBones[x]->mOffsetMatrix)));
	}
}

void Model::LoadMeshTransform(const int NodeID, const glm::mat4 ParentMatrix)
{
	glm::mat4 glmTransform = AssimpToGLMMatrixConverter(NodeMapList[NodeID].NodeTransform);
	glm::mat4 GlobalTransform = ParentMatrix * glmTransform;

	for (auto mesh : MeshList)
	{
		if (mesh->GetNodeId() == NodeID)
		{
			mesh->SetTransformMatrix(GlobalTransform);
		}
	}

	for (int x = 0; x < NodeMapList[NodeID].ChildNodeList.size(); x++)
	{
		LoadMeshTransform(NodeMapList[NodeID].ChildNodeList[x], GlobalTransform);
	}
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

void Model::LoadTextures(VulkanEngine& engine, std::shared_ptr<TextureManager> textureManager, MeshData& Properties, const std::string& FilePath, aiMesh* mesh, const aiScene* scene, std::shared_ptr<Texture> shadowTexture)
{
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	auto directory = FilePath.substr(0, FilePath.find_last_of('/')) + '/';

	MeshTextures meshTextures;
	meshTextures.DiffuseMap = DefaultTexture;
	meshTextures.RendererShadowMap = shadowTexture;
	meshTextures.ReflectionMap = "C:/Users/dotha/source/repos/VulkanGraphics/texture/pbr/plastic/metallic.png";
	meshTextures.CubeMap[0] = DefaultTexture;
	meshTextures.CubeMap[1] = DefaultTexture;
	meshTextures.CubeMap[2] = DefaultTexture;
	meshTextures.CubeMap[3] = DefaultTexture;
	meshTextures.CubeMap[4] = DefaultTexture;
	meshTextures.CubeMap[5] = DefaultTexture;

	Properties.properties.material.ambient = glm::vec3(0.5f, 0.5f, 0.5f);
	Properties.properties.material.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	Properties.properties.material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
	Properties.properties.material.shininess = 32;
	Properties.properties.material.reflectivness = 0;
	Properties.properties.minLayers = 8.0f;
	Properties.properties.maxLayers = 32.0f;
	Properties.properties.heightScale = 0.1f;


	aiColor3D color(0.f, 0.f, 0.f);
	material->Get(AI_MATKEY_COLOR_AMBIENT, color);
	Properties.properties.material.ambient.r = color.r;
	Properties.properties.material.ambient.g = color.g;
	Properties.properties.material.ambient.b = color.b;

	material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	Properties.properties.material.diffuse.r = color.r;
	Properties.properties.material.diffuse.g = color.g;
	Properties.properties.material.diffuse.b = color.b;

	material->Get(AI_MATKEY_COLOR_SPECULAR, color);
	Properties.properties.material.specular.r = color.r;
	Properties.properties.material.specular.g = color.g;
	Properties.properties.material.specular.b = color.b;

	if (AI_SUCCESS != aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &Properties.properties.material.shininess))
	{
		Properties.properties.material.shininess = 32.0f;
	}

	if (AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_REFLECTIVITY, &Properties.properties.material.reflectivness))
	{
		Properties.properties.material.reflectivness = 0.0f;
	}

	aiString TextureLocation;
	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_DIFFUSE); x++)
	{
		material->GetTexture(aiTextureType_DIFFUSE, x, &TextureLocation);
		if (!textureManager->GetTextureByName(directory + TextureLocation.C_Str()))
		{
			meshTextures.DiffuseMap = directory + TextureLocation.C_Str();
		}
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_SPECULAR); x++)
	{
		material->GetTexture(aiTextureType_SPECULAR, x, &TextureLocation);
		if (!textureManager->GetTextureByName(directory + TextureLocation.C_Str()))
		{
			meshTextures.SpecularMap = directory + TextureLocation.C_Str();
		}
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_NORMALS); x++)
	{
		material->GetTexture(aiTextureType_NORMALS, x, &TextureLocation);
		if (!textureManager->GetTextureByName(directory + TextureLocation.C_Str()))
		{
			meshTextures.NormalMap = directory + TextureLocation.C_Str();
		}
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_HEIGHT); x++)
	{
		material->GetTexture(aiTextureType_HEIGHT, x, &TextureLocation);
		if (!textureManager->GetTextureByName(directory + TextureLocation.C_Str()))
		{
			meshTextures.DepthMap = directory + TextureLocation.C_Str();
		}
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_OPACITY); x++)
	{
		material->GetTexture(aiTextureType_OPACITY, x, &TextureLocation);
		if (!textureManager->GetTextureByName(directory + TextureLocation.C_Str()))
		{
			meshTextures.AlphaMap = directory + TextureLocation.C_Str();
		}
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_EMISSIVE); x++)
	{
		material->GetTexture(aiTextureType_EMISSIVE, x, &TextureLocation);
		if (!textureManager->GetTextureByName(directory + TextureLocation.C_Str()))
		{
			meshTextures.EmissionMap = directory + TextureLocation.C_Str();
		}
	}

	for (unsigned int x = 0; x < material->GetTextureCount(aiTextureType_REFLECTION); x++)
	{
		material->GetTexture(aiTextureType_REFLECTION, x, &TextureLocation);
		if (!textureManager->GetTextureByName(directory + TextureLocation.C_Str()))
		{
			meshTextures.ReflectionMap = directory + TextureLocation.C_Str();
		}
	}

	if (meshTextures.DiffuseMap != DefaultTexture)
	{
		Properties.properties.UseDiffuseMapBit = 1;
	}

	if (meshTextures.SpecularMap != DefaultTexture)
	{
		Properties.properties.UseSpecularMapBit = 1;
	}

	if (meshTextures.NormalMap != DefaultTexture)
	{
		Properties.properties.UseNormalMapBit = 1;
	}

	if (meshTextures.DepthMap != DefaultTexture)
	{
		Properties.properties.UseDepthMapBit = 1;
	}

	if (meshTextures.AlphaMap != DefaultTexture)
	{
		Properties.properties.UseAlphaMapBit = 1;
	}

	if (meshTextures.EmissionMap != DefaultTexture)
	{
		Properties.properties.UseEmissionMapBit = 1;
	}

	if (meshTextures.ReflectionMap != DefaultTexture)
	{
		Properties.properties.UseReflectionMapBit = 1;
	}

	Properties.TextureList = meshTextures;
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

void Model::Draw(VkCommandBuffer& RenderCommandBuffer, std::shared_ptr<GraphicsPipeline> pipeline, int FrameNumber)
{
	for (auto mesh : MeshList)
	{
		mesh->Draw(RenderCommandBuffer, pipeline, FrameNumber);
	}
}

void Model::Update(VulkanEngine& engine, std::shared_ptr<PerspectiveCamera>& camera, LightBufferObject& light)
{
	glm::mat4 modelMatrix = ModelTransformMatrix;
	modelMatrix = glm::translate(modelMatrix, ModelPosition);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(ModelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(ModelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(ModelRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, ModelScale);

	if (NodeMapList.size() > 0)
	{
		LoadMeshTransform(0, modelMatrix);
	}
	if (BoneList.size() > 0)
	{
		AnimationPlayer.Update();
	}
	for (auto mesh : MeshList)
	{
		//mesh->properites.material.specular = glm::vec3(0.02f);
		//mesh->properites.EmissionStrength = abs(sin(glfwGetTime()));
		mesh->Update(engine, camera, light, BoneList, modelMatrix);
	}
}

void Model::UpdateImGUI()
{

	ImGui::Begin("Model");

	if (ImGui::TreeNode("MeshList"))
	{
		ImGui::Text("Node contents");
		ImGui::Columns(2, "tree", true);
		for (int x = 0; x < MeshList.size(); x++)
		{
			ImGui::NextColumn();
			ImGui::Text(MeshList[x]->GetMeshName().c_str());
			ImGui::SliderFloat3("Position", &MeshList[x]->MeshPosition.x, 0.0f, 20.0f);
			ImGui::SliderFloat3("Rotation", &MeshList[x]->MeshRotate.x, 0.0f, 360.0f);
			ImGui::SliderFloat3("Scale", &MeshList[x]->MeshScale.x, 0.0f, 20.0f);
			ImGui::NextColumn();
		}
		ImGui::TreePop();
	}

	//if (ImGui::Button("Play"))
	//{
	//	if (AnimationPlayer.GetPlayAnimationFlag())
	//	{
	//		AnimationPlayer.SetPlayAnimationFlag(false);
	//	}
	//	else
	//	{
	//		AnimationPlayer.SetPlayAnimationFlag(true);
	//	}
	//}
	ImGui::SliderFloat3("Position", &MeshList[0]->MeshPosition.x, 0.0f, 20.0f);
	ImGui::SliderFloat3("Rotation", &MeshList[0]->MeshRotate.x, 0.0f, 360.0f);
	ImGui::SliderFloat3("Scale", &MeshList[0]->MeshScale.x, 0.0f, 20.0f);
	ImGui::SliderFloat("Shininess", &MeshList[0]->properites.material.shininess, 0.001f, 256.0f);
	ImGui::SliderFloat("Reflectivness", &MeshList[0]->properites.material.reflectivness, 0.0f, 1.0f);

	ImGui::SliderFloat("Anibar", AnimationPlayer.GetAnimationTimePtr(), 0.0f, AnimationPlayer.GetAnimationLength());
	ImGui::SliderFloat("PlaySpeed", AnimationPlayer.GetAnimationPlaySpeedPtr(), 0.0f, 10.0f);
	ImGui::End();
}

void Model::Destroy(VulkanEngine& engine)
{
	for (auto mesh : MeshList)
	{
		mesh->Destory(engine);
	}
}