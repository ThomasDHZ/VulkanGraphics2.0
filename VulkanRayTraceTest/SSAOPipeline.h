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
	void SetUpDescriptorPool(SSAOTextureList& textures);
	void SetUpDescriptorLayout(SSAOTextureList& textures);
	void SetUpDescriptorSets(SSAOTextureList& textures);
	void SetUpShaderPipeLine(const VkRenderPass& renderPass);
	std::vector<VkDescriptorBufferInfo> GetKernallBufferListDescriptor(std::vector<std::shared_ptr<VulkanBuffer>> SamplePoints);
public:
	SSAOPipeline();
	SSAOPipeline(const VkRenderPass& renderPass, SSAOTextureList& textures);
	~SSAOPipeline();

	void UpdateGraphicsPipeLine(const VkRenderPass& renderPass, SSAOTextureList& textures);
};

