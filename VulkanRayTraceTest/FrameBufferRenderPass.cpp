//#include "FrameBufferRenderPass.h"
//
//FrameBufferRenderPass::FrameBufferRenderPass()
//{
//}
//
//FrameBufferRenderPass::FrameBufferRenderPass(VulkanEngine& engine, std::shared_ptr<Texture> texture)
//{
//    CreateRenderPass(engine);
//    CreateRendererFramebuffers(engine);
//    CreateDescriptorSetLayout(engine);
//    frameBufferPipeline = std::make_shared<FrameBufferRenderingPipeline>(engine, RenderPass, descriptorLayout);
//    CreateDescriptorPool(engine);
//    CreateDescriptorSets(engine, texture);
//    mesh = Mesh(engine, FrameBufferVertices, FrameBufferIndices, 1);
//}
//
//FrameBufferRenderPass::~FrameBufferRenderPass()
//{
//}
//
//void FrameBufferRenderPass::CreateDescriptorSetLayout(VulkanEngine& engine)
//{
//    std::vector<DescriptorSetLayoutBindingInfo> LayoutBindingInfo = {};
//    LayoutBindingInfo.emplace_back(DescriptorSetLayoutBindingInfo{ 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT });
//    descriptorLayout = frameBufferPipeline->CreateDescriptorSetLayout(engine, LayoutBindingInfo);
//}
//
//void FrameBufferRenderPass::CreateDescriptorPool(VulkanEngine& engine)
//{
//    std::array<VkDescriptorPoolSize, 1> poolSizes{};
//    poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//    poolSizes[0].descriptorCount = static_cast<uint32_t>(engine.SwapChain.SwapChainImages.size());
//
//    VkDescriptorPoolCreateInfo poolInfo{};
//    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
//    poolInfo.pPoolSizes = poolSizes.data();
//    poolInfo.maxSets = static_cast<uint32_t>(engine.SwapChain.SwapChainImages.size());
//
//    if (vkCreateDescriptorPool(engine.Device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create descriptor pool!");
//    }
//}
//
//void FrameBufferRenderPass::CreateDescriptorSets(VulkanEngine& engine, std::shared_ptr<Texture> texture)
//{
//    std::vector<VkDescriptorSetLayout> layouts(engine.SwapChain.SwapChainImages.size(), descriptorLayout);
//    VkDescriptorSetAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//    allocInfo.descriptorPool = descriptorPool;
//    allocInfo.descriptorSetCount = static_cast<uint32_t>(engine.SwapChain.SwapChainImages.size());
//    allocInfo.pSetLayouts = layouts.data();
//
//    descriptorSets.resize(engine.SwapChain.SwapChainImages.size());
//    if (vkAllocateDescriptorSets(engine.Device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
//        throw std::runtime_error("failed to allocate descriptor sets!");
//    }
//
//    for (size_t i = 0; i < engine.SwapChain.SwapChainImages.size(); i++)
//    {
//
//        std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
//
//        VkDescriptorImageInfo DiffuseMapImage = {};
//        DiffuseMapImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//        DiffuseMapImage.imageView = texture->GetTextureView();
//        DiffuseMapImage.sampler = texture->GetTextureSampler();
//
//        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//        descriptorWrites[0].dstSet = descriptorSets[i];
//        descriptorWrites[0].dstBinding = 0;
//        descriptorWrites[0].dstArrayElement = 0;
//        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//        descriptorWrites[0].descriptorCount = 1;
//        descriptorWrites[0].pImageInfo = &DiffuseMapImage;
//
//        vkUpdateDescriptorSets(engine.Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
//    }
//}
//
//void FrameBufferRenderPass::CreateRenderPass(VulkanEngine& engine)
//{
//    VkAttachmentDescription colorAttachment{};
//    colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
//    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
//    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//    VkAttachmentReference colorAttachmentRef{};
//    colorAttachmentRef.attachment = 0;
//    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//    VkSubpassDescription subpass{};
//    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//    subpass.colorAttachmentCount = 1;
//    subpass.pColorAttachments = &colorAttachmentRef;
//
//    VkSubpassDependency dependency{};
//    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//    dependency.dstSubpass = 0;
//    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//    dependency.srcAccessMask = 0;
//    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//
//    std::array<VkAttachmentDescription, 1> attachments = { colorAttachment };
//    VkRenderPassCreateInfo renderPassInfo{};
//    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
//    renderPassInfo.pAttachments = attachments.data();
//    renderPassInfo.subpassCount = 1;
//    renderPassInfo.pSubpasses = &subpass;
//    renderPassInfo.dependencyCount = 1;
//    renderPassInfo.pDependencies = &dependency;
//
//    if (vkCreateRenderPass(engine.Device, &renderPassInfo, nullptr, &RenderPass) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create render pass!");
//    }
//}
//
//void FrameBufferRenderPass::CreateRendererFramebuffers(VulkanEngine& engine)
//{
//    SwapChainFramebuffers.resize(engine.SwapChain.GetSwapChainImageCount());
//
//    for (size_t i = 0; i < engine.SwapChain.GetSwapChainImageCount(); i++) {
//        std::array<VkImageView, 1> attachments = {
//            engine.SwapChain.GetSwapChainImageViews()[i]
//        };
//
//        VkFramebufferCreateInfo framebufferInfo{};
//        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//        framebufferInfo.renderPass = RenderPass;
//        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
//        framebufferInfo.pAttachments = attachments.data();
//        framebufferInfo.width = engine.SwapChain.GetSwapChainResolution().width;
//        framebufferInfo.height = engine.SwapChain.GetSwapChainResolution().height;
//        framebufferInfo.layers = 1;
//
//        if (vkCreateFramebuffer(engine.Device, &framebufferInfo, nullptr, &SwapChainFramebuffers[i]) != VK_SUCCESS) {
//            throw std::runtime_error("failed to create framebuffer!");
//        }
//    }
//}
//
//void FrameBufferRenderPass::Draw(VulkanEngine& engine, VkCommandBuffer commandbuffer, uint32_t index)
//{
//    std::array<VkClearValue, 2> clearValues{};
//    clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
//    clearValues[1].depthStencil = { 1.0f, 0 };
//
//    VkRenderPassBeginInfo renderPassInfo{};
//    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//    renderPassInfo.renderPass = RenderPass;
//    renderPassInfo.framebuffer = SwapChainFramebuffers[index];
//    renderPassInfo.renderArea.offset = { 0, 0 };
//    renderPassInfo.renderArea.extent = engine.SwapChain.GetSwapChainResolution();
//    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
//    renderPassInfo.pClearValues = clearValues.data();
//
//    VkBuffer vertexBuffers[] = { mesh.VertexBuffer.Buffer };
//    VkDeviceSize offsets[] = { 0 };
//
//    vkCmdBeginRenderPass(commandbuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
//    vkCmdBindPipeline(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, frameBufferPipeline->ShaderPipeline);
//    vkCmdBindVertexBuffers(commandbuffer, 0, 1, vertexBuffers, offsets);
//    vkCmdBindDescriptorSets(commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, frameBufferPipeline->ShaderPipelineLayout, 0, 1, &descriptorSets[index], 0, nullptr);
//    vkCmdBindIndexBuffer(commandbuffer, mesh.IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
//    vkCmdDrawIndexed(commandbuffer, static_cast<uint32_t>(mesh.IndexCount), 1, 0, 0, 0);
//    vkCmdEndRenderPass(commandbuffer);
//}
//
//void FrameBufferRenderPass::UpdateSwapChain(VulkanEngine& engine)
//{
//    frameBufferPipeline->UpdateGraphicsPipeLine(engine, RenderPass, descriptorLayout);
//
//    vkDestroyRenderPass(engine.Device, RenderPass, nullptr);
//    RenderPass = VK_NULL_HANDLE;
//
//    for (auto& framebuffer : SwapChainFramebuffers)
//    {
//        vkDestroyFramebuffer(engine.Device, framebuffer, nullptr);
//        framebuffer = VK_NULL_HANDLE;
//    }
//
//    CreateRenderPass(engine);
//    CreateRendererFramebuffers(engine);
//}
//
//void FrameBufferRenderPass::Destroy(VulkanEngine& engine)
//{
//    frameBufferPipeline->Destroy(engine);
//
//    vkDestroyRenderPass(engine.Device, RenderPass, nullptr);
//    RenderPass = VK_NULL_HANDLE;
//
//    for (auto& framebuffer : SwapChainFramebuffers)
//    {
//        vkDestroyFramebuffer(engine.Device, framebuffer, nullptr);
//        framebuffer = VK_NULL_HANDLE;
//    }
//}