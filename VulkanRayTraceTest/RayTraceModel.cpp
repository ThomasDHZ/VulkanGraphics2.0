#include "RayTraceModel.h"
#include <chrono>


RayTraceModel::RayTraceModel()
{
}

RayTraceModel::RayTraceModel(VkDevice& device, VkPhysicalDevice& physicalDevice, MeshDetails& meshDetails)
{
	vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(device, "vkGetBufferDeviceAddressKHR"));

	Mesh ModelMesh{};
	ModelMesh.vertices = meshDetails.vertices;
	ModelMesh.indices = meshDetails.indices;

	ModelMesh.Transform = glm::mat4(1.0f);
	ModelMesh.Transform = glm::transpose(ModelMesh.Transform);

	ModelMesh.VertexCount = ModelMesh.vertices.size();
	ModelMesh.IndexCount = ModelMesh.indices.size();
	ModelMesh.TriangleCount = static_cast<uint32_t>(ModelMesh.indices.size()) / 3;

	ModelMesh.VertexBuffer.CreateBuffer(device, physicalDevice, ModelMesh.VertexCount * sizeof(RTVertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ModelMesh.vertices.data());
	ModelMesh.IndexBuffer.CreateBuffer(device, physicalDevice, ModelMesh.IndexCount * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ModelMesh.indices.data());
	ModelMesh.TransformBuffer.CreateBuffer(device, physicalDevice, sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &ModelMesh.Transform);

	ModelMesh.VertexBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(device, ModelMesh.VertexBuffer.Buffer);
	ModelMesh.IndexBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(device, ModelMesh.IndexBuffer.Buffer);
	ModelMesh.TransformBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(device, ModelMesh.TransformBuffer.Buffer);

	MeshList.emplace_back(ModelMesh);
}

RayTraceModel::RayTraceModel(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath)
{
	Assimp::Importer ModelImporter;
	vkGetBufferDeviceAddressKHR = reinterpret_cast<PFN_vkGetBufferDeviceAddressKHR>(vkGetDeviceProcAddr(engine.Device, "vkGetBufferDeviceAddressKHR"));

	const aiScene* Scene = ModelImporter.ReadFile(FilePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		std::cout << "Error loading model: " << ModelImporter.GetErrorString() << std::endl;
		return;
	}

	LoadMesh(engine, textureManager, FilePath, Scene->mRootNode, Scene);

	ModelTransform = glm::mat4(1.0f);
	ModelTransform = glm::translate(ModelTransform, ModelPosition);
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	ModelTransform = glm::scale(ModelTransform, ModelScale);
	ModelTransform = glm::transpose(ModelTransform);

	ModelVertexCount = ModelVertices.size();
	ModelIndexCount = ModelIndices.size();
	ModelTriangleCount = static_cast<uint32_t>(ModelIndices.size()) / 3;

	ModelVertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, ModelVertexCount * sizeof(RTVertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ModelVertices.data());
	ModelIndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, ModelIndexCount * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ModelIndices.data());
	ModelTransformBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &ModelTransform);

	ModelVertexBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(engine.Device, ModelVertexBuffer.Buffer);
	ModelIndexBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(engine.Device, ModelIndexBuffer.Buffer);
	ModelTransformBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(engine.Device, ModelTransformBuffer.Buffer);
}

RayTraceModel::~RayTraceModel()
{
}

void RayTraceModel::LoadMesh(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath, aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		Mesh ModelMesh{};
		ModelMesh.vertices = LoadVertices(mesh);
		ModelMesh.indices = LoadIndices(mesh);
		ModelMesh.material = LoadMaterial(engine, textureManager, FilePath, mesh, scene);

		ModelMesh.Transform = glm::mat4(1.0f);
		ModelMesh.Transform = glm::translate(ModelMesh.Transform, glm::vec3(0.0f, 0.0f, 0.0f));
		ModelMesh.Transform = glm::rotate(ModelMesh.Transform, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMesh.Transform = glm::rotate(ModelMesh.Transform, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMesh.Transform = glm::rotate(ModelMesh.Transform, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMesh.Transform = glm::transpose(ModelMesh.Transform);

		ModelMesh.VertexCount = ModelMesh.vertices.size();
		ModelMesh.IndexCount = ModelMesh.indices.size();
		ModelMesh.TriangleCount = static_cast<uint32_t>(ModelMesh.indices.size()) / 3;

		ModelMesh.VertexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, ModelMesh.VertexCount * sizeof(RTVertex), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ModelMesh.vertices.data());
		ModelMesh.IndexBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, ModelMesh.IndexCount * sizeof(uint32_t), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, ModelMesh.indices.data());
		ModelMesh.TransformBuffer.CreateBuffer(engine.Device, engine.PhysicalDevice, sizeof(glm::mat4), VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &ModelMesh.Transform);

		ModelMesh.VertexBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(engine.Device, ModelMesh.VertexBuffer.Buffer);
		ModelMesh.IndexBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(engine.Device, ModelMesh.IndexBuffer.Buffer);
		ModelMesh.TransformBufferDeviceAddress.deviceAddress = getBufferDeviceAddress(engine.Device, ModelMesh.TransformBuffer.Buffer);

		MeshList.emplace_back(ModelMesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		LoadMesh(engine, textureManager, FilePath, node->mChildren[i], scene);
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

		if(mesh->mColors[0])
		{
			vertex.Color = glm::vec4{ mesh->mColors[x]->r, mesh->mColors[x]->g, mesh->mColors[x]->b, mesh->mColors[x]->a };
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
			ModelIndices.emplace_back(Faces.mIndices[y]);
		}
	}

	return IndexList;
}

Material RayTraceModel::LoadMaterial(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath, aiMesh* mesh, const aiScene* scene)
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

uint64_t RayTraceModel::getBufferDeviceAddress(VkDevice& device, VkBuffer buffer)
{
	VkBufferDeviceAddressInfoKHR bufferDeviceAI{};
	bufferDeviceAI.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	bufferDeviceAI.buffer = buffer;
	return vkGetBufferDeviceAddressKHR(device, &bufferDeviceAI);
}

void RayTraceModel::Update()
{
	ModelTransform = glm::mat4(1.0f);
	ModelTransform = glm::translate(ModelTransform, ModelPosition);
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelTransform = glm::rotate(ModelTransform, glm::radians(ModelRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	ModelTransform = glm::scale(ModelTransform, ModelScale);
	ModelTransform = glm::transpose(ModelTransform);
}

void RayTraceModel::Destory(VkDevice& device)
{
	for (auto& mesh : MeshList)
	{
		mesh.VertexBuffer.DestoryBuffer(device);
		mesh.IndexBuffer.DestoryBuffer(device);
		mesh.TransformBuffer.DestoryBuffer(device);
		mesh.MaterialBuffer.DestoryBuffer(device);
	}
	for (auto& buffer : MeshOffsetBufferList)
	{
		buffer.DestoryBuffer(device);
	}
	 ModelIndexBuffer.DestoryBuffer(device);
	 ModelVertexBuffer.DestoryBuffer(device);
	 ModelTransformBuffer.DestoryBuffer(device);
	 ModelUniformBuffer.DestoryBuffer(device);
}
