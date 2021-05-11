#pragma once
#include "VulkanEngine.h"
#include "GraphicsPipeline.h"
#include "AssetManager.h"


struct SSAOProperties
{
	alignas(16) glm::mat4 projection;
	alignas(4)  uint32_t kernelSize;
	alignas(4)  float radius;
	alignas(4)  float bias;
	alignas(4)  float TextureWidth;
	alignas(4)  float TextureHeight;
};

struct SSAOTextureList
{
	std::shared_ptr<Texture> GPositionTexture;
	std::shared_ptr<Texture> GNormalTexture;
	std::shared_ptr<Texture> NoiseTexture;
	std::vector<VulkanBuffer> KernalSampleBufferList;
};

class SSAOPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, SSAOTextureList& textures);
	void SetUpDescriptorLayout(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, SSAOTextureList& textures);
	void SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, SSAOTextureList& textures);
	void SetUpShaderPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass);
	std::vector<VkDescriptorBufferInfo> GetKernallBufferListDescriptor(std::vector<VulkanBuffer>& SamplePoints);
public:
	SSAOPipeline();
	SSAOPipeline(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, SSAOTextureList& textures);
	~SSAOPipeline();

	void UpdateGraphicsPipeLine(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, SSAOTextureList& textures);
};

