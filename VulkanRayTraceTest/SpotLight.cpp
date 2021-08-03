#include "SpotLight.h"

SpotLight::SpotLight() : Light<SpotLightBuffer>()
{
}

SpotLight::SpotLight(std::shared_ptr<VulkanEngine> engine) : Light<SpotLightBuffer>(engine)
{
}

SpotLight::SpotLight(std::shared_ptr<VulkanEngine> engine, SpotLightBuffer light) : Light<SpotLightBuffer>(engine)
{
	LightBuffer.UniformDataInfo = light;
	Update(engine);
}

SpotLight::~SpotLight()
{
}
