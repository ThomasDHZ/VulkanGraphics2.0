#include "DeferredFrameBufferMesh.h"

DeferredFrameBufferMesh::DeferredFrameBufferMesh() : Mesh()
{}

DeferredFrameBufferMesh::DeferredFrameBufferMesh(VulkanEngine& engine, std::shared_ptr<Texture> gBufferPosition, std::shared_ptr<Texture> gBufferNormal, VkDescriptorSetLayout layout) : Mesh(engine, FrameBufferVertices, FrameBufferIndices, RenderDrawFlags::RenderNormally)
{
    GBufferPositionTexture = gBufferPosition;
    GBufferNormalTexture = gBufferNormal;
    NoiseTexture = gBufferNormal;

    CreateUniformBuffers(engine);
    CreateDescriptorPool(engine);
    CreateDescriptorSets(engine, layout);
}

DeferredFrameBufferMesh::~DeferredFrameBufferMesh()
{
}

void DeferredFrameBufferMesh::CreateUniformBuffers(VulkanEngine& renderer)
{
    ssAOConfig = VulkanUniformBuffer(renderer, sizeof(SSAOConfig));
}

void DeferredFrameBufferMesh::CreateDescriptorPool(VulkanEngine& engine)
{
    std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};
    DescriptorPoolList.emplace_back(AddDsecriptorPoolBinding(engine, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));
    DescriptorPoolList.emplace_back(AddDsecriptorPoolBinding(engine, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));
    DescriptorPoolList.emplace_back(AddDsecriptorPoolBinding(engine, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));
    DescriptorPoolList.emplace_back(AddDsecriptorPoolBinding(engine, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    BaseMesh::CreateDescriptorPool(engine, DescriptorPoolList);
}

void DeferredFrameBufferMesh::CreateDescriptorSets(VulkanEngine& engine, VkDescriptorSetLayout& layout)
{
    BaseMesh::CreateDescriptorSets(engine, layout);

    VkDescriptorImageInfo gBufferPosition = AddImageDescriptorInfo(engine, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, GBufferPositionTexture);
    VkDescriptorImageInfo gBufferNormal = AddImageDescriptorInfo(engine, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, GBufferNormalTexture);
    VkDescriptorImageInfo BufferNoiseTexture = AddImageDescriptorInfo(engine, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, NoiseTexture);

    for (size_t i = 0; i < engine.SwapChain.GetSwapChainImageCount(); i++)
    {
        VkDescriptorBufferInfo SSAOConfigBuffer = AddBufferDescriptorInfo(engine, ssAOConfig.GetUniformBuffer(i), sizeof(SSAOConfig));

        std::vector<VkWriteDescriptorSet> DescriptorList;
        DescriptorList.emplace_back(AddDescriptorSetTextureInfo(engine, 0, DescriptorSets[i], gBufferPosition));
        DescriptorList.emplace_back(AddDescriptorSetTextureInfo(engine, 1, DescriptorSets[i], gBufferNormal));
        DescriptorList.emplace_back(AddDescriptorSetTextureInfo(engine, 2, DescriptorSets[i], BufferNoiseTexture));
        DescriptorList.emplace_back(AddDescriptorSetBufferInfo(engine, 3, DescriptorSets[i], SSAOConfigBuffer));
        BaseMesh::CreateDescriptorSetsData(engine, DescriptorList);
    }
}

void DeferredFrameBufferMesh::Update(VulkanEngine& renderer, SSAOConfig& ssaoSettings)
{
    ssAOConfig.UpdateUniformBuffer(renderer, static_cast<void*>(&ssaoSettings));
}

void DeferredFrameBufferMesh::UpdateSwapChain(VulkanEngine& engine, std::shared_ptr<Texture> FrameBufferImage, VkDescriptorSetLayout& layout)
{
    vkDestroyDescriptorPool(engine.Device, DescriptorPool, nullptr);
    DescriptorPool = VK_NULL_HANDLE;

    DiffuseTexture = FrameBufferImage;

    CreateDescriptorPool(engine);
    CreateDescriptorSets(engine, layout);
}

void DeferredFrameBufferMesh::UpdateSwapChain(VulkanEngine& engine, std::shared_ptr<TextureManager> textureManager, VkDescriptorSetLayout& layout, std::shared_ptr<Texture> FrameBufferImage, std::shared_ptr<Texture> BloomImage)
{
    vkDestroyDescriptorPool(engine.Device, DescriptorPool, nullptr);
    DescriptorPool = VK_NULL_HANDLE;

    DiffuseTexture = FrameBufferImage;
    EmissionTexture = BloomImage;

    CreateDescriptorPool(engine);
    CreateDescriptorSets(engine, layout);
}

void DeferredFrameBufferMesh::UpdateSwapChain(VulkanEngine& engine, std::shared_ptr<TextureManager> textureManager, VkDescriptorSetLayout& layout, std::shared_ptr<Texture> FrameBufferImage, std::shared_ptr<Texture> BloomImage, int effectRenderer, std::shared_ptr<GraphicsPipeline> shader)
{
    vkDestroyDescriptorPool(engine.Device, DescriptorPool, nullptr);
    DescriptorPool = VK_NULL_HANDLE;

    DiffuseTexture = FrameBufferImage;
    EmissionTexture = BloomImage;

    CreateDescriptorPool(engine);
    CreateDescriptorSets(engine, layout);
}

void DeferredFrameBufferMesh::UpdateSwapChain(VulkanEngine& engine, std::shared_ptr<Texture> gBufferPosition, std::shared_ptr<Texture> gBufferNormal, VkDescriptorSetLayout layout)
{
    vkDestroyDescriptorPool(engine.Device, DescriptorPool, nullptr);
    DescriptorPool = VK_NULL_HANDLE;

    GBufferPositionTexture = gBufferPosition;
    GBufferNormalTexture = gBufferNormal;
    NoiseTexture = gBufferNormal;

    CreateDescriptorPool(engine);
    CreateDescriptorSets(engine, layout);
}

void DeferredFrameBufferMesh::Destory(VulkanEngine& engine)
{
    ssAOConfig.Destroy(engine);
    BaseMesh::Destory(engine);
}