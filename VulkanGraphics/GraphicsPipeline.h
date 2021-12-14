#pragma once
#include "Vulkanengine.h"

struct ConstMeshBase
{
	alignas(4) uint32_t MeshIndex;
};

struct RayTraceConstants
{
	alignas(16) glm::mat4 proj = glm::mat4(1.0f);
	alignas(16) glm::mat4 view = glm::mat4(1.0f);
	alignas(16) glm::vec3 CameraPos = glm::vec3(0.0f);
	alignas(4) uint32_t frame = 0;
	alignas(4) int      AntiAliasingCount = 100;
	alignas(4) int      MaxRefeflectCount = 15;
	alignas(4) int	    ApplyAntiAliasing = 0;
	alignas(4) float TestRoughness = 0.000000001f;
	alignas(4) float SampleRate = .025;
};

struct ConstMeshInfo
{
	alignas(4) uint32_t MeshIndex;
	alignas(16) glm::mat4 proj = glm::mat4(1.0f);
	alignas(16) glm::mat4 view = glm::mat4(1.0f);
	alignas(16) glm::vec3 CameraPos = glm::vec3(0.0f);
};

struct PrefilterConst
{
	alignas(16) glm::mat4 proj = glm::mat4(1.0f);
	alignas(16) glm::mat4 view = glm::mat4(1.0f);
	alignas(4) float MipLevel = 0.0f;
	alignas(4) uint32_t SampleCount = 1024;
	alignas(4) float FaceSize = 0.0f;
};

struct ConstSkyBoxView
{
	alignas(16) glm::mat4 proj = glm::mat4(1.0f);
	alignas(16) glm::mat4 view = glm::mat4(1.0f);
};

struct PrefilterSkybox
{
	alignas(4) uint32_t SkyboxSize = 0.0f;
	alignas(4) float roughness = 0.0f;
};

struct ConstBloomProperites
{
	alignas(4) uint32_t BloomPass = 0;
};

struct LightSceneInfo : public ConstMeshBase
{
	alignas(16) glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
};

struct DescriptorSetBindingStruct
{
	uint32_t DescriptorSlotNumber;
	VkShaderStageFlags StageFlags;
	VkDescriptorType DescriptorType;
	VkWriteDescriptorSetAccelerationStructureKHR AccelerationStructureDescriptor;
	std::vector<VkDescriptorImageInfo> TextureDescriptor;
	std::vector<VkDescriptorBufferInfo> BufferDescriptor;
	uint32_t Count;
};


class GraphicsPipeline
{
private:
	std::vector<VkDescriptorPoolSize>  DescriptorPoolList{};
	std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo{};
	std::vector<VkWriteDescriptorSet> DescriptorList{};
	std::vector<DescriptorSetBindingStruct> DescriptorBindingList{};

	VkWriteDescriptorSet AddAccelerationBuffer(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkWriteDescriptorSetAccelerationStructureKHR& accelerationStructure);
	VkWriteDescriptorSet AddTextureDescriptorSet(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo, VkDescriptorType descriptorType);
	VkWriteDescriptorSet AddTextureDescriptorSet(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorImageInfo>& TextureImageInfo, VkDescriptorType descriptorType);
	VkWriteDescriptorSet AddBufferDescriptorSet(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo, VkDescriptorType descriptorType);
	VkWriteDescriptorSet AddBufferDescriptorSet(unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, std::vector<VkDescriptorBufferInfo>& BufferInfoList, VkDescriptorType descriptorType);

protected:
	VkSampler NullSampler = VK_NULL_HANDLE;
	VkDescriptorImageInfo nullBufferInfo;

public:
	VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet DescriptorSet = VK_NULL_HANDLE;
	VkPipelineLayout ShaderPipelineLayout = VK_NULL_HANDLE;
	VkPipeline ShaderPipeline = VK_NULL_HANDLE;

	GraphicsPipeline();
	~GraphicsPipeline();

	void AddDescriptorSetBinding(unsigned int BindingNumber, VkWriteDescriptorSetAccelerationStructureKHR& accelerationStructure, VkShaderStageFlags StageFlags);
	void AddDescriptorSetBinding(unsigned int BindingNumber, VkDescriptorImageInfo& TextureImageInfo, VkShaderStageFlags StageFlags, VkDescriptorType BufferType);
	void AddDescriptorSetBinding(unsigned int BindingNumber, std::vector<VkDescriptorImageInfo>& TextureImageInfo, VkShaderStageFlags StageFlags, VkDescriptorType BufferType, uint32_t DescriptorCount);
	void AddDescriptorSetBinding(unsigned int BindingNumber, VkDescriptorBufferInfo& BufferInfo, VkDescriptorType BufferType, VkShaderStageFlags StageFlags);
	void AddDescriptorSetBinding(unsigned int BindingNumber, std::vector<VkDescriptorBufferInfo>& BufferInfo, VkDescriptorType BufferType, VkShaderStageFlags StageFlags, uint32_t DescriptorCount);
	void SubmitDescriptorSet();

	virtual void UpdateGraphicsPipeLine();
	virtual void Destroy();
};

