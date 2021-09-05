//#include "WaterSurface2D.h"
//
//WaterSurface2D::WaterSurface2D() : Sprite()
//{
//}
//
//WaterSurface2D::WaterSurface2D(RendererManager& renderer, std::shared_ptr<TextureManager> textureManager, glm::vec2 StartPos, glm::vec2 WaterSize, std::shared_ptr<Texture> texture) : Sprite()
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
//	CoinTextures.RendererDiffuseMap = texture;
//	CoinTextures.SpecularMap = "texture/coin_specularOriginal.bmp";
//	CoinTextures.NormalMap = "texture/fn03a-default-noise-1024x720.jpg";
//	CoinTextures.AlphaMap = "texture/SparkManAlpha.bmp";
//
//	RenderBitFlags = RenderBitFlag::RenderOnMainPass | RenderBitFlag::RenderMainPass;
//	ObjectFlagBits = ObjectFlags::None;
//
//	SetUpSprite(renderer, textureManager, Water2DVertices, CoinTextures, StartPos, custom);
//}
//
//WaterSurface2D::~WaterSurface2D()
//{
//}
//
//void WaterSurface2D::DrawMessage(RendererManager& renderer)
//{
//	ObjectMesh->CreateDrawMessage(renderer, 1, renderer.forwardRenderer.reflection2DPipeline);
//}
