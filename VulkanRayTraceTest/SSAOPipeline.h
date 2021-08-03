#pragma once
#include "Vulkanengine.h"
#include "GraphicsPipeline.h"
#include "AssetManager.h"


struct SSAOProperties
{
	alignas(16) glm::mat4 projection;
	alignas(4)  int kernelSize;
	alignas(4)  float radius;
	alignas(4)  float bias;
	alignas(4)  float TextureWidth;
	alignas(4)  float TextureHeight;
};

struct SSAOTextureList
{
	std::shared_ptr<Texture> GPositionTexture;
	std::shared_ptr<Texture> GNormalTexture;
	std::shared_ptr<Texture> NormalMapTexture;
	std::shared_ptr<Texture> NoiseTexture;
	std::vector<std::shared_ptr<VulkanBuffer>> KernalSampleBufferList;
};

class SSAOPipeline : public GraphicsPipeline
{
private:
	void SetUpDescriptorPool(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, SSAOTextureList& textures);
	void SetUpDescriptorLayout(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, SSAOTextureList& textures);
	void SetUpDescriptorSets(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, SSAOTextureList& textures);
	void SetUpShaderPipeLine(std::shared_ptr<VulkanEngine> engine, const VkRenderPass& renderPass);
	std::vector<VkDescriptorBufferInfo> GetKernallBufferListDescriptor(std::vector<std::shared_ptr<VulkanBuffer>> SamplePoints);
public:
	SSAOPipeline();
	SSAOPipeline(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, SSAOTextureList& textures);
	~SSAOPipeline();

	void UpdateGraphicsPipeLine(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, SSAOTextureList& textures);
};

