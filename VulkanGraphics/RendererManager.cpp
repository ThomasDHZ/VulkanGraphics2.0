#include "RendererManager.h"
#include <stb_image.h>
#include "BlinnPhongPipeline.h"

RendererManager::RendererManager()
{
}

RendererManager::RendererManager(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    interfaceRenderPass = InterfaceRenderPass(engine);

    BlinnRenderer = BlinnPhongRasterRenderer(engine);
    pbrRenderer = PBRRenderer(EnginePtr::GetEnginePtr());
    ////  renderer2D = Renderer2D(EnginePtr::GetEnginePtr());
    //
    if (EnginePtr::GetEnginePtr()->RayTracingFeatureCompatible)
    {
        rayTraceRenderer = RayTraceRenderer(EnginePtr::GetEnginePtr(), window, AssetManagerPtr::GetAssetPtr());
       // pbrRayTraceRenderer = PBRRayTraceRenderer(EnginePtr::GetEnginePtr(), window, AssetManagerPtr::GetAssetPtr());
    }
}

RendererManager::~RendererManager()
{

}

void RendererManager::RebuildSwapChain(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window->GetWindowPtr(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window->GetWindowPtr(), &width, &height);
        glfwWaitEvents();
    }
    EnginePtr::GetEnginePtr()->ScreenResoulation = glm::ivec2(width, height);


    vkDeviceWaitIdle(engine->Device);

    for (auto imageView : engine->SwapChain.GetSwapChainImageViews()) {
        vkDestroyImageView(engine->Device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(engine->Device, engine->SwapChain.GetSwapChain(), nullptr);

    AssetManagerPtr::GetAssetPtr()->Update();
    engine->SwapChain.RebuildSwapChain(window->GetWindowPtr(), engine->Device, engine->PhysicalDevice, engine->Surface);

    interfaceRenderPass.RebuildSwapChain();
    BlinnRenderer.RebuildSwapChain();
    pbrRenderer.RebuildSwapChain();
    //////renderer2D.RebuildSwapChain();
    
    if (EnginePtr::GetEnginePtr()->RayTracingFeatureCompatible)
    {
        rayTraceRenderer.RebuildSwapChain(engine, window);
       // pbrRayTraceRenderer.RebuildSwapChain(engine, window);
    }
}

void RendererManager::Update(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window, uint32_t currentImage)
{
    if(engine->UpdateRendererFlag)
    {
        RebuildSwapChain(engine, window);
        engine->UpdateRendererFlag = false;
    }

    AssetManagerPtr::GetAssetPtr()->Update();
    BlinnRenderer.Update();
    if (EnginePtr::GetEnginePtr()->RayTracingFeatureCompatible &&
        EnginePtr::GetEnginePtr()->RayTraceFlag)
    {
       rayTraceRenderer.rayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, AssetManagerPtr::GetAssetPtr());
      // pbrRayTraceRenderer.pbrRayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, AssetManagerPtr::GetAssetPtr());
       // hybridRenderer.rayTraceRenderPass.SetUpTopLevelAccelerationStructure(engine, AssetManagerPtr::GetAssetPtr());
    }
}

