#include "TextureManager.h"
#include "ImGui/imgui_impl_vulkan.h"

std::shared_ptr<TextureManager> TextureManagerPtr::textureManager = nullptr;

TextureManager::TextureManager()
{
}

TextureManager::TextureManager(std::shared_ptr<VulkanEngine> Engine)
{
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
	if (vkCreateSampler(EnginePtr::GetEnginePtr()->Device, &NullSamplerInfo, nullptr, &NullSampler))
	{
		throw std::runtime_error("Failed to create Sampler.");
	}
}

TextureManager::~TextureManager()
{
}

std::shared_ptr<Texture2D> TextureManager::LoadTexture2D(std::shared_ptr<Texture2D> texture)
{
	TextureList.emplace_back(texture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return texture;
}

std::shared_ptr<Texture2D> TextureManager::LoadTexture2D(const std::string TextureLocation, VkFormat format)
{
	const std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(Texture2D(TextureLocation, format));
	TextureList.emplace_back(texture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return texture;
}

std::shared_ptr<Texture2D> TextureManager::LoadTexture2D(glm::ivec2 TextureResolution, std::vector<Pixel>& PixelList, VkFormat format)
{
	const std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(Texture2D(TextureResolution, PixelList, format));
	TextureList.emplace_back(texture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return texture;
}

std::shared_ptr<Texture3D> TextureManager::LoadTexture3D(const std::string TextureLocation, VkFormat format)
{
	const std::shared_ptr<Texture3D> texture = std::make_shared<Texture3D>(Texture3D(TextureLocation, format));
	Texture3DList.emplace_back(texture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return texture;
}

std::shared_ptr<Texture3D> TextureManager::LoadTexture3D(glm::ivec3& TextureResolution, std::vector<Pixel>& PixelList, VkFormat format)
{
	const std::shared_ptr<Texture3D> texture = std::make_shared<Texture3D>(Texture3D(TextureResolution, PixelList, format));
	Texture3DList.emplace_back(texture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return texture;
}

std::shared_ptr<FontTexture> TextureManager::LoadFontTexture(void* GlyphData, uint32_t width, uint32_t height)
{
	const std::shared_ptr<FontTexture> texture = std::make_shared<FontTexture>(FontTexture(GlyphData, width, height));
	TextureList.emplace_back(texture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return texture;
}

void TextureManager::LoadCubeMap(CubeMapLayout CubeMapFiles, VkFormat textureFormat)
{
	CubeMap = std::make_shared<CubeMapTexture>(CubeMapTexture(CubeMapFiles, textureFormat));
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::LoadCubeMap(std::string CubeMapFiles[6], VkFormat textureFormat)
{
	CubeMap = std::make_shared<CubeMapTexture>(CubeMapTexture(CubeMapFiles, textureFormat));
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::LoadCubeMap(std::string CubeMapLocation, VkFormat textureFormat)
{
	CubeMap = std::make_shared<CubeMapTexture>(CubeMapTexture(CubeMapLocation, textureFormat));
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::LoadCubeMap(std::shared_ptr<Texture> cubeMapTexture)
{
	CubeMap = cubeMapTexture;
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteTexture2DByID(uint32_t TextureID)
{
	auto texture = GetTextureByID(TextureID);
	texture->Delete();
	TextureList.erase(TextureList.begin() + texture->TextureBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteTexture3DByID(uint32_t TextureID)
{
	auto texture = GetTextureByID(TextureID);
	texture->Delete();
	Texture3DList.erase(Texture3DList.begin() + texture->TextureBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteTexture2DByBufferIndex(uint32_t Texture2DBufferIndex)
{
	auto texture = GetTextureByBufferIndex(Texture2DBufferIndex);
	texture->Delete();
	TextureList.erase(TextureList.begin() + Texture2DBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteTexture3DByBufferIndex(uint32_t Texture3DBufferIndex)
{
	auto texture = GetTexture3DByBufferIndex(Texture3DBufferIndex);
	texture->Delete();
	Texture3DList.erase(Texture3DList.begin() + Texture3DBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
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
	vkDestroySampler(EnginePtr::GetEnginePtr()->Device, NullSampler, nullptr);
	NullSampler = VK_NULL_HANDLE;

	UnloadAllTextures();
	UnloadCubeMap();
}

std::shared_ptr<Texture> TextureManager::GetTextureByID(uint32_t TextureID)
{
	for (auto& texture : TextureList)
	{
		if (texture->TextureID == TextureID)
		{
			return texture;
		}
	}

	std::cout << "Texture with ID: " << TextureID << "not found." << std::endl;
	return nullptr;
}

std::shared_ptr<Texture3D> TextureManager::GetTexture3DByID(uint32_t TextureID)
{
	for (auto& texture : Texture3DList)
	{
		if (texture->TextureID == TextureID)
		{
			return texture;
		}
	}

	std::cout << "Texture with ID: " << TextureID << "not found." << std::endl;
	return nullptr;
}

void TextureManager::UpdateBufferIndex()
{
	for (int x = 0; x < TextureList.size(); x++)
	{
		TextureList[x]->TextureBufferIndex = x;
	}

	for (int x = 0; x < Texture3DList.size(); x++)
	{
		Texture3DList[x]->TextureBufferIndex = x;
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