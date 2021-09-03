#include "SpotLight.h"

SpotLight::SpotLight() : Light<SpotLightBuffer>()
{
}

SpotLight::SpotLight(std::shared_ptr<VulkanEngine> engine) : Light<SpotLightBuffer>(engine)
{
}

SpotLight::SpotLight(SpotLightBuffer light) : Light<SpotLightBuffer>(EnginePtr::GetEnginePtr())
{
	LightBuffer.UniformDataInfo = light;
	Update();
}

SpotLight::~SpotLight()
{
}
