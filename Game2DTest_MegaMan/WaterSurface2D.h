//#pragma once
//#include "Sprite.h"
//
//struct WaveProperites
//{
//	alignas(16) glm::vec3 WaterColor = glm::vec3(0.0f, 0.78431f, 1.0f);
//	alignas(8) glm::vec2 Distortion_scale = glm::vec2(.389f, .053f);
//	alignas(4) float intensity = 00.3f;
//	alignas(4)float speed = 0.085f;
//	alignas(4) float waveAmp = 0.238f;
//	alignas(4) float waveSpeed = 4.0f;
//	alignas(4) float wavePeriod = 2.423f;
//};
//
//class WaterSurface2D : public Sprite
//{
//private:
//	void DrawMessage(RendererManager& renderer) override;
//public:
//	WaterSurface2D();
//	WaterSurface2D(RendererManager& renderer, std::shared_ptr<TextureManager>textureManager, glm::vec2 StartPos, glm::vec2 WaterSize, std::shared_ptr<Texture> texture);
//	~WaterSurface2D();
//
//	WaveProperites waveprop;
//};
//
