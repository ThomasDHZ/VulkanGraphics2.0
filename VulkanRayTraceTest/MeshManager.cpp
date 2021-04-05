#include "MeshManager.h"

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
}

void MeshManager::AddMesh(std::shared_ptr<Mesh> mesh)
{
	MeshList.emplace_back(mesh);
}

void MeshManager::Update(VulkanEngine& engine, MaterialManager& materialManager)
{
    for (auto& mesh : MeshList)
    {
        if (mesh->ParentModelID == 0)
        {
            mesh->Update(engine, materialManager);
        }
    }
}

void MeshManager::UpdateBufferIndex(VulkanEngine& engine)
{
	for (int x = 0; x < MeshList.size(); x++)
	{
		MeshList[x]->MeshBufferIndex = x;
	}
}

void MeshManager::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout layout)
{
	for (auto& mesh : MeshList)
	{
		mesh->Draw(commandBuffer, layout);
	}
}

void MeshManager::Destroy(VulkanEngine& engine)
{
    for (auto mesh : MeshList)
    {
        mesh->Destory(engine);
    }
}

std::vector<VkDescriptorBufferInfo> MeshManager::GetVertexBufferListDescriptors()
{
    std::vector<VkDescriptorBufferInfo> VertexBufferInfoList;
    for (auto& mesh : MeshList)
    {
        VkDescriptorBufferInfo VertexBufferInfo = {};
        VertexBufferInfo.buffer = mesh->VertexBuffer.Buffer;
        VertexBufferInfo.offset = 0;
        VertexBufferInfo.range = VK_WHOLE_SIZE;
        VertexBufferInfoList.emplace_back(VertexBufferInfo);
    }
    return VertexBufferInfoList;
}

std::vector<VkDescriptorBufferInfo> MeshManager::GetIndexBufferListDescriptors()
{
    std::vector<VkDescriptorBufferInfo> IndexBufferInfoList;
    for (auto& mesh : MeshList)
    {
        VkDescriptorBufferInfo IndexBufferInfo = {};
        IndexBufferInfo.buffer = mesh->IndexBuffer.Buffer;
        IndexBufferInfo.offset = 0;
        IndexBufferInfo.range = VK_WHOLE_SIZE;
        IndexBufferInfoList.emplace_back(IndexBufferInfo);
    }
    return IndexBufferInfoList;
}

std::vector<VkDescriptorBufferInfo> MeshManager::GetTransformBufferListDescriptors()
{
    std::vector<VkDescriptorBufferInfo> TransformBufferList{};
    for (auto& mesh : MeshList)
    {
        VkDescriptorBufferInfo TransformBufferInfo = {};
        TransformBufferInfo.buffer = mesh->TransformBuffer.Buffer;
        TransformBufferInfo.offset = 0;
        TransformBufferInfo.range = VK_WHOLE_SIZE;
        TransformBufferList.emplace_back(TransformBufferInfo);
    }
    return TransformBufferList;
}

std::vector<VkDescriptorBufferInfo> MeshManager::GetMeshPropertiesListDescriptors()
{
    std::vector<VkDescriptorBufferInfo> MeshPropertiesmBufferList{};
    for (auto& mesh : MeshList)
    {
        VkDescriptorBufferInfo MeshPropertiesmBufferBufferInfo = {};
        MeshPropertiesmBufferBufferInfo.buffer = mesh->MeshProperties.VulkanBufferData.Buffer;
        MeshPropertiesmBufferBufferInfo.offset = 0;
        MeshPropertiesmBufferBufferInfo.range = VK_WHOLE_SIZE;
        MeshPropertiesmBufferList.emplace_back(MeshPropertiesmBufferBufferInfo);
    }
    return MeshPropertiesmBufferList;
}

std::shared_ptr<Mesh> MeshManager::GetMeshIndex(uint32_t MeshID)
{
    std::shared_ptr<Mesh> meshptr;
    for (auto& mesh : MeshList)
    {
        if (MeshID == mesh->MeshID)
        {
            meshptr = mesh;
        }
    }

    return meshptr;
}