void RendererManager::GUIUpdate(std::shared_ptr<VulkanEngine> engine)
{
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::SliderInt("Active Renderer", &ActiveRenderer, 0, 2);
    if (EnginePtr::GetEnginePtr()->RayTracingFeatureCompatible)
    {
        ImGui::Checkbox("Ray Tracing:", &EnginePtr::GetEnginePtr()->RayTraceFlag);
    }

    if (ActiveRenderer == 0)
    {
        if (EnginePtr::GetEnginePtr()->RayTraceFlag == false)
        {
            BlinnRenderer.GUIUpdate();
        }
        else
        {
            if (EnginePtr::GetEnginePtr()->RayTracingFeatureCompatible)
            {
                rayTraceRenderer.GUIUpdate(EnginePtr::GetEnginePtr());
            }
        }
    }
    else if (ActiveRenderer == 1)
    {
        if (EnginePtr::GetEnginePtr()->RayTraceFlag == false)
        {
            pbrRenderer.GUIUpdate();
        }
        else
        {
            if (EnginePtr::GetEnginePtr()->RayTracingFeatureCompatible)
            {
               // pbrRayTraceRenderer.GUIUpdate(EnginePtr::GetEnginePtr());
            }
        }
    }
    else if (ActiveRenderer == 2)
    {
        //  renderer2D.GUIUpdate();
    }

    //for (int x = 0; x < MeshManagerPtr::GetMeshManagerPtr()->MeshList.size(); x++)
 //{

    //ImGui::SliderInt("Active Camera", &AssetManagerPtr::GetAssetPtr()->cameraManager->cameraIndex, 0, AssetManagerPtr::GetAssetPtr()->cameraManager->CameraList.size());


    CameraManagerPtr::GetCameraManagerPtr()->CameraList;
    std::vector<const char*> charVec(CameraManagerPtr::GetCameraManagerPtr()->CameraList.size(), nullptr);
    for (int i = 0; i < CameraManagerPtr::GetCameraManagerPtr()->CameraList.size(); i++) {
        charVec[i] = CameraManagerPtr::GetCameraManagerPtr()->CameraList[i]->CameraName.c_str();
    }

    GUIChanged |= ImGui::ListBox("sdfdsf", &selectedCamera, charVec.data(), charVec.size());
    GUIChanged |= ImGui::Checkbox("Shadow Debug", &ShadowDebugFlag);

    ImGui::LabelText("Directional Light", "Directional Light");
    for (int x = 0; x < LightManagerPtr::GetLightManagerPtr()->DirectionalLightList.size(); x++)
    {
        //LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[x]->LightBuffer.UniformDataInfo.direction.x = cos(glm::radians(glfwGetTime() * 360.0f)) * 40.0f;
        //LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[x]->LightBuffer.UniformDataInfo.direction.y = -50.0f + sin(glm::radians(glfwGetTime() * 360.0f)) * 20.0f;
        //LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[x]->LightBuffer.UniformDataInfo.direction.z = 25.0f + sin(glm::radians(glfwGetTime() * 360.0f)) * 5.0f;

        GUIChanged |= ImGui::SliderFloat3(("DLight position " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[x]->LightBuffer.UniformDataInfo.position.x, -100.0f, 2000.0f);
        GUIChanged |= ImGui::SliderFloat3(("DLight direction " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[x]->LightBuffer.UniformDataInfo.direction.x, -180.0f, 180.0f);
        GUIChanged |= ImGui::SliderFloat3(("DLight ambient " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
        GUIChanged |= ImGui::SliderFloat3(("DLight Diffuse " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
        GUIChanged |= ImGui::SliderFloat3(("DLight specular " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
        GUIChanged |= ImGui::SliderFloat2(("DLight LeftRight " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[x]->LeftRight.x, -5000.0f, 5000.0f);
        GUIChanged |= ImGui::SliderFloat2(("DLight TopBottom " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[x]->TopBottom.x, -5000.0f, 5000.0f);
        GUIChanged |= ImGui::SliderFloat2(("DLight NearFar " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->DirectionalLightList[x]->NearFar.x, -5000.0f, 5000.0f);
        ImGui::LabelText("______", "______");
    }

    ImGui::LabelText("Point Light", "Point Light");
    for (int x = 0; x < LightManagerPtr::GetLightManagerPtr()->PointLightList.size(); x++)
    {
        GUIChanged |= ImGui::SliderFloat3(("PLight position " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->PointLightList[x]->LightBuffer.UniformDataInfo.position.x, -2000.0f, 2000.0f);
        GUIChanged |= ImGui::SliderFloat3(("PLight direction " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->PointLightList[x]->cameraDirection.x, -180.0f, 180.0f);
        GUIChanged |= ImGui::SliderFloat3(("PLight ambient " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->PointLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
        GUIChanged |= ImGui::SliderFloat3(("PLight Diffuse " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->PointLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
        GUIChanged |= ImGui::SliderFloat3(("PLight specular " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->PointLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
        GUIChanged |= ImGui::SliderFloat(("PLight constant " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->PointLightList[x]->LightBuffer.UniformDataInfo.constant, 0.0f, 1.0f);
        GUIChanged |= ImGui::SliderFloat(("PLight linear " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->PointLightList[x]->LightBuffer.UniformDataInfo.linear, 0.0f, 1.0f);
        GUIChanged |= ImGui::SliderFloat2(("PLight LeftRight " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->PointLightList[x]->LeftRight.x, -5000.0f, 5000.0f);
        GUIChanged |= ImGui::SliderFloat2(("PLight TopBottom " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->PointLightList[x]->TopBottom.x, -5000.0f, 5000.0f);
        GUIChanged |= ImGui::SliderFloat2(("PLight NearFar " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->PointLightList[x]->NearFar.x, -5000.0f, 5000.0f);
        ImGui::LabelText("______", "______");
    }

    ImGui::LabelText("SpotLight Light", "SpotLight Light");
    for (int x = 0; x < LightManagerPtr::GetLightManagerPtr()->SpotLightList.size(); x++)
    {
        GUIChanged |= ImGui::SliderFloat3(("SLight position " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->SpotLightList[x]->LightBuffer.UniformDataInfo.position.x, -2000.0f, 2000.0f);
        GUIChanged |= ImGui::SliderFloat3(("SLight direction " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->SpotLightList[x]->LightBuffer.UniformDataInfo.direction.x, -360.0f, 360.0f);
        GUIChanged |= ImGui::SliderFloat3(("SLight ambient " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->SpotLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
        GUIChanged |= ImGui::SliderFloat3(("SLight Diffuse " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->SpotLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
        GUIChanged |= ImGui::SliderFloat3(("SLight specular " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->SpotLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
        GUIChanged |= ImGui::SliderFloat(("SLight constant " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->SpotLightList[x]->LightBuffer.UniformDataInfo.constant, 0.0f, 1.0f);
        GUIChanged |= ImGui::SliderFloat(("SLight linear " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->SpotLightList[x]->LightBuffer.UniformDataInfo.linear, 0.0f, 1.0f);
        GUIChanged |= ImGui::SliderFloat(("SLight quadratic " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->SpotLightList[x]->LightBuffer.UniformDataInfo.quadratic, 0.0f, 1.0f);

        ImGui::LabelText("______", "______");
    }

    for (int x = 0; x < MeshManagerPtr::GetMeshManagerPtr()->MeshList.size(); x++)
    {
        GUIChanged |= ImGui::CheckboxFlags(("MainPass " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->DrawFlags, Renderer_Draw_Main_Pass);
        GUIChanged |= ImGui::CheckboxFlags(("ShadowPass " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->DrawFlags, Renderer_Draw_Shadow_Pass);
        GUIChanged |= ImGui::CheckboxFlags(("ReflectionPass " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->DrawFlags, Renderer_Draw_Reflection_Pass);

        //GUIChanged |= ImGui::SliderFloat3(("Albedo " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshMaterial->MaterialTextureData.Albedo.x, .0f, 1.0f);
        //GUIChanged |= ImGui::SliderFloat(("Matallic " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshMaterial->MaterialTextureData.Matallic, 0.0f, 1.0f);
        //GUIChanged |= ImGui::SliderFloat(("Roughness" + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshMaterial->MaterialTextureData.Roughness, 0.0f, 1.0f);
        //GUIChanged |= ImGui::SliderFloat(("AmbientOcclusion " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshMaterial->MaterialTextureData.AmbientOcclusion, 0.0f, 1.0f);
        //GUIChanged |= ImGui::SliderFloat(("minLayers" + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshProperties.UniformDataInfo.minLayers, 0.0f, 3.0f);
        //GUIChanged |= ImGui::SliderFloat(("maxLayers " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshProperties.UniformDataInfo.maxLayers, 0.0f, 3.0f);
        //GUIChanged |= ImGui::SliderFloat(("heightScale " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshProperties.UniformDataInfo.heightScale, 0.0f, 3.0f);
        GUIChanged |= ImGui::SliderFloat3(("Mesh Pos " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshPosition.x, -100.0f, 100.0f);
        GUIChanged |= ImGui::SliderFloat3(("Mesh Rot " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshRotation.x, -360.0f, 360.0f);
        GUIChanged |= ImGui::SliderFloat3(("Mesh Scale " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshScale.x, 0.0f, 3.0f);
        //GUIChanged |= ImGui::SliderFloat2(("UV Ofset" + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshProperties.UniformDataInfo.UVOffset.x, 0.0f, 1.0f);
        //GUIChanged |= ImGui::SliderFloat2(("UV Scale " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->UVScale.x, 0.0f, 2.0f);
        //GUIChanged |= ImGui::SliderFloat2(("UV Flip " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->UVFlip.x, 0.0f, 1.0f);
        //GUIChanged |= ImGui::SliderFloat2(("UV Flip " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->UVFlip.x, 0.0f, 1.0f);
        //GUIChanged |= ImGui::SliderFloat3(("Ambient " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshMaterial->MaterialTextureData.min.x, .0f, 1.0f);
        //GUIChanged |= ImGui::SliderFloat3(("Ambient " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshMaterial->MaterialTextureData.Albedo.x, .0f, 1.0f);
        //GUIChanged |= ImGui::SliderFloat3(("Diffuse " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshMaterial->MaterialTextureData.Diffuse.x, 0.0f, 1.0f);
        //GUIChanged |= ImGui::SliderFloat3(("Specular" + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshMaterial->MaterialTextureData.Specular.x, 0.0f, 1.0f);
        //GUIChanged |= ImGui::SliderFloat(("Reflect " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshMaterial->MaterialTextureData.Reflectivness, 0.0f, 1.0f);
        //GUIChanged |= ImGui::SliderFloat(("Shininess " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshMaterial->MaterialTextureData.Shininess, 0.0f, 255.0f);
        //GUIChanged |= ImGui::SliderFloat(("Alpha " + std::to_string(x)).c_str(), &MeshManagerPtr::GetMeshManagerPtr()->MeshList[x]->MeshMaterial->MaterialTextureData.Alpha, 0.0f, 1.0f);
        ImGui::LabelText("______", "______");
    }

    //ImGui::LabelText("Sphere Light", "Sphere Light");
    //for (int x = 0; x < LightManagerPtr::GetLightManagerPtr()->SphereAreaLightList.size(); x++)
    //{
    //    GUIChanged |= ImGui::SliderFloat3(("Sphere direction " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->SphereAreaLightList[x]->LightBuffer.UniformDataInfo.position.x, -100.0f, 100.0f);
    //    GUIChanged |= ImGui::SliderFloat3(("Sphere ambient " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->SphereAreaLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
    //    GUIChanged |= ImGui::SliderFloat3(("Sphere Diffuse " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->SphereAreaLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
    //    GUIChanged |= ImGui::SliderFloat3(("Sphere specular " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->SphereAreaLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
    //    GUIChanged |= ImGui::SliderFloat(("Sphere Radius " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->SphereAreaLightList[x]->LightBuffer.UniformDataInfo.SphereRadius, 0.0f, 100.0f);
    //    GUIChanged |= ImGui::SliderFloat(("Sphere Alumin " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->SphereAreaLightList[x]->LightBuffer.UniformDataInfo.Luminosity, 0.0f, 100000.0f);
    //    ImGui::LabelText("______", "______");
    //}

    ////ImGui::LabelText("Tube Light", "Tube Light");
    ////for (int x = 0; x < LightManagerPtr::GetLightManagerPtr()->TubeAreaLightList.size(); x++)
    ////{
    ////    GUIChanged |= ImGui::SliderFloat3(("Tube StartPos " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->TubeAreaLightList[x]->LightBuffer.UniformDataInfo.StartPos.x, -100.0f, 100.0f);
    ////    GUIChanged |= ImGui::SliderFloat3(("Tube EndPos " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->TubeAreaLightList[x]->LightBuffer.UniformDataInfo.EndPos.x, -100.0f, 100.0f);
    ////    GUIChanged |= ImGui::SliderFloat3(("Tube ambient " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->TubeAreaLightList[x]->LightBuffer.UniformDataInfo.ambient.x, 0.0f, 1.0f);
    ////    GUIChanged |= ImGui::SliderFloat3(("Tube Diffuse " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->TubeAreaLightList[x]->LightBuffer.UniformDataInfo.diffuse.x, 0.0f, 1.0f);
    ////    GUIChanged |= ImGui::SliderFloat3(("Tube specular " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->TubeAreaLightList[x]->LightBuffer.UniformDataInfo.specular.x, 0.0f, 1.0f);
    ////    GUIChanged |= ImGui::SliderFloat(("Tube Radius " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->TubeAreaLightList[x]->LightBuffer.UniformDataInfo.TubeRadius, 0.0f, INT32_MAX);
    ////    GUIChanged |= ImGui::SliderFloat(("Tube Alumin " + std::to_string(x)).c_str(), &LightManagerPtr::GetLightManagerPtr()->TubeAreaLightList[x]->LightBuffer.UniformDataInfo.Luminosity, 0.0f, 100000.0f);
    ////    ImGui::LabelText("______", "______");
    ////}

    if (GUIChanged)
    {
        CameraManagerPtr::GetCameraManagerPtr()->SetActiveCamera(selectedCamera);
        MaterialManagerPtr::GetMaterialManagerPtr()->UpdateBufferIndex();
        rayTraceRenderer.rayTraceRenderPass.Frame = 0;
        GUIChanged = false;
    }

   /* guiRenderer.GUIUpdate();*/
}

void RendererManager::Draw(std::shared_ptr<VulkanEngine> engine, std::shared_ptr<VulkanWindow> window)
{
    EnginePtr::GetEnginePtr()->CMDIndex = (EnginePtr::GetEnginePtr()->CMDIndex + 1) % MAX_FRAMES_IN_FLIGHT;

    vkWaitForFences(EnginePtr::GetEnginePtr()->Device, 1, &EnginePtr::GetEnginePtr()->inFlightFences[EnginePtr::GetEnginePtr()->CMDIndex], VK_TRUE, UINT64_MAX);
    vkResetFences(EnginePtr::GetEnginePtr()->Device, 1, &EnginePtr::GetEnginePtr()->inFlightFences[EnginePtr::GetEnginePtr()->CMDIndex]);

    VkResult result = vkAcquireNextImageKHR(EnginePtr::GetEnginePtr()->Device, EnginePtr::GetEnginePtr()->SwapChain.GetSwapChain(), UINT64_MAX, EnginePtr::GetEnginePtr()->AcquireImageSemaphores[EnginePtr::GetEnginePtr()->CMDIndex], VK_NULL_HANDLE, &EnginePtr::GetEnginePtr()->ImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RebuildSwapChain(engine, window);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
    {
        throw std::runtime_error("Failed to acquire swap chain image!");
    }

    Update(engine, window, EnginePtr::GetEnginePtr()->CMDIndex);

    std::vector<VkCommandBuffer> CommandBufferSubmitList;

    AssetManagerPtr::GetAssetPtr()->ObjManager->SubmitAnimationToCommandBuffer(CommandBufferSubmitList);
    if (ActiveRenderer == 0)
    {
        if (EnginePtr::GetEnginePtr()->RayTraceFlag == false)
        {
            BlinnRenderer.Draw();
            BlinnRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
        }
        else
        {
            if (EnginePtr::GetEnginePtr()->RayTracingFeatureCompatible)
            {
                rayTraceRenderer.Draw(engine, window);
                rayTraceRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
            }
        }
    }
    else if (ActiveRenderer == 1)
    {
        if (EnginePtr::GetEnginePtr()->RayTraceFlag == false)
        {
            pbrRenderer.Draw();
            pbrRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
        }
        else
        {
            if (EnginePtr::GetEnginePtr()->RayTracingFeatureCompatible)
            {
               // pbrRayTraceRenderer.Draw(engine, window);
               // pbrRayTraceRenderer.AddToCommandBufferSubmitList(CommandBufferSubmitList);
            }
        }
    }
    else if (ActiveRenderer == 2)
    {
   /*     EnginePtr::GetEnginePtr()->RayTraceFlag = false;
        renderer2D.Draw();
        renderer2D.AddToCommandBufferSubmitList(CommandBufferSubmitList);*/
    }

    interfaceRenderPass.Draw();
    CommandBufferSubmitList.emplace_back(interfaceRenderPass.ImGuiCommandBuffers[EnginePtr::GetEnginePtr()->CMDIndex]);

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &EnginePtr::GetEnginePtr()->AcquireImageSemaphores[EnginePtr::GetEnginePtr()->CMDIndex];
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = static_cast<uint32_t>(CommandBufferSubmitList.size());
    submitInfo.pCommandBuffers = CommandBufferSubmitList.data();
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &EnginePtr::GetEnginePtr()->PresentImageSemaphores[EnginePtr::GetEnginePtr()->ImageIndex];
    VkResult a = vkQueueSubmit(EnginePtr::GetEnginePtr()->GraphicsQueue, 1, &submitInfo, EnginePtr::GetEnginePtr()->inFlightFences[EnginePtr::GetEnginePtr()->CMDIndex]);
    if (a != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &EnginePtr::GetEnginePtr()->PresentImageSemaphores[EnginePtr::GetEnginePtr()->ImageIndex];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &EnginePtr::GetEnginePtr()->SwapChain.Swapchain;
    presentInfo.pImageIndices = &EnginePtr::GetEnginePtr()->ImageIndex;
    result = vkQueuePresentKHR(EnginePtr::GetEnginePtr()->PresentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RebuildSwapChain(engine, window);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
    {
        throw std::runtime_error("Failed to present swap chain image!");
    }
}

void RendererManager::Destroy(std::shared_ptr<VulkanEngine> engine)
{
    interfaceRenderPass.Destroy();
    BlinnRenderer.Destroy();
    pbrRenderer.Destroy();

    if (EnginePtr::GetEnginePtr()->RayTracingFeatureCompatible)
    {
        rayTraceRenderer.Destroy(engine);
       // pbrRayTraceRenderer.Destroy(engine);
    }
    //renderer2D.Destroy();
}