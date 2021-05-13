#include "BloomPipeline.h"
#include "Vertex.h"

BloomPipeline::BloomPipeline() : GraphicsPipeline()
{
}

BloomPipeline::BloomPipeline(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, std::shared_ptr<Texture> BloomTexture) : GraphicsPipeline()
{
	SetUpDescriptorPool(engine, assetManager);
	SetUpDescriptorLayout(engine, assetManager);
	SetUpShaderPipeLine(engine, renderPass);
	SetUpDescriptorSets(engine, assetManager, BloomTexture);
}

BloomPipeline::~BloomPipeline()
{
}

void BloomPipeline::SetUpDescriptorPool(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager)
{
	std::vector<VkDescriptorPoolSize>  DescriptorPoolList = {};
	DescriptorPoolList.emplace_back(engine.AddDsecriptorPoolBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1));
	DescriptorPool = engine.CreateDescriptorPool(DescriptorPoolList);
}

void BloomPipeline::SetUpDescriptorLayout(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager)
{
	std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
	LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1 });
	DescriptorSetLayout = engine.CreateDescriptorSetLayout(LayoutBindingInfo);
}

void BloomPipeline::SetUpDescriptorSets(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, std::shared_ptr<Texture> BloomTexture)
{
	DescriptorSets = engine.CreateDescriptorSets(DescriptorPool, DescriptorSetLayout);
	VkDescriptorImageInfo BloomTextureBuffer = engine.AddTextureDescriptor(BloomTexture->View, BloomTexture->Sampler);

	std::vector<VkWriteDescriptorSet> DescriptorList;
	DescriptorList.emplace_back(engine.AddTextureDescriptorSet(0, DescriptorSets, BloomTextureBuffer));

	vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(DescriptorList.size()), DescriptorList.data(), 0, nullptr);
}

void BloomPipeline::SetUpShaderPipeLine(VulkanEngine& engine, const VkRenderPass& renderPass)
{
	std::vector<VkPipelineShaderStageCreateInfo> PipelineShaderStageList;
	PipelineShaderStageList.emplace_back(engine.CreateShader("Shader/BloomShaderVert.spv", VK_SHADER_STAGE_VERTEX_BIT));
	PipelineShaderStageList.emplace_back(engine.CreateShader("Shader/BloomShaderFrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT));

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)engine.SwapChain.GetSwapChainResolution().width;
	viewport.height = (float)engine.SwapChain.GetSwapChainResolution().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = engine.SwapChain.GetSwapChainResolution();

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState ColorAttachment;
	ColorAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	ColorAttachment.blendEnable = VK_TRUE;
	ColorAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	ColorAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	ColorAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	ColorAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	ColorAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	ColorAttachment.alphaBlendOp = VK_BLEND_OP_SUBTRACT;

	int ColorAttachmentCount = 1;
	std::vector<VkPipelineColorBlendAttachmentState> ColorAttachmentList(ColorAttachmentCount, ColorAttachment);

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = static_cast<uint32_t>(ColorAttachmentList.size());
	colorBlending.pAttachments = ColorAttachmentList.data();
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(ConstBloomProperites);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &DescriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	if (vkCreatePipelineLayout(engine.Device, &pipelineLayoutInfo, nullptr, &ShaderPipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = static_cast<uint32_t>(PipelineShaderStageList.size());
	pipelineInfo.pStages = PipelineShaderStageList.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = ShaderPipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	if (vkCreateGraphicsPipelines(engine.Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &ShaderPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	for (auto& shader : PipelineShaderStageList)
	{
		vkDestroyShaderModule(engine.Device, shader.module, nullptr);
	}
}

void BloomPipeline::UpdateGraphicsPipeLine(VulkanEngine& engine, std::shared_ptr<AssetManager> assetManager, const VkRenderPass& renderPass, std::shared_ptr<Texture> BloomTexture)
{
	GraphicsPipeline::UpdateGraphicsPipeLine(engine, renderPass);
	SetUpDescriptorPool(engine, assetManager);
	SetUpDescriptorLayout(engine, assetManager);
	SetUpShaderPipeLine(engine, renderPass);
	SetUpDescriptorSets(engine, assetManager, BloomTexture);
}