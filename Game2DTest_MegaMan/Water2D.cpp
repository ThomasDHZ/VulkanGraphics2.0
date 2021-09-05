//#include "Water2D.h"
//
//Water2D::Water2D() : TextureRenderedSprite()
//{
//}
//
//Water2D::Water2D(RendererManager& renderer, std::shared_ptr<TextureManager> textureManager, glm::vec2 StartPos, glm::vec2 WaterSize, const std::shared_ptr<Camera>& camera) : TextureRenderedSprite(renderer)
//{
//	CustomBuffer custom = {};
//	custom.ByteSize = sizeof(WaveProperites);
//
//	const std::vector<Vertex> Water2DVertices =
//	{
//		{{0.0f, 0.0f, 0.0f},				 {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
//		{{WaterSize.x, 0.0f, 0.0f},			 {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
//		{{WaterSize.x, WaterSize.y, 0.0f},   {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
//		{{0.0f, WaterSize.y, 0.0f},		     {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}}
//	};
//
//	MeshTextures CoinTextures = {};
//	CoinTextures.DiffuseMap = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/SparkManAlpha.bmp";
//	CoinTextures.SpecularMap = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/SparkManAlpha.bmp";
//	CoinTextures.NormalMap = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/SparkManAlpha.bmp";
//	CoinTextures.EmissionMap = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/SparkManAlpha.bmp";
//	CoinTextures.AlphaMap = "C:/Users/dotha/source/repos/VulkanGraphics/VulkanGraphics/texture/SparkManAlpha.bmp";
//
//	RenderBitFlags = RenderBitFlag::RenderOnTexturePass | RenderBitFlag::RenderOnMainPass | RenderBitFlag::RenderMainPass | RenderBitFlag::RenderEffectPass;
//	ObjectFlagBits = ObjectFlags::None;
//
//	WaterCamera = camera;
//	SetUpSprite(renderer, textureManager, Water2DVertices, CoinTextures, StartPos, custom);
//}
//
//Water2D::~Water2D()
//{
//}
//
//void Water2D::DrawMessage(RendererManager& renderer)
//{
//	ObjectMesh->CreateDrawMessage(renderer, 1, renderer.forwardRenderer.renderer2DPipeline);
//}
//
