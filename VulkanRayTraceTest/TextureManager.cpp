#include "TextureManager.h"
#include "ImGui/imgui_impl_vulkan.h"

std::shared_ptr<TextureManager> TextureManagerPtr::textureManager = nullptr;

TextureManager::TextureManager()
{
}

TextureManager::TextureManager(std::shared_ptr<VulkanEngine> Engine)
{
	engine = Engine;

	LoadTexture2D("../texture/DefaultTexture.png", VK_FORMAT_R8G8B8A8_UNORM);
	TextureList.back()->FileName = "DefaultTexture";
	
	LoadTexture2D("../texture/AlphaDefault.png", VK_FORMAT_R8G8B8A8_UNORM);
	TextureList.back()->FileName = "DefaultAlphaTexture";

	VkSamplerCreateInfo NullSamplerInfo = {};
	NullSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	NullSamplerInfo.magFilter = VK_FILTER_NEAREST;
	NullSamplerInfo.minFilter = VK_FILTER_NEAREST;
	NullSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	NullSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	NullSamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	NullSamplerInfo.anisotropyEnable = VK_TRUE;
	NullSamplerInfo.maxAnisotropy = 16.0f;
	NullSamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	NullSamplerInfo.unnormalizedCoordinates = VK_FALSE;
	NullSamplerInfo.compareEnable = VK_FALSE;
	NullSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	NullSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	NullSamplerInfo.minLod = 0;
	NullSamplerInfo.maxLod = 0;
	NullSamplerInfo.mipLodBias = 0;
	if (vkCreateSampler(engine->Device, &NullSamplerInfo, nullptr, &NullSampler))
	{
		throw std::runtime_error("Failed to create Sampler.");
	}
}

TextureManager::~TextureManager()
{
}

std::shared_ptr<Texture2D> TextureManager::LoadTexture2D(const std::string TextureLocation, VkFormat format)
{
	std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(Texture2D(engine, TextureLocation, format));
	TextureList.emplace_back(texture);
	return texture;
}

std::shared_ptr<Texture2D> TextureManager::LoadTexture2D(unsigned int width, unsigned int height, std::vector<Pixel>& PixelList, VkFormat format)
{
	std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(Texture2D(engine, width, height, PixelList, format));
	TextureList.emplace_back(texture);
	TextureList.back()->TextureID;
	return texture;
}

uint32_t TextureManager::LoadTexture2D(std::shared_ptr<Texture> RenderedTexture)
{
	unsigned int TextureID = TextureList.size();
	TextureList.emplace_back(RenderedTexture);
	TextureList.back()->TextureBufferIndex = TextureList.size();
	return TextureID;
}

std::shared_ptr<FontTexture> TextureManager::LoadTextTexture(void* GlyphData, uint32_t width, uint32_t height)
{
	std::shared_ptr<FontTexture> texture = std::make_shared<FontTexture>(FontTexture(engine, GlyphData, width, height));
	TextureList.emplace_back(texture);
	TextureList.back()->TextureID;
	return texture;
}

uint32_t TextureManager::Load3DTexture(const std::string TextureLocation, VkFormat format)
{
	Texture3DList.emplace_back(std::make_shared<Texture3D>(Texture3D(engine, TextureLocation, format)));
	return Texture3DList.back()->TextureID;
}

uint32_t TextureManager::LoadTexture3D(int width, int height, int depth, std::vector<Pixel>& PixelList, VkFormat format)
{
	unsigned int TextureID = Texture3DList.size();
	Texture3DList.emplace_back(std::make_shared<Texture3D>(Texture3D(engine, width, height, depth, PixelList, format)));
	return TextureID;
}

void TextureManager::LoadCubeMap(CubeMapLayout CubeMapFiles, VkFormat textureFormat)
{
	CubeMap = std::make_shared<CubeMapTexture>(CubeMapTexture(engine, CubeMapFiles, textureFormat));
}

void TextureManager::LoadCubeMap(std::string CubeMapFiles[6], VkFormat textureFormat)
{
	CubeMap = std::make_shared<CubeMapTexture>(CubeMapTexture(engine, CubeMapFiles, textureFormat));
}

