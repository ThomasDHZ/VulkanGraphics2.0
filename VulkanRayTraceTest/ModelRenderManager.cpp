#include "ModelRenderManager.h"

std::vector<VkDescriptorBufferInfo> ModelRenderManager::VertexBufferListDescriptor()
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

std::vector<VkDescriptorBufferInfo> ModelRenderManager::IndexBufferListDescriptor()
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

std::vector<VkDescriptorBufferInfo> ModelRenderManager::MaterialBufferListDescriptor()
{
    return std::vector<VkDescriptorBufferInfo>();
}

std::vector<VkDescriptorBufferInfo> ModelRenderManager::TextureBufferListDescriptor()
{
    return std::vector<VkDescriptorBufferInfo>();
}

ModelRenderManager::ModelRenderManager()
{
}

ModelRenderManager::ModelRenderManager(VulkanEngine& engine)
{
}

ModelRenderManager::~ModelRenderManager()
{
}

void ModelRenderManager::AddModel(VulkanEngine& engine, TextureManager& textureManager, const std::string& FilePath)
{
    ModelList.emplace_back(Model(engine, textureManager, FilePath));
}

void ModelRenderManager::UpdateMeshDescriptors(VulkanEngine& engine, VkDescriptorSet& descriptorSet)
{
    std::vector<VkDescriptorBufferInfo> VertexDescriptorInfo = VertexBufferListDescriptor();
    VkWriteDescriptorSet VertexDescriptor{};
    VertexDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    VertexDescriptor.dstSet = descriptorSet;
    VertexDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    VertexDescriptor.dstBinding = 3;
    VertexDescriptor.descriptorCount = static_cast<uint32_t>(VertexDescriptorInfo.size());
    VertexDescriptor.pBufferInfo = VertexDescriptorInfo.data();
    vkUpdateDescriptorSets(engine.Device, 1, &VertexDescriptor, 0, VK_NULL_HANDLE);

    std::vector<VkDescriptorBufferInfo> IndexDescriptorInfo = IndexBufferListDescriptor();
    VkWriteDescriptorSet IndexDescriptor{};
    IndexDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    IndexDescriptor.dstSet = descriptorSet;
    IndexDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
    IndexDescriptor.dstBinding = 4;
    IndexDescriptor.descriptorCount = static_cast<uint32_t>(IndexDescriptorInfo.size());
    IndexDescriptor.pBufferInfo = IndexDescriptorInfo.data();
    vkUpdateDescriptorSets(engine.Device, 1, &IndexDescriptor, 0, VK_NULL_HANDLE);
}
