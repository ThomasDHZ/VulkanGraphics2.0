#pragma once
#include "Vulkanengine.h"
#include "SceneData.h"

template <class T>
class Light
{
private:
protected:


public:
	UniformData<T> LightBuffer;
	Light()
	{

	}
	
	Light(std::shared_ptr<VulkanEngine> engine)
	{
		LightBuffer = UniformData<T>(engine);
		Update(engine);
	}

	~Light()
	{

	}

	virtual void Update(std::shared_ptr<VulkanEngine> engine)
	{
		LightBuffer.Update(engine);
	}

	virtual void Destroy(std::shared_ptr<VulkanEngine> engine)
	{
		LightBuffer.Destroy(engine);
	}

	VkBuffer GetLightBuffer() { return LightBuffer.VulkanBufferData.Buffer; }
};
