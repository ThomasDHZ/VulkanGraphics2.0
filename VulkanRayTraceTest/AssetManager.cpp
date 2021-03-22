#include "AssetManager.h"

AssetManager::AssetManager()
{
}

AssetManager::AssetManager(VulkanEngine& engine)
{
    textureManager = TextureManager(engine);
    materialManager = MaterialManager(engine, textureManager);
}

AssetManager::~AssetManager()
{
}

void AssetManager::AddModel(VulkanEngine& engine, MaterialManager& materailManager, const std::string& FilePath)
{
    modelManager.ModelList.emplace_back(Model(engine, materailManager, textureManager, FilePath));
}

void AssetManager::AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList)
{
    modelManager.ModelList.emplace_back(Model(engine, textureManager, VertexList, IndexList));
}

void AssetManager::AddModel(VulkanEngine& engine, std::vector<Vertex>& VertexList, std::vector<uint32_t>& IndexList, std::shared_ptr<Material> material)
{
    modelManager.ModelList.emplace_back(Model(engine, VertexList, IndexList, material));
}

void AssetManager::UpdateMeshDescriptors(VulkanEngine& engine, VkDescriptorSet& descriptorSet)
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

std::vector<VkDescriptorBufferInfo> AssetManager::GetVertexBufferListDescriptor()
{
    std::vector<VkDescriptorBufferInfo> VertexBufferInfoList;
    for (int x = 0; x < modelManager.ModelList.size(); x++)
    {
        for (int y = 0; y < modelManager.ModelList[x].MeshList.size(); y++)
        {
            VkDescriptorBufferInfo VertexBufferInfo = {};
            VertexBufferInfo.buffer = modelManager.ModelList[x].MeshList[y].VertexBuffer.Buffer;
            VertexBufferInfo.offset = 0;
            VertexBufferInfo.range = VK_WHOLE_SIZE;
            VertexBufferInfoList.emplace_back(VertexBufferInfo);
        }
    }
    return VertexBufferInfoList;
}

std::vector<VkDescriptorBufferInfo> AssetManager::GetIndexBufferListDescriptor()
{
    std::vector<VkDescriptorBufferInfo> IndexBufferInfoList;
    for (int x = 0; x < modelManager.ModelList.size(); x++)
    {
        for (int y = 0; y < modelManager.ModelList[x].MeshList.size(); y++)
        {
            VkDescriptorBufferInfo IndexBufferInfo = {};
            IndexBufferInfo.buffer = modelManager.ModelList[x].MeshList[y].IndexBuffer.Buffer;
            IndexBufferInfo.offset = 0;
            IndexBufferInfo.range = VK_WHOLE_SIZE;
            IndexBufferInfoList.emplace_back(IndexBufferInfo);
        }
    }
    return IndexBufferInfoList;
}

std::vector<VkDescriptorBufferInfo> AssetManager::GetMaterialBufferListDescriptor(std::vector<std::shared_ptr<Material>> MaterialList)
{

    std::vector<VkDescriptorBufferInfo> MaterialBufferList{};
    for (int x = 0; x < MaterialList.size(); x++)
    {
        VkDescriptorBufferInfo MaterialBufferInfo = {};
        MaterialBufferInfo.buffer = MaterialList[x]->MaterialBuffer.Buffer;
        MaterialBufferInfo.offset = 0;
        MaterialBufferInfo.range = VK_WHOLE_SIZE;
        MaterialBufferList.emplace_back(MaterialBufferInfo);
    }

    return MaterialBufferList;
}

std::vector<VkDescriptorBufferInfo> AssetManager::GetMeshDataListDescriptor()
{
    std::vector<VkDescriptorBufferInfo> TransformBufferList{};
    for (int x = 0; x < modelManager.ModelList.size(); x++)
    {
        for (int y = 0; y < modelManager.ModelList[x].MeshList.size(); y++)
        {
            VkDescriptorBufferInfo TransformBufferInfo = {};
            TransformBufferInfo.buffer = modelManager.ModelList[x].MeshList[y].MeshProperties.VulkanBufferData.Buffer;
            TransformBufferInfo.offset = 0;
            TransformBufferInfo.range = VK_WHOLE_SIZE;
            TransformBufferList.emplace_back(TransformBufferInfo);
        }
    }
    return TransformBufferList;
}

std::vector<VkDescriptorBufferInfo> AssetManager::GetTransformBufferListDescriptor()
{
    std::vector<VkDescriptorBufferInfo> TransformBufferList{};
    for (int x = 0; x < modelManager.ModelList.size(); x++)
    {
        for (int y = 0; y < modelManager.ModelList[x].MeshList.size(); y++)
        {
            VkDescriptorBufferInfo TransformBufferInfo = {};
            TransformBufferInfo.buffer = modelManager.ModelList[x].MeshList[y].TransformBuffer.Buffer;
            TransformBufferInfo.offset = 0;
            TransformBufferInfo.range = VK_WHOLE_SIZE;
            TransformBufferList.emplace_back(TransformBufferInfo);
        }
    }
    return TransformBufferList;
}

std::vector<VkDescriptorImageInfo> AssetManager::GetTextureBufferListDescriptor()
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

std::vector<VkDescriptorImageInfo> AssetManager::Get3DTextureBufferListDescriptor()
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

uint32_t AssetManager::GetMeshPropertiesBufferListDescriptorCount()
{
    uint32_t count = 0;
    for (int x = 0; x < modelManager.ModelList.size(); x++)
    {
        for (int y = 0; y < modelManager.ModelList[x].MeshList.size(); y++)
        {
            count++;
        }
    }
    return count++;
}

uint32_t AssetManager::GetVertexBufferListDescriptorCount()
{
    uint32_t count = 0;
    for (int x = 0; x < modelManager.ModelList.size(); x++)
    {
        for (int y = 0; y < modelManager.ModelList[x].MeshList.size(); y++)
        {
            count++;
        }
    }
    return count++;
}

uint32_t AssetManager::GetIndexBufferListDescriptorCount()
{
    uint32_t count = 0;
    for (int x = 0; x < modelManager.ModelList.size(); x++)
    {
        for (int y = 0; y < modelManager.ModelList[x].MeshList.size(); y++)
        {
            count++;
        }
    }
    return count++;
}

uint32_t AssetManager::GetTransformBufferListDescriptorCount()
{
    uint32_t count = 0;
    for (int x = 0; x < modelManager.ModelList.size(); x++)
    {
        for (int y = 0; y < modelManager.ModelList[x].MeshList.size(); y++)
        {
            count++;
        }
    }
    return count++;
}

uint32_t AssetManager::GetMaterialBufferListDescriptorCount(std::vector<std::shared_ptr<Material>> MaterialList)
{
    uint32_t count = 0;
    for (int x = 0; x < MaterialList.size(); x++)
    {
        count++;
    }

    return count;
}

uint32_t AssetManager::GetTextureBufferListDescriptorCount()
{
    return textureManager.GetTextureList().size();
}
