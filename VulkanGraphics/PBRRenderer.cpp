#include "PBRRenderer.h"

PBRRenderer::PBRRenderer() : BaseRenderer()
{
}

PBRRenderer::PBRRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    cubeMapRenderer = CubeMapRenderPass(512.0f);
    prefilterRenderPass = PrefilterRenderPass(512.0f);
    brdfRenderPass = BRDFRenderPass(cubeMapRenderer.BlurredSkyBoxTexture);
    FrameBufferTextureRenderer = PBRFrameBufferTextureRenderPass(cubeMapRenderer.BlurredSkyBoxTexture, prefilterRenderPass.BlurredSkyBoxTexture, brdfRenderPass.BRDFTexture);
    //DebugDepthRenderer = DepthDebugRenderPass(FrameBufferTextureRenderer.DepthTexture);
    FrameBufferRenderer = FrameBufferRenderPass(FrameBufferTextureRenderer.RenderedTexture, FrameBufferTextureRenderer.BloomTexture);
}

PBRRenderer::~PBRRenderer()
{
}

void PBRRenderer::RebuildSwapChain()
{
    cubeMapRenderer.RebuildSwapChain();
    prefilterRenderPass.RebuildSwapChain();
    brdfRenderPass.RebuildSwapChain(cubeMapRenderer.BlurredSkyBoxTexture);
    FrameBufferTextureRenderer.RebuildSwapChain(cubeMapRenderer.BlurredSkyBoxTexture, prefilterRenderPass.BlurredSkyBoxTexture, brdfRenderPass.BRDFTexture);
   // DebugDepthRenderer.RebuildSwapChain(FrameBufferTextureRenderer.DepthTexture);
    FrameBufferRenderer.RebuildSwapChain(FrameBufferTextureRenderer.RenderedTexture, FrameBufferTextureRenderer.BloomTexture);
}

void PBRRenderer::GUIUpdate()
{
 //   ImGui::Image(FrameBufferTextureRenderer.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
 //   ImGui::Image(FrameBufferTextureRenderer.BloomTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
 ////   ImGui::Image(DebugDepthRenderer.DebugDepthTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
 //   ImGui::Image(cubeMapRenderer.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
 //   ImGui::Image(prefilterRenderPass.RenderedTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
 //   ImGui::Image(brdfRenderPass.BRDFTexture->ImGuiDescriptorSet, ImVec2(180.0f, 180.0f));
}

void PBRRenderer::Draw()
{
    FrameBufferTextureRenderer.Draw();
   //DebugDepthRenderer.Draw();
    FrameBufferRenderer.Draw();
}

void PBRRenderer::Destroy()
{
    cubeMapRenderer.Destroy();
    prefilterRenderPass.Destroy();
    brdfRenderPass.Destroy();
    FrameBufferTextureRenderer.Destroy();
 //   DebugDepthRenderer.Destroy();
    FrameBufferRenderer.Destroy();
}

std::vector<VkCommandBuffer> PBRRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{
    CommandBufferSubmitList.emplace_back(FrameBufferTextureRenderer.GetCommandBuffer());
    CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    return CommandBufferSubmitList;
}
