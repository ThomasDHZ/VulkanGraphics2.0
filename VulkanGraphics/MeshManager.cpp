#include "MeshManager.h"
//#include "WaterSurfaceMesh.h"
//#include "BillboardMesh.h"
//#include "Level2D.h"
//#include "Sprite.h"

std::shared_ptr<MeshManager> MeshManagerPtr::meshManager = nullptr;

MeshManager::MeshManager()
{
}

MeshManager::MeshManager(std::shared_ptr<VulkanEngine> Engine, std::shared_ptr<InputManager> inputmanager, std::shared_ptr<MaterialManager> materialmanager)
{
    engine = Engine;
    inputManager = inputmanager;
    materialManager = materialmanager;
}

MeshManager::~MeshManager()
{
}

void MeshManager::AddMesh(std::shared_ptr<Mesh> mesh)
{
	MeshList.emplace_back(mesh);
    EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
    UpdateBufferIndex();
}

void MeshManager::DeleteMesh(uint32_t MeshBufferIndex)
{
    MeshList[MeshBufferIndex]->Destory();
    MeshList.erase(MeshList.begin() + MeshBufferIndex);
    EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
    UpdateBufferIndex();
}

void MeshManager::Update(std::shared_ptr<Camera> camera)
{
    for (auto& mesh : MeshList)
    {
        if (mesh->ParentModelID == 0)
        {
            switch (mesh->MeshType)
            {
            case MeshTypeFlag::Mesh_Type_Normal:  mesh->Update(); break;
            //case MeshTypeFlag::Mesh_Type_2D_Sprite: static_cast<Sprite*>(mesh.get())->Update(engine, inputManager, materialManager, timer, MeshList); break;
           // case MeshTypeFlag::Mesh_Type_Billboard: static_cast<BillboardMesh*>(mesh.get())->Update(camera); break;
            default: mesh->Update();
            };
           
        }
    }
}

void MeshManager::UpdateBufferIndex()
{
	for (int x = 0; x < MeshList.size(); x++)
	{
		MeshList[x]->MeshBufferIndex = x;
	}
}

void MeshManager::Draw(VkCommandBuffer& commandBuffer, VkPipelineLayout& ShaderLayout, std::shared_ptr<Camera> CameraView)
{
    for (auto& mesh : MeshList)
    {
        if (mesh->DrawFlags == MeshDrawFlags::Mesh_Draw_All)
        {
            mesh->Draw(commandBuffer, ShaderLayout, CameraView);
        }
    }
}

void MeshManager::Destroy()
{
    for (auto mesh : MeshList)
    {
        mesh->Destory();
    }
}

std::vector<VkDescriptorBufferInfo> MeshManager::GetVertexBufferListDescriptors()
{
    std::vector<VkDescriptorBufferInfo> VertexBufferInfoList;
    if (MeshList.size() == 0)
    {
        VkDescriptorBufferInfo nullBuffer;
        nullBuffer.buffer = VK_NULL_HANDLE;
        nullBuffer.offset = 0;
        nullBuffer.range = VK_WHOLE_SIZE;
        VertexBufferInfoList.emplace_back(nullBuffer);
    }
    else
    {
        for (auto& mesh : MeshList)
        {
            VkDescriptorBufferInfo VertexBufferInfo = {};
            VertexBufferInfo.buffer = mesh->VertexBuffer.Buffer;
            VertexBufferInfo.offset = 0;
            VertexBufferInfo.range = VK_WHOLE_SIZE;
            VertexBufferInfoList.emplace_back(VertexBufferInfo);
        }
    }
    return VertexBufferInfoList;
}

std::vector<VkDescriptorBufferInfo> MeshManager::GetIndexBufferListDescriptors()
{
    std::vector<VkDescriptorBufferInfo> IndexBufferInfoList;
    if (MeshList.size() == 0)
    {
        VkDescriptorBufferInfo nullBuffer;
        nullBuffer.buffer = VK_NULL_HANDLE;
        nullBuffer.offset = 0;
        nullBuffer.range = VK_WHOLE_SIZE;
        IndexBufferInfoList.emplace_back(nullBuffer);
    }
    else
    {
        for (auto& mesh : MeshList)
        {
            VkDescriptorBufferInfo IndexBufferInfo = {};
            IndexBufferInfo.buffer = mesh->IndexBuffer.Buffer;
            IndexBufferInfo.offset = 0;
            IndexBufferInfo.range = VK_WHOLE_SIZE;
            IndexBufferInfoList.emplace_back(IndexBufferInfo);
        }
    }
    return IndexBufferInfoList;
}

std::vector<VkDescriptorBufferInfo> MeshManager::GetTransformBufferListDescriptors()
{
    std::vector<VkDescriptorBufferInfo> TransformBufferList{};
    if (MeshList.size() == 0)
    {
        VkDescriptorBufferInfo nullBuffer;
        nullBuffer.buffer = VK_NULL_HANDLE;
        nullBuffer.offset = 0;
        nullBuffer.range = VK_WHOLE_SIZE;
        TransformBufferList.emplace_back(nullBuffer);
    }
    else
    {
        for (auto& mesh : MeshList)
        {
            VkDescriptorBufferInfo TransformBufferInfo = {};
            TransformBufferInfo.buffer = mesh->TransformBuffer.Buffer;
            TransformBufferInfo.offset = 0;
            TransformBufferInfo.range = VK_WHOLE_SIZE;
            TransformBufferList.emplace_back(TransformBufferInfo);
        }
    }
    return TransformBufferList;
}

std::vector<VkDescriptorBufferInfo> MeshManager::GetMeshPropertiesListDescriptors()
{
    std::vector<VkDescriptorBufferInfo> MeshPropertiesmBufferList{};
    if (MeshList.size() == 0)
    {
        VkDescriptorBufferInfo nullBuffer;
        nullBuffer.buffer = VK_NULL_HANDLE;
        nullBuffer.offset = 0;
        nullBuffer.range = VK_WHOLE_SIZE;
        MeshPropertiesmBufferList.emplace_back(nullBuffer);
    }
    else
    {
        for (auto& mesh : MeshList)
        {
            VkDescriptorBufferInfo MeshPropertiesmBufferBufferInfo = {};
            MeshPropertiesmBufferBufferInfo.buffer = mesh->MeshProperties.VulkanBufferData.Buffer;
            MeshPropertiesmBufferBufferInfo.offset = 0;
            MeshPropertiesmBufferBufferInfo.range = VK_WHOLE_SIZE;
            MeshPropertiesmBufferList.emplace_back(MeshPropertiesmBufferBufferInfo);
        }
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

uint32_t MeshManager::GetMeshDescriptorCount()
{
    if (MeshList.size() > 0)
    {
        return MeshList.size();
    }
    else
    {
        return 1;
    }
}
