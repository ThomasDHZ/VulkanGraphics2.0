#include "PBRRenderer.h"

PBRRenderer::PBRRenderer() : BaseRenderer()
{
}

PBRRenderer::PBRRenderer(std::shared_ptr<VulkanEngine> engine) : BaseRenderer()
{
    ////  DebugDepthRenderPass = DepthDebugRenderPass(DepthRenderPass.DepthTextureList[0]);
// equirectangularToCubemapRenderPass = EquirectangularToCubemapRenderPass();

    brdfRenderPass = BRDFRenderPass(CubeMapSamplerSize);

    //Shadow Pass
    {
        DepthRenderPass = DepthPassRendererPass(512);
        depthCubeMapRenderPass = DepthCubeMapRenderPass(1024);
    }
    //Reflection Pass
    {
        ReflectionIrradianceRenderPass = IrradianceRenderPass(TextureManagerPtr::GetTextureManagerPtr()->GetAllCubeMapTextures()[0], CubeMapSamplerSize);
        ReflectionPrefilterRenderPass = PrefilterRenderPass(TextureManagerPtr::GetTextureManagerPtr()->GetAllCubeMapTextures()[0], CubeMapSamplerSize);

        reflectionPBRPass = PBRReflectionRenderPass(glm::ivec2(512), ReflectionIrradianceRenderPass.RenderedCubeMap, ReflectionPrefilterRenderPass.RenderedCubeMap, brdfRenderPass.BRDFMap, DepthRenderPass.DepthTextureList);
    }
    //Main Render Pass
    {
        irradianceRenderPass = IrradianceRenderPass(reflectionPBRPass.RenderedTexture, CubeMapSamplerSize);
        prefilterRenderPass = PrefilterRenderPass(reflectionPBRPass.RenderedTexture, CubeMapSamplerSize);

        pbrRenderer = PBRRenderPass(engine, irradianceRenderPass.RenderedCubeMap, prefilterRenderPass.RenderedCubeMap, brdfRenderPass.BRDFMap, DepthRenderPass.DepthTextureList);
        FrameBufferRenderer = FrameBufferRenderPass(pbrRenderer.RenderedTexture, pbrRenderer.RenderedBloomTexture);
    }
}

PBRRenderer::~PBRRenderer()
{
}

void PBRRenderer::RebuildSwapChain()
{
    brdfRenderPass.RebuildSwapChain(CubeMapSamplerSize);

    //Shadow Pass
    {
        DepthRenderPass.RebuildSwapChain(512);
        depthCubeMapRenderPass.RebuildSwapChain(1024);
    }
    //Reflection Pass
    {
        ReflectionIrradianceRenderPass.RebuildSwapChain(TextureManagerPtr::GetTextureManagerPtr()->GetAllCubeMapTextures()[0], CubeMapSamplerSize);
        ReflectionPrefilterRenderPass.RebuildSwapChain(TextureManagerPtr::GetTextureManagerPtr()->GetAllCubeMapTextures()[0], CubeMapSamplerSize);

        reflectionPBRPass.RebuildSwapChain(glm::ivec2(512), ReflectionIrradianceRenderPass.RenderedCubeMap, ReflectionPrefilterRenderPass.RenderedCubeMap, brdfRenderPass.BRDFMap, DepthRenderPass.DepthTextureList);
    }
    //Main Render Pass
    {
       irradianceRenderPass.RebuildSwapChain(reflectionPBRPass.RenderedTexture, CubeMapSamplerSize);
       prefilterRenderPass.RebuildSwapChain(reflectionPBRPass.RenderedTexture, CubeMapSamplerSize);

        pbrRenderer.RebuildSwapChain(irradianceRenderPass.RenderedCubeMap, prefilterRenderPass.RenderedCubeMap, brdfRenderPass.BRDFMap, DepthRenderPass.DepthTextureList);
        FrameBufferRenderer.RebuildSwapChain(pbrRenderer.RenderedTexture, pbrRenderer.RenderedBloomTexture);
    }
}

void PBRRenderer::GUIUpdate()
{
    
    const char* afdsfas[]{"asdf", "Fads", "wqer", "adsfaer34"};
    GUIChangedFlag |= ImGui::ListBox("sdfdsf", &selected, afdsfas, IM_ARRAYSIZE(afdsfas));
    ImGui::SliderFloat3("reflect Pos " , &reflectionPBRPass.reflectPos.x, -5.0f, 5.0f);
    ImGui::SliderFloat2("XNearFar ", &reflectionPBRPass.XNearFar.x, -5.0f, 5.0f);
    ImGui::SliderFloat2("YNearFar ", &reflectionPBRPass.YNearFar.x, -5.0f, 5.0f);
    ImGui::SliderFloat2("ZNearFar ", &reflectionPBRPass.ZNearFar.x, -5.0f, 5.0f);
    GUIChangedFlag |= ImGui::Checkbox("Shadow Debug", &ShadowDebugFlag);

    if (GUIChangedFlag)
    {
        CameraManagerPtr::GetCameraManagerPtr()->SetActiveCamera(selected);

        RebuildSwapChain();
        GUIChangedFlag = false;
    }
}

void PBRRenderer::Draw()
{
    brdfRenderPass.Draw();

    //Shadow Pass
    {
        DepthRenderPass.Draw();
        depthCubeMapRenderPass.Draw();
    }
    //Reflection Pass
    {
        ReflectionIrradianceRenderPass.Draw();
        ReflectionPrefilterRenderPass.Draw();

        reflectionPBRPass.Draw();
    }
    //Main Render Pass
    {
        irradianceRenderPass.Draw();
        prefilterRenderPass.Draw();

        pbrRenderer.Draw();
        FrameBufferRenderer.Draw();
    }
}

void PBRRenderer::Destroy()
{
    brdfRenderPass.Destroy();

    //Shadow Pass
    {
        DepthRenderPass.Destroy();
        depthCubeMapRenderPass.Destroy();
    }
    //Reflection Pass
    {
        ReflectionIrradianceRenderPass.Destroy();
        ReflectionPrefilterRenderPass.Destroy();

        reflectionPBRPass.Destroy();
    }
    //Main Render Pass
    {
        irradianceRenderPass.Destroy();
        // prefilterRenderPass = PrefilterRenderPass(reflectionPBRPass.RenderedTexture, CubeMapSamplerSize);

        pbrRenderer.Destroy();
        FrameBufferRenderer.Destroy();
    }
}

std::vector<VkCommandBuffer> PBRRenderer::AddToCommandBufferSubmitList(std::vector<VkCommandBuffer>& CommandBufferSubmitList)
{

    //Shadow Pass
    {
        CommandBufferSubmitList.emplace_back(DepthRenderPass.GetCommandBuffer());
        CommandBufferSubmitList.emplace_back(depthCubeMapRenderPass.GetCommandBuffer());
    }
    //Reflection Pass
    {
        CommandBufferSubmitList.emplace_back(ReflectionIrradianceRenderPass.GetCommandBuffer());

        CommandBufferSubmitList.emplace_back(reflectionPBRPass.GetCommandBuffer());
    }
    //Main Render Pass
    {
        CommandBufferSubmitList.emplace_back(irradianceRenderPass.GetCommandBuffer());

        CommandBufferSubmitList.emplace_back(pbrRenderer.GetCommandBuffer());
        CommandBufferSubmitList.emplace_back(FrameBufferRenderer.GetCommandBuffer());
    }

    return CommandBufferSubmitList;
}