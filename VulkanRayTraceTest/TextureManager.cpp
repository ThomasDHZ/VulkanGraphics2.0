#include "TextureManager.h"
#include "ImGui/imgui_impl_vulkan.h"

TextureManager::TextureManager()
{
}

TextureManager::TextureManager(VulkanEngine& engine)
{
	LoadTexture2D(engine, "../texture/DefaultTexture.png", VK_FORMAT_R8G8B8A8_UNORM);
	TextureList[0]->FileName = "DefaultTexture";
	
	LoadTexture2D(engine, "../texture/AlphaDefault.png", VK_FORMAT_R8G8B8A8_UNORM);
	TextureList[1]->FileName = "DefaultAlphaTexture";

	//auto ab = LoadTexture2D(engine, "../texture/pbr/rusted_iron/albedo.png", VK_FORMAT_R8G8B8A8_SRGB);
	//TextureList[2]->FileName = "AlbedoTexture";
	//
	//auto ac = LoadTexture2D(engine, "../texture/pbr/rusted_iron/normal.png", VK_FORMAT_R8G8B8A8_UNORM);
	//TextureList[3]->FileName = "NormalTexture";
	//
	//auto ad = LoadTexture2D(engine, "../texture/pbr/rusted_iron/roughness.png", VK_FORMAT_R8G8B8A8_UNORM);
	//TextureList[4]->FileName = "RoughnessTexture";
	//
	//auto ae = LoadTexture2D(engine, "../texture/pbr/rusted_iron/ao.png", VK_FORMAT_R8G8B8A8_UNORM);
	//TextureList[5]->FileName = "AOTexture";

	//auto af = LoadTexture2D(engine, "../texture/pbr/rusted_iron/metallic.png", VK_FORMAT_R8G8B8A8_UNORM);
	//TextureList[6]->FileName = "Metallic";

	//int a = 34;
}

TextureManager::~TextureManager()
{
}