void TextureManager::LoadCubeMap(std::string CubeMapLocation, VkFormat textureFormat)
{
	CubeMap = std::make_shared<CubeMapTexture>(CubeMapTexture(engine, CubeMapLocation, textureFormat));
}

void TextureManager::LoadCubeMap(std::shared_ptr<Texture> cubeMapTexture)
{
	CubeMap = cubeMapTexture;
}

void TextureManager::DeleteTexture(uint32_t TextureBufferIndex)
{
	auto texture = GetTextureByBufferIndex(TextureBufferIndex);
	texture->Delete();
	TextureList.erase(TextureList.begin() + TextureBufferIndex);

	for(int x = 0; x < TextureList.size(); x++)
	{
		TextureList[x]->TextureBufferIndex = x;
	}
}

void TextureManager::UnloadAllTextures()
{
	for (auto& texture : TextureList)
	{
		texture->Delete();
	}

	for (auto& texture : Texture3DList)
	{
		texture->Delete();
	}
}

void TextureManager::UnloadCubeMap()
{
	if (CubeMap != nullptr)
	{
		CubeMap->Delete();
	}
}

void TextureManager::Destory()
{
	vkDestroySampler(engine->Device, NullSampler, nullptr);
	NullSampler = VK_NULL_HANDLE;

	UnloadAllTextures();
	UnloadCubeMap();
}

void TextureManager::Update()
{
	for (int x = 0; x < TextureList.size(); x++)
	{
		TextureList[x]->Update(x);
	}
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
	if (TextureList.size() == 0)
	{
		VkDescriptorImageInfo nullBuffer;
		nullBuffer.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		nullBuffer.imageView = VK_NULL_HANDLE;
		nullBuffer.sampler = TextureList[0]->Sampler;
		DescriptorImageList.emplace_back(nullBuffer);
	}
	else
	{
		for (auto texture : TextureList)
		{
			VkDescriptorImageInfo DescriptorImage{};
			DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			DescriptorImage.imageView = texture->GetTextureView();
			DescriptorImage.sampler = texture->GetTextureSampler();
			DescriptorImageList.emplace_back(DescriptorImage);
		}
	}
	return DescriptorImageList;
}

std::vector<VkDescriptorImageInfo> TextureManager::Get3DTextureBufferListDescriptor()
{
	std::vector<VkDescriptorImageInfo> DescriptorImageList;
	if (Texture3DList.size() == 0)
	{
		VkDescriptorImageInfo nullBuffer;
		nullBuffer.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		nullBuffer.imageView = VK_NULL_HANDLE;
		nullBuffer.sampler = TextureList[0]->Sampler;
		DescriptorImageList.emplace_back(nullBuffer);
	}
	else
	{
		for (auto texture : Texture3DList)
		{
			VkDescriptorImageInfo DescriptorImage{};
			DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			DescriptorImage.imageView = texture->GetTextureView();
			DescriptorImage.sampler = texture->GetTextureSampler();
			DescriptorImageList.emplace_back(DescriptorImage);
		}
	}
	return DescriptorImageList;
}

VkDescriptorImageInfo TextureManager::GetSkyBoxTextureBufferListDescriptor()
{
	VkDescriptorImageInfo DescriptorImage{};
	if (CubeMap.get() != nullptr)
	{
		DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		DescriptorImage.imageView = CubeMap->GetTextureView();
		DescriptorImage.sampler = CubeMap->GetTextureSampler();
	}
	else
	{
		DescriptorImage.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		DescriptorImage.imageView = VK_NULL_HANDLE;
		DescriptorImage.sampler = TextureList[0]->Sampler;
	}
	return DescriptorImage;
}

uint32_t TextureManager::GetTextureBufferDescriptorCount()
{
	if (TextureList.size() > 0)
	{
		return TextureList.size();
	}
	else
	{
		return 1;
	}
}
uint32_t TextureManager::Get3DTextureBufferDescriptorCount()
{
	if (Texture3DList.size() > 0)
	{
		return Texture3DList.size();
	}
	else
	{
		return 1;
	}
}