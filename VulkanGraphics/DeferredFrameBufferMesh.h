#pragma once
#include "Mesh.h"
#include "FrameBufferMesh.h"

struct SSAOConfig
{
   alignas(4)  int KernelSize = 64;
   alignas(4)  float Radius = 0.5f;
   alignas(4)  float Bias = 0.025f;
};

class DeferredFrameBufferMesh : public Mesh
{
private:
    std::shared_ptr<Texture> GBufferPositionTexture;
    std::shared_ptr<Texture> GBufferNormalTexture;
    std::shared_ptr<Texture> NoiseTexture;

    VulkanUniformBuffer ssAOConfig;

    void CreateUniformBuffers(VulkanEngine& engine) override;
    void CreateDescriptorPool(VulkanEngine& engine) override;
    void CreateDescriptorSets(VulkanEngine& engine, VkDescriptorSetLayout& layout) override;

public:

    DeferredFrameBufferMesh();
    DeferredFrameBufferMesh(VulkanEngine& engine, std::shared_ptr<Texture> gBufferPosition, std::shared_ptr<Texture> gBufferNormal, VkDescriptorSetLayout layout);
    ~DeferredFrameBufferMesh();

    SSAOConfig settings;

    void Update(VulkanEngine& renderer);
    void UpdateSwapChain(VulkanEngine& engine, std::shared_ptr<Texture> FrameBufferImage, VkDescriptorSetLayout& layout);
    void UpdateSwapChain(VulkanEngine& engine, std::shared_ptr<TextureManager> textureManager, VkDescriptorSetLayout& layout, std::shared_ptr<Texture> FrameBufferImage, std::shared_ptr<Texture> BloomImage);
    void UpdateSwapChain(VulkanEngine& engine, std::shared_ptr<TextureManager>textureManager, VkDescriptorSetLayout& layout, std::shared_ptr<Texture> FrameBufferImage, std::shared_ptr<Texture> BloomImage, int effectRenderer, std::shared_ptr<GraphicsPipeline> shader);
    void Destory(VulkanEngine& engine) override;
};