std::shared_ptr<Texture2D> TextureManager::LoadTexture2D(VulkanEngine& engine, const std::string TextureLocation, VkFormat format)
{
	//uint32_t TextureID = IsTextureLoaded(TextureLocation);
	//if (TextureID == -1)
	//{
		unsigned int TextureID = TextureList.size();
		std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(Texture2D(engine, TextureLocation, format, TextureID));
		TextureList.emplace_back(texture);

		ImGui_ImplVulkan_AddTexture(texture->ImGuiDescriptorSet, texture->Sampler, texture->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	//}
	return texture;
}

uint32_t TextureManager::LoadTexture2D(VulkanEngine& engine, unsigned int width, unsigned int height, std::vector<Pixel>& PixelList, VkFormat format)
{
	unsigned int TextureID = TextureList.size();
	TextureList.emplace_back(std::make_shared<Texture2D>(Texture2D(engine, width, height, PixelList, format, TextureID)));
	ImGui_ImplVulkan_AddTexture(TextureList.back()->ImGuiDescriptorSet, TextureList.back()->Sampler, TextureList.back()->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	return TextureID;
}

uint32_t TextureManager::LoadTexture2D(std::shared_ptr<Texture> RenderedTexture)
{
	unsigned int TextureID = TextureList.size();
	TextureList.emplace_back(RenderedTexture);
	TextureList.back()->TextureBufferIndex = TextureList.size();
	ImGui_ImplVulkan_AddTexture(TextureList.back()->ImGuiDescriptorSet, TextureList.back()->Sampler, TextureList.back()->View, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	return TextureID;
}

uint32_t TextureManager::Load3DTexture(VulkanEngine& engine, const std::string TextureLocation, VkFormat format)
{
	//uint32_t TextureID = IsTextureLoaded(TextureLocation);
	//if (TextureID == -1)
	//{
	unsigned int TextureID = TextureList.size();
	Texture3DList.emplace_back(std::make_shared<Texture3D>(Texture3D(engine, TextureLocation, format, TextureID)));
	//}
	return TextureID;
}

uint32_t TextureManager::LoadTexture3D(VulkanEngine& engine, int width, int height, int depth, std::vector<Pixel>& PixelList, VkFormat format)
{
	unsigned int TextureID = Texture3DList.size();
	Texture3DList.emplace_back(std::make_shared<Texture3D>(Texture3D(engine, width, height, depth, PixelList, format, TextureID)));
	return TextureID;
}

void TextureManager::LoadCubeMap(VulkanEngine& engine, CubeMapLayout CubeMapFiles, VkFormat textureFormat)
{
	CubeMap = CubeMapTexture(engine, CubeMapFiles, textureFormat, 0);
}

void TextureManager::LoadCubeMap(VulkanEngine& engine, std::string CubeMapFiles[6], VkFormat textureFormat)
{
	CubeMap = CubeMapTexture(engine, CubeMapFiles, textureFormat, 0);
}

void TextureManager::DeleteTexture(VulkanEngine& engine, uint32_t TextureBufferIndex)
{
	auto texture = GetTextureByBufferIndex(TextureBufferIndex);
	texture->Delete(engine);
	TextureList.erase(TextureList.begin() + TextureBufferIndex);

	for(int x = 0; x < TextureList.size(); x++)
	{
		TextureList[x]->TextureBufferIndex = x;
	}
}

void TextureManager::UnloadAllTextures(VulkanEngine& engine)
{
	for (auto& texture : TextureList)
	{
		texture->Delete(engine);
	}

	for (auto& texture : Texture3DList)
	{
		texture->Delete(engine);
	}
}

void TextureManager::UnloadCubeMap(VulkanEngine& engine)
{
	CubeMap.Delete(engine);
}

void TextureManager::Destory(VulkanEngine& engine)
{
	UnloadAllTextures(engine);
	UnloadCubeMap(engine);
}


uint32_t TextureManager::IsTextureLoaded(std::string name)
{
	uint32_t textureID = -1;
	for (auto texture : TextureList)
	{
		if (texture->FileName == name)
		{
			textureID = texture->TextureID;
			return textureID;
		}
	}

	return textureID;
}

std::shared_ptr<Texture> TextureManager::GetTextureByName(const std::string TextureName)
{
	for (auto texture : TextureList)
	{
		if (texture->FileName == TextureName)
		{
			return texture;
		}
	}

	return TextureList[0];
}

std::shared_ptr<Texture3D> TextureManager::Get3DTextureByName(const std::string TextureName)
{
	for (auto texture : Texture3DList)
	{
		if (texture->FileName == TextureName)
		{
			return texture;
		}
	}

	return Texture3DList[0];
}

std::vector<VkDescriptorImageInfo> TextureManager::GetTextureBufferListDescriptor()
{
	std::vector<VkDescriptorImageInfo> DescriptorImageList;
	for (auto texture : TextureList)
	{
		VkDescriptorImageInfo DescriptorImage{};
		DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		DescriptorImage.imageView = texture->GetTextureView();
		DescriptorImage.sampler = texture->GetTextureSampler();
		DescriptorImageList.emplace_back(DescriptorImage);
	}
	return DescriptorImageList;
}

std::vector<VkDescriptorImageInfo> TextureManager::Get3DTextureBufferListDescriptor()
{
	std::vector<VkDescriptorImageInfo> DescriptorImageList;
	for (auto texture : Texture3DList)
	{
		VkDescriptorImageInfo DescriptorImage{};
		DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		DescriptorImage.imageView = texture->GetTextureView();
		DescriptorImage.sampler = texture->GetTextureSampler();
		DescriptorImageList.emplace_back(DescriptorImage);
	}
	return DescriptorImageList;
}

VkDescriptorImageInfo TextureManager::GetSkyBoxTextureBufferListDescriptor()
{
	VkDescriptorImageInfo DescriptorImage{};
	DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	DescriptorImage.imageView = CubeMap.GetTextureView();
	DescriptorImage.sampler = CubeMap.GetTextureSampler();
	return DescriptorImage;
}