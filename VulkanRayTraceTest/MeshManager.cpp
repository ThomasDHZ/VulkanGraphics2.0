#include "MeshManager.h"
#include "WaterSurfaceMesh.h"
#include "BillboardMesh.h"
#include "Level2D.h"
#include "Sprite.h"

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
}

void MeshManager::AddMesh(std::shared_ptr<Mesh> mesh)
{
	MeshList.emplace_back(mesh);
}

void MeshManager::Update(VulkanEngine& engine, InputManager& inputManager, MaterialManager& materialManager, std::shared_ptr<Camera> camera)
{
    for (auto& mesh : MeshList)
    {
        if (mesh->ParentModelID == 0)
        {
            switch (mesh->MeshType)
            {
            case MeshTypeFlag::Mesh_Type_Normal:  mesh->Update(engine, inputManager, materialManager); break;
            //case MeshTypeFlag::Mesh_Type_2D_Sprite: static_cast<Sprite*>(mesh.get())->Update(engine, inputManager, materialManager, timer, MeshList); break;
            case MeshTypeFlag::Mesh_Type_Billboard: static_cast<BillboardMesh*>(mesh.get())->Update(engine, inputManager, materialManager, camera); break;
            default: mesh->Update(engine, inputManager, materialManager);
            };
           
        }
    }
    UpdateBufferIndex(engine);
}

void MeshManager::UpdateBufferIndex(VulkanEngine& engine)
{
	for (int x = 0; x < MeshList.size(); x++)
	{
		MeshList[x]->MeshBufferIndex = x;
	}
}

void MeshManager::Draw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, VkPipelineLayout layout, RenderPassID RendererID, std::shared_ptr<Camera> CameraView)
{
    for (auto& mesh : MeshList)
    {
        if (mesh->DrawFlags == MeshDrawFlags::Mesh_Draw_All)
        {
            mesh->Draw(commandBuffer, layout, RendererID, CameraView);
        }
        else if(mesh->DrawFlags == MeshDrawFlags::Mesh_Skip_Water_Renderer)
        {
            auto water = static_cast<WaterSurfaceMesh*>(mesh.get());
            water->Draw(commandBuffer, renderPassInfo, RendererID, 1);
        }
    }
}

void MeshManager::GUIDraw(VkCommandBuffer& commandBuffer, VkRenderPassBeginInfo& renderPassInfo, VkPipelineLayout layout)
{
    for (auto& mesh : GUIMeshList)
    {
        mesh->Draw(commandBuffer, layout);
    }
}

void MeshManager::Destroy(VulkanEngine& engine)
{
    for (auto mesh : MeshList)
    {
        mesh->Destory(engine);
    }
}

std::vector<VkDescriptorBufferInfo> MeshManager::GetVertexBufferListDescriptors()
{
    std::vector<VkDescriptorBufferInfo> VertexBufferInfoList;
    for (auto& mesh : MeshList)
    {
        VkDescriptorBufferInfo VertexBufferInfo = {};
        VertexBufferInfo.buffer = mesh->VertexBuffer.Buffer;
        VertexBufferInfo.offset = 0;
        VertexBufferInfo.range = VK_WHOLE_SIZE;
        VertexBufferInfoList.emplace_back(VertexBufferInfo);
    }
    return VertexBufferInfoList;
}

std::vector<VkDescriptorBufferInfo> MeshManager::GetIndexBufferListDescriptors()
{
    std::vector<VkDescriptorBufferInfo> IndexBufferInfoList;
    for (auto& mesh : MeshList)
    {
        VkDescriptorBufferInfo IndexBufferInfo = {};
        IndexBufferInfo.buffer = mesh->IndexBuffer.Buffer;
        IndexBufferInfo.offset = 0;
        IndexBufferInfo.range = VK_WHOLE_SIZE;
        IndexBufferInfoList.emplace_back(IndexBufferInfo);
    }
    return IndexBufferInfoList;
}

std::vector<VkDescriptorBufferInfo> MeshManager::GetTransformBufferListDescriptors()
{
    std::vector<VkDescriptorBufferInfo> TransformBufferList{};
    for (auto& mesh : MeshList)
    {
        VkDescriptorBufferInfo TransformBufferInfo = {};
        TransformBufferInfo.buffer = mesh->TransformBuffer.Buffer;
        TransformBufferInfo.offset = 0;
        TransformBufferInfo.range = VK_WHOLE_SIZE;
        TransformBufferList.emplace_back(TransformBufferInfo);
    }
    return TransformBufferList;
}

std::vector<VkDescriptorBufferInfo> MeshManager::GetMeshPropertiesListDescriptors()
{
    std::vector<VkDescriptorBufferInfo> MeshPropertiesmBufferList{};
    for (auto& mesh : MeshList)
    {
        VkDescriptorBufferInfo MeshPropertiesmBufferBufferInfo = {};
        MeshPropertiesmBufferBufferInfo.buffer = mesh->MeshProperties.VulkanBufferData.Buffer;
        MeshPropertiesmBufferBufferInfo.offset = 0;
        MeshPropertiesmBufferBufferInfo.range = VK_WHOLE_SIZE;
        MeshPropertiesmBufferList.emplace_back(MeshPropertiesmBufferBufferInfo);
    }
    return MeshPropertiesmBufferList;
}

std::shared_ptr<Mesh> MeshManager::GetMeshIndex(uint32_t MeshID)
{
    std::shared_ptr<Mesh> meshptr;
    for (auto& mesh : MeshList)
    {
        if (MeshID == mesh->MeshID)
        {
            meshptr = mesh;
        }
    }

    return meshptr;
}