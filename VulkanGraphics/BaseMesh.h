#pragma once
#include <vector>
#include <memory>
#include "Vertex.h"
#include "VulkanEngine.h"
#include "GraphicsPipeline.h"
#include "VertexBuffer.h"
#include "IndicesBuffer.h"
#include "Camera.h"
#include "CubeMapTexture.h"
#include "TextureManager.h"
#include "VulkanUniformBuffer.h"
#include "Bone.h"
#include "DrawMesh.h"

const  std::string DefaultTexture = "C:/Users/dotha/source/repos/VulkanGraphics/texture/DefaultTexture.png";

enum RenderDrawFlags
{
    RenderNormally = 1 << 0,
    RenderAnimated = 1 << 1,
    RenderWireFrame = 1 << 2,
    RenderWireFrameAnimated = 1 << 3,
    RenderShadow = 1 << 4,
    RenderShadowAnimated = 1 << 5,
    RenderLightDebug = 1 << 6
};

struct MeshTextures
{
    std::string DiffuseMap = DefaultTexture;
    std::string SpecularMap = DefaultTexture;
    std::string NormalMap = DefaultTexture;
    std::string DepthMap = DefaultTexture;
    std::string AlphaMap = DefaultTexture;
    std::string EmissionMap = DefaultTexture;
    std::string ReflectionMap = DefaultTexture;
    std::string CubeMap[6];

    std::shared_ptr<Texture> RendererDiffuseMap;
    std::shared_ptr<Texture> RendererSpecularMap;
    std::shared_ptr<Texture> RendererNormalMap;
    std::shared_ptr<Texture> RendererDepthMap;
    std::shared_ptr<Texture> RendererAlphaMap;
    std::shared_ptr<Texture> RendererEmissionMap;
    std::shared_ptr<Texture> RendererReflectionMap;
    std::shared_ptr<CubeMapTexture> RendererCubeMap;
};

struct Material
{
    alignas(16) glm::vec3 ambient = glm::vec3(1.0f, 0.0f, 0.0f);
    alignas(16) glm::vec3 diffuse = glm::vec3(0.0f, 1.0f, 0.0f);
    alignas(16) glm::vec3 specular = glm::vec3(0.0f, 0.0f, 1.0f);
    alignas(4) float shininess = 32;
    alignas(4) float reflectivness = 0;
};

struct MeshProperties
{
    alignas(16) Material material;
};

struct MeshData
{
    unsigned int MeshID;
    unsigned int NodeID;
    std::string NodeName;
    std::vector<Vertex> VertexList;
    std::vector<uint16_t> IndexList;
    MeshTextures TextureList;
    glm::mat4 TransformMatrix;
    MeshProperties properties;
};

class BaseMesh : public DrawMesh
{
protected:


    VertexBuffer MeshVertex;
    IndicesBuffer MeshIndices;

    VkDescriptorPool DescriptorPool;
    std::vector<VkDescriptorSet> DescriptorSets;

    std::shared_ptr<Texture> DiffuseTexture;
    std::shared_ptr<Texture> SpecularTexture;
    std::shared_ptr<Texture> NormalTexture;
    std::shared_ptr<Texture> DepthTexture;
    std::shared_ptr<Texture> AlphaTexture;
    std::shared_ptr<Texture> EmissionTexture;
    std::shared_ptr<Texture> ReflectionTexture;
    std::shared_ptr<CubeMapTexture> SkyBoxTexture;

    bool MeshDeletedFlag = false;

    void LoadTextures(VulkanEngine& engine, std::shared_ptr<TextureManager> textureManager, MeshTextures textures);
    void CreateDescriptorPool(VulkanEngine& engine, std::vector<VkDescriptorPoolSize> DescriptorPoolInfo);
    void CreateDescriptorSets(VulkanEngine& engine, VkDescriptorSetLayout layout);
    void CreateDescriptorSetsData(VulkanEngine& engine, std::vector<VkWriteDescriptorSet> DescriptorInfoList);

    VkDescriptorPoolSize AddDsecriptorPoolBinding(VulkanEngine& engine, VkDescriptorType descriptorType);
    VkDescriptorImageInfo AddImageDescriptorInfo(VulkanEngine& engine, VkImageLayout ImageLayout, std::shared_ptr<Texture> texture);
    VkWriteDescriptorSet AddDescriptorSetTextureInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorImageInfo& TextureImageInfo);
    VkDescriptorBufferInfo AddBufferDescriptorInfo(VulkanEngine& engine, VkBuffer Buffer, VkDeviceSize BufferSize);
    VkWriteDescriptorSet AddDescriptorSetBufferInfo(VulkanEngine& engine, unsigned int BindingNumber, VkDescriptorSet& DescriptorSet, VkDescriptorBufferInfo& BufferInfo);

public:

    glm::vec3 MeshPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 MeshRotate = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 MeshScale = glm::vec3(1.0f);

    BaseMesh();
    BaseMesh(VulkanEngine& engine, const MeshData& meshData, int renderFlags);
    BaseMesh(VulkanEngine& engine, const std::vector<Vertex>& Vertexdata, const std::vector<uint16_t>& Indicesdata, int renderFlags);
    BaseMesh(VulkanEngine& engine, const std::vector<Vertex>& Vertexdata, int renderFlags);
    ~BaseMesh();

    unsigned int RenderFlags = 0;

    virtual void Draw(VkCommandBuffer& RenderCommandBuffer, std::shared_ptr<GraphicsPipeline> pipeline, int FrameNumber);
    virtual void Update(VulkanEngine& engine);
    virtual void Destory(VulkanEngine& engine);

    void SetRenderFlags(int renderFlags);
    void SetPosition2D(glm::vec2 Pos);
    void SetPosition2D(float x, float y);
    void SetPosition3D(glm::vec3 Pos);
    void SetPosition3D(float x, float y, float z);
    void SetRotation2D(glm::vec2 Rotation);
    void SetRotation2D(float x, float y);
    void SetRotation3D(glm::vec3 Rotation);
    void SetRotation3D(float x, float y, float z);
    void SetScale2D(glm::vec2 Scale);
    void SetScale2D(float x, float y);
    void SetScale3D(glm::vec3 Scale);
    void SetScale3D(float x, float y, float z);

    glm::vec2 GetPosition2D() { return glm::vec2(MeshPosition.x, MeshPosition.y); }
    glm::vec3 GetPosition3D() { return MeshPosition; }
    glm::vec2 GetRotation2D() { return glm::vec2(MeshRotate.x, MeshRotate.y); }
    glm::vec3 GetRotation3D() { return MeshRotate; }
    glm::vec2 GetScale2D() { return glm::vec2(MeshScale.x, MeshScale.y); }
    glm::vec3 GetScale3D() { return MeshScale; }
    int GetRenderFlags() { return RenderFlags; }
    bool GetMeshDeletedFlag() { return MeshDeletedFlag; }
};