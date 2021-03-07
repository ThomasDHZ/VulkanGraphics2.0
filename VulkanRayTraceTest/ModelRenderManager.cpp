#include "ModelRenderManager.h"

ModelRenderManager::ModelRenderManager()
{
}

ModelRenderManager::ModelRenderManager(VulkanEngine& engine)
{
    textureManager = TextureManager(engine);
}

ModelRenderManager::~ModelRenderManager()
{
}

void ModelRenderManager::AddModel(VulkanEngine& engine, const std::string& FilePath)
{
    ModelList.emplace_back(Model(engine, textureManager, FilePath));
}

void ModelRenderManager::AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList)
{
    ModelList.emplace_back(Model(engine, VertexList, IndexList));
}

void ModelRenderManager::AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, Material& material)
{
    ModelList.emplace_back(Model(engine, VertexList, IndexList, material));
}

void ModelRenderManager::UpdateMeshDescriptors(VulkanEngine& engine, VkDescriptorSet& descriptorSet)
{
    //std::vector<VkDescriptorBufferInfo> VertexDescriptorInfo = VertexBufferListDescriptor();
    //VkWriteDescriptorSet VertexDescriptor{};
    //VertexDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //VertexDescriptor.dstSet = descriptorSet;
    //VertexDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    //VertexDescriptor.dstBinding = 3;
    //VertexDescriptor.descriptorCount = static_cast<uint32_t>(VertexDescriptorInfo.size());
    //VertexDescriptor.pBufferInfo = VertexDescriptorInfo.data();
    //vkUpdateDescriptorSets(engine.Device, 1, &VertexDescriptor, 0, VK_NULL_HANDLE);

    //std::vector<VkDescriptorBufferInfo> IndexDescriptorInfo = IndexBufferListDescriptor();
    //VkWriteDescriptorSet IndexDescriptor{};
    //IndexDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //IndexDescriptor.dstSet = descriptorSet;
    //IndexDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    //IndexDescriptor.dstBinding = 4;
    //IndexDescriptor.descriptorCount = static_cast<uint32_t>(IndexDescriptorInfo.size());
    //IndexDescriptor.pBufferInfo = IndexDescriptorInfo.data();
    //vkUpdateDescriptorSets(engine.Device, 1, &IndexDescriptor, 0, VK_NULL_HANDLE);
}

std::vector<VkDescriptorBufferInfo> ModelRenderManager::GetVertexBufferListDescriptor()
{
    std::vector<VkDescriptorBufferInfo> VertexBufferInfoList;
    for (int x = 0; x < ModelList.size(); x++)
    {
        for (int y = 0; y < ModelList[x].MeshList.size(); y++)
        {
            VkDescriptorBufferInfo VertexBufferInfo = {};
            VertexBufferInfo.buffer = ModelList[x].MeshList[y].VertexBuffer.Buffer;
            VertexBufferInfo.offset = 0;
            VertexBufferInfo.range = VK_WHOLE_SIZE;
            VertexBufferInfoList.emplace_back(VertexBufferInfo);
        }
    }
    return VertexBufferInfoList;
}

std::vector<VkDescriptorBufferInfo> ModelRenderManager::GetIndexBufferListDescriptor()
{
    std::vector<VkDescriptorBufferInfo> IndexBufferInfoList;
    for (int x = 0; x < ModelList.size(); x++)
    {
        for (int y = 0; y < ModelList[x].MeshList.size(); y++)
        {
            VkDescriptorBufferInfo IndexBufferInfo = {};
            IndexBufferInfo.buffer = ModelList[x].MeshList[y].IndexBuffer.Buffer;
            IndexBufferInfo.offset = 0;
            IndexBufferInfo.range = VK_WHOLE_SIZE;
            IndexBufferInfoList.emplace_back(IndexBufferInfo);
        }
    }
    return IndexBufferInfoList;
}

