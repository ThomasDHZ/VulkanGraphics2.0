#include "LightManager.h"
#include "OrthographicCamera.h"

LightManager::LightManager()
{
}

LightManager::LightManager(VulkanEngine& engine, std::shared_ptr<TextureManager>textureManager, VkDescriptorSetLayout& layout, int renderBit, glm::vec3 Pos)
{
	//light.dLight.direction = glm::vec3(0.5f, 1.0f, 0.3f);

	//PointLightList.emplace_back(std::make_shared<PointLight>(PointLight(engine, textureManager, layout, renderBit, Pos)));
	//PointLightList.emplace_back(std::make_shared<PointLight>(PointLight(engine, textureManager, layout, renderBit, Pos)));
	//PointLightList.emplace_back(std::make_shared<PointLight>(PointLight(engine, textureManager, layout, renderBit, Pos)));
	//PointLightList.emplace_back(std::make_shared<PointLight>(PointLight(engine, textureManager, layout, renderBit, Pos)));

	light.dLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
	light.dLight.ambient = glm::vec3(0.45f, 0.45f, 0.45f);
	light.dLight.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
	light.dLight.specular = glm::vec3(0.5f, 0.5f, 0.5f);

    //light.pLight[0].position = glm::vec3(0.7f, 0.2f, 2.0f);
    //light.pLight[0].ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    //light.pLight[0].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    //light.pLight[0].specular = glm::vec3(1.0f, 1.0f, 1.0f);
    //light.pLight[0].constant = 1.0f;
    //light.pLight[0].linear = 0.09f;
    //light.pLight[0].quadratic = 0.032f;
    //// point light 2
    //light.pLight[0].position = glm::vec3(2.3f, -3.3f, -4.0f);
    //light.pLight[0].ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    //light.pLight[0].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    //light.pLight[0].specular = glm::vec3(1.0f, 1.0f, 1.0f);
    //light.pLight[0].constant = 1.0f;
    //light.pLight[0].linear = 0.09f;
    //light.pLight[0].quadratic = 0.032f;
    //// point light 3
    //light.pLight[0].position = glm::vec3(-4.0f, 2.0f, -12.0f);
    //light.pLight[0].ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    //light.pLight[0].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    //light.pLight[0].specular = glm::vec3(1.0f, 1.0f, 1.0f);
    //light.pLight[0].constant = 1.0f;
    //light.pLight[0].linear = 0.09f;
    //light.pLight[0].quadratic = 0.032f;
    //// point light 4
    //light.pLight[0].position = glm::vec3(0.0f, 0.0f, -3.0f);
    //light.pLight[0].ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    //light.pLight[0].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    //light.pLight[0].specular = glm::vec3(1.0f, 1.0f, 1.0f);
    //light.pLight[0].constant = 1.0f;
    //light.pLight[0].linear = 0.09f;
    //light.pLight[0].quadratic = 0.032f;

    light.sLight.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
    light.sLight.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
    light.sLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    light.sLight.constant = 1.0f;
    light.sLight.linear = 0.09f;
    light.sLight.quadratic = 0.032f;
    light.sLight.cutOff = glm::cos(glm::radians(12.5f));
    light.sLight.outerCutOff = glm::cos(glm::radians(15.0f));
}

LightManager::~LightManager()
{
}

void LightManager::Draw(VkCommandBuffer& RenderCommandBuffer, std::shared_ptr<GraphicsPipeline> pipeline, int FrameNumber)
{
	for (auto light : PointLightList)
	{
		light->Draw(RenderCommandBuffer, pipeline, FrameNumber);
	}
}

void LightManager::Update(VulkanEngine& engine, std::shared_ptr<PerspectiveCamera>& camera)
{
	light.viewPos = static_cast<PerspectiveCamera*>(camera.get())->GetPosition();
    light.sLight.position = camera->GetPosition();
    light.sLight.direction = camera->GetFront();
	for (int x = 0; x < PointLightList.size(); x++)
	{
		PointLightList[x]->Update(engine, camera);
		light.pLight[x] = PointLightList[x]->pointLight;
	}
}

void LightManager::Destory(VulkanEngine& engine)
{
	for (auto pointLight : PointLightList)
	{
		pointLight->Destory(engine);
	}
}