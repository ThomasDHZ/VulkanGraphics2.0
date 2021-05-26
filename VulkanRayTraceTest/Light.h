#pragma once
#include "VulkanEngine.h"
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
	
	Light(VulkanEngine& engine)
	{
		LightBuffer = UniformData<T>(engine);
		Update(engine);
	}

	~Light()
	{

	}

	virtual void Update(VulkanEngine& engine)
	{
		LightBuffer.Update(engine);
	}

	virtual void Destroy(VulkanEngine& engine)
	{
		LightBuffer.Destroy(engine);
	}

	VkBuffer GetLightBuffer() { return LightBuffer.VulkanBufferData.Buffer; }
};
