#include "PointLight.h"

PointLight::PointLight() : Light<PointLightBuffer>()
{
}

PointLight::PointLight(PointLightBuffer light) : Light<PointLightBuffer>(EnginePtr::GetEnginePtr())
{
	LightBuffer.UniformDataInfo = light;
	lightViewCamera = std::make_shared<OrthographicLightViewCamera>(OrthographicLightViewCamera(LightBuffer.UniformDataInfo.position));
	Update();
}

PointLight::~PointLight()
{
}

void PointLight::Update()
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	LightBuffer.UniformDataInfo.position.x = sin(glfwGetTime()) * 3.0f;
	LightBuffer.UniformDataInfo.position.z = cos(glfwGetTime()) * 2.0f;
	LightBuffer.UniformDataInfo.position.y = 5.0 + cos(glfwGetTime()) * 1.0f;


	Light::Update();
	lightViewCamera->Update(LightBuffer.UniformDataInfo.position);
	LightBuffer.UniformDataInfo.lightSpaceMatrix = lightViewCamera->GetLightSpaceMatrix();
}

void PointLight::Destroy()
{
	Light::Destroy();
}