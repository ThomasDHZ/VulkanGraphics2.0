#include "SpotLight.h"

SpotLight::SpotLight() : Light<SpotLightBuffer>()
{
}

SpotLight::SpotLight(VulkanEngine& engine) : Light<SpotLightBuffer>(engine)
{
}

SpotLight::SpotLight(VulkanEngine& engine, SpotLightBuffer light) : Light<SpotLightBuffer>(engine)
{
	LightBuffer.UniformDataInfo = light;
	Update(engine);
}

SpotLight::~SpotLight()
{
}