std::vector<VkDescriptorBufferInfo> ModelRenderManager::GetMaterialBufferListDescriptor()
{

    std::vector<VkDescriptorBufferInfo> MaterialBufferList{};
    for (int x = 0; x < ModelList.size(); x++)
    {
        for (int y = 0; y < ModelList[x].MeshList.size(); y++)
        {
            VkDescriptorBufferInfo MaterialBufferInfo = {};
            MaterialBufferInfo.buffer = ModelList[x].MeshList[y].MaterialBuffer.Buffer;
            MaterialBufferInfo.offset = 0;
            MaterialBufferInfo.range = VK_WHOLE_SIZE;
            MaterialBufferList.emplace_back(MaterialBufferInfo);
        }
    }

    return MaterialBufferList;
}

std::vector<VkDescriptorBufferInfo> ModelRenderManager::GetTransformBufferListDescriptor()
{
    std::vector<VkDescriptorBufferInfo> TransformBufferList{};
    for (int x = 0; x < ModelList.size(); x++)
    {
        for (int y = 0; y < ModelList[x].MeshList.size(); y++)
        {
            VkDescriptorBufferInfo TransformBufferInfo = {};
            TransformBufferInfo.buffer = ModelList[x].MeshList[y].TransformBuffer.Buffer;
            TransformBufferInfo.offset = 0;
            TransformBufferInfo.range = VK_WHOLE_SIZE;
            TransformBufferList.emplace_back(TransformBufferInfo);
        }
    }
    return TransformBufferList;
}

std::vector<VkDescriptorImageInfo> ModelRenderManager::GetTextureBufferListDescriptor()
{
    std::vector<VkDescriptorImageInfo> DescriptorImageList;
    for (auto texture : textureManager.GetTextureList())
    {
        VkDescriptorImageInfo DescriptorImage{};
        DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        DescriptorImage.imageView = texture->GetTextureView();
        DescriptorImage.sampler = texture->GetTextureSampler();
        DescriptorImageList.emplace_back(DescriptorImage);
    }
    return DescriptorImageList;
}

std::vector<VkDescriptorImageInfo> ModelRenderManager::Get3DTextureBufferListDescriptor()
{
    std::vector<VkDescriptorImageInfo> DescriptorImageList;
    for (auto texture : textureManager.Get3DTextureList())
    {
        VkDescriptorImageInfo DescriptorImage{};
        DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        DescriptorImage.imageView = texture->GetTextureView();
        DescriptorImage.sampler = texture->GetTextureSampler();
        DescriptorImageList.emplace_back(DescriptorImage);
    }
    return DescriptorImageList;
}

uint32_t ModelRenderManager::GetVertexBufferListDescriptorCount()
{
    uint32_t count = 0;
    for (int x = 0; x < ModelList.size(); x++)
    {
        for (int y = 0; y < ModelList[x].MeshList.size(); y++)
        {
            count++;
        }
    }
    return count++;
}

uint32_t ModelRenderManager::GetIndexBufferListDescriptorCount()
{
    uint32_t count = 0;
    for (int x = 0; x < ModelList.size(); x++)
    {
        for (int y = 0; y < ModelList[x].MeshList.size(); y++)
        {
            count++;
        }
    }
    return count++;
}

uint32_t ModelRenderManager::GetTransformBufferListDescriptorCount()
{
    uint32_t count = 0;
    for (int x = 0; x < ModelList.size(); x++)
    {
        for (int y = 0; y < ModelList[x].MeshList.size(); y++)
        {
            count++;
        }
    }
    return count++;
}

uint32_t ModelRenderManager::GetMaterialBufferListDescriptorCount()
{
    uint32_t count = 0;
    for (int x = 0; x < ModelList.size(); x++)
    {
        for (int y = 0; y < ModelList[x].MeshList.size(); y++)
        {
            count++;
        }
    }
    return count++;
}

uint32_t ModelRenderManager::GetTextureBufferListDescriptorCount()
{
    return textureManager.GetTextureList().size();
}
