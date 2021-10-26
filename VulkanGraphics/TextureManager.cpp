#include "TextureManager.h"
#include "ImGui/imgui_impl_vulkan.h"

std::shared_ptr<TextureManager> TextureManagerPtr::textureManager = nullptr;

TextureManager::TextureManager()
{
}

TextureManager::TextureManager(std::shared_ptr<VulkanEngine> Engine)
{
	LoadTexture2D("../texture/DefaultTexture.png", VK_FORMAT_R8G8B8A8_UNORM);
	Texture2DList.back()->FileName = "DefaultTexture";
	
	LoadTexture2D("../texture/AlphaDefault.png", VK_FORMAT_R8G8B8A8_UNORM);
	Texture2DList.back()->FileName = "DefaultAlphaTexture";

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
	Texture2DList.emplace_back(texture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return texture;
}

std::shared_ptr<Texture2D> TextureManager::LoadTexture2D(const std::string TextureLocation, VkFormat format)
{
	uint32_t isTextureLoaded = IsTextureLoaded(TextureLocation);
	if (isTextureLoaded != -1)
	{
		return GetTextureByID(isTextureLoaded);
	}

	IsTextureLoaded(TextureLocation);
	const std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(Texture2D(TextureLocation, format));
	Texture2DList.emplace_back(texture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return texture;
}

std::shared_ptr<Texture2D> TextureManager::LoadTexture2D(glm::ivec2 TextureResolution, std::vector<Pixel>& PixelList, VkFormat format)
{
	const std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(Texture2D(TextureResolution, PixelList, format));
	Texture2DList.emplace_back(texture);
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

//std::shared_ptr<FontTexture> TextureManager::LoadFontTexture(void* GlyphData, uint32_t width, uint32_t height)
//{
//	const std::shared_ptr<FontTexture> texture = std::make_shared<FontTexture>(FontTexture(GlyphData, width, height));
//	Texture2DList.emplace_back(texture);
//	UpdateBufferIndex();
//	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
//	return texture;
//}

std::shared_ptr<CubeMapTexture> TextureManager::LoadCubeMap(CubeMapLayout CubeMapFiles, VkFormat textureFormat)
{
	 std::shared_ptr<CubeMapTexture> texture = std::make_shared<CubeMapTexture>(CubeMapTexture(CubeMapFiles, textureFormat));
	 CubeMapList.emplace_back(texture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return texture;
}

std::shared_ptr<CubeMapTexture> TextureManager::LoadCubeMap(std::string CubeMapFiles[6], VkFormat textureFormat)
{
	const std::shared_ptr<CubeMapTexture> texture = std::make_shared<CubeMapTexture>(CubeMapTexture(CubeMapFiles, textureFormat));
	CubeMapList.emplace_back(texture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return texture;
}

std::shared_ptr<CubeMapTexture> TextureManager::LoadCubeMap(std::string CubeMapLocation, VkFormat textureFormat)
{
	const std::shared_ptr<CubeMapTexture> texture = std::make_shared<CubeMapTexture>(CubeMapTexture(CubeMapLocation, textureFormat));
	CubeMapList.emplace_back(texture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return texture;
}

std::shared_ptr<CubeMapTexture> TextureManager::LoadCubeMap(std::shared_ptr<CubeMapTexture> cubeMapTexture)
{
	CubeMapList.emplace_back(cubeMapTexture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return cubeMapTexture;
}

void TextureManager::DeleteTexture2DByID(uint32_t TextureID)
{
	auto texture = GetTextureByID(TextureID);
	texture->Delete();
	Texture2DList.erase(Texture2DList.begin() + texture->TextureBufferIndex);
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
	Texture2DList.erase(Texture2DList.begin() + Texture2DBufferIndex);
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
	for (auto& texture : Texture2DList)
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
	for (auto& CubeMapList : Texture3DList)
	{
		CubeMapList->Delete();
	}
}

void TextureManager::Destory()
{
	vkDestroySampler(EnginePtr::GetEnginePtr()->Device, NullSampler, nullptr);
	NullSampler = VK_NULL_HANDLE;

	UnloadAllTextures();
	UnloadCubeMap();
}

std::shared_ptr<Texture2D> TextureManager::GetTextureByID(uint32_t TextureID)
{
	for (auto& texture : Texture2DList)
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
	for (int x = 0; x < Texture2DList.size(); x++)
	{
		Texture2DList[x]->TextureBufferIndex = x;
	}

	for (int x = 0; x < Texture3DList.size(); x++)
	{
		Texture3DList[x]->TextureBufferIndex = x;
	}

	for (int x = 0; x < CubeMapList.size(); x++)
	{
		CubeMapList[x]->TextureBufferIndex = x;
	}
}

uint32_t TextureManager::IsTextureLoaded(std::string name)
{
	uint32_t textureID = -1;
	for (auto texture : Texture2DList)
	{
		if (texture->FileName == name)
		{
			textureID = texture->TextureID;
			return textureID;
		}
	}

	return textureID;
}

std::shared_ptr<Texture2D> TextureManager::GetTexture2DByName(const std::string TextureName)
{
	for (auto texture : Texture2DList)
	{
		if (texture->FileName == TextureName)
		{
			return texture;
		}
	}

	return Texture2DList[0];
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
	if (Texture2DList.size() == 0)
	{
		VkDescriptorImageInfo nullBuffer;
		nullBuffer.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		nullBuffer.imageView = VK_NULL_HANDLE;
		nullBuffer.sampler = Texture2DList[0]->Sampler;
		DescriptorImageList.emplace_back(nullBuffer);
	}
	else
	{
		for (auto texture : Texture2DList)
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
		nullBuffer.sampler = Texture2DList[0]->Sampler;
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

std::vector<VkDescriptorImageInfo> TextureManager::GetSkyBoxTextureBufferListDescriptor()
{
	std::vector<VkDescriptorImageInfo> DescriptorImageList;
	if (CubeMapList.size() == 0)
	{
		VkDescriptorImageInfo nullBuffer;
		nullBuffer.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		nullBuffer.imageView = VK_NULL_HANDLE;
		nullBuffer.sampler = Texture2DList[0]->Sampler;
		DescriptorImageList.emplace_back(nullBuffer);
	}
	else
	{
		for (auto texture : CubeMapList)
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

uint32_t TextureManager::GetTextureBufferDescriptorCount()
{
	if (Texture2DList.size() > 0)
	{
		return Texture2DList.size();
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

uint32_t TextureManager::GetCubeMapBufferDescriptorCount()
{
	if (CubeMapList.size() > 0)
	{
		return CubeMapList.size();
	}
	else
	{
		return 1;
	}
}
