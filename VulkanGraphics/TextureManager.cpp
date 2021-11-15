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

std::shared_ptr<RenderedColorTexture> TextureManager::LoadRenderedColorTexture(std::shared_ptr<RenderedColorTexture> renderedColorTexture)
{
	RenderedColorTextureList.emplace_back(renderedColorTexture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return renderedColorTexture;
}

std::shared_ptr<RenderedDepthTexture> TextureManager::LoadRenderedDepthTexture(std::shared_ptr<RenderedDepthTexture> renderedDepthTexture)
{
	RenderedDepthTextureList.emplace_back(renderedDepthTexture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return renderedDepthTexture;
}

std::shared_ptr<RenderedCubeMapTexture> TextureManager::LoadRenderedCubeMapTexture(std::shared_ptr<RenderedCubeMapTexture> renderedCubeMapTexture)
{
	RenderedCubeMapTextureList.emplace_back(renderedCubeMapTexture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return renderedCubeMapTexture;
}

std::shared_ptr<RenderedCubeMapDepthTexture> TextureManager::LoadRenderedCubeMapDepthTexture(std::shared_ptr<RenderedCubeMapDepthTexture> renderedCubeMapDepthTexture)
{
	RenderedCubeMapDepthTextureList.emplace_back(renderedCubeMapDepthTexture);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
	return renderedCubeMapDepthTexture;
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

void TextureManager::DeleteCubeMapTextureByID(uint32_t TextureID)
{
	auto texture = GetCubeMapTextureByID(TextureID);
	texture->Delete();
	CubeMapList.erase(CubeMapList.begin() + texture->TextureBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteRenderedColorTextureByID(uint32_t TextureID)
{
	auto texture = GetRenderedColorTextureByID(TextureID);
	texture->Delete();
	RenderedColorTextureList.erase(RenderedColorTextureList.begin() + texture->TextureBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteRenderedDepthTextureByID(uint32_t TextureID)
{
	auto texture = GetRenderedDepthTextureByID(TextureID);
	texture->Delete();
	RenderedDepthTextureList.erase(RenderedDepthTextureList.begin() + texture->TextureBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteRenderedCubeMapTextureByID(uint32_t TextureID)
{
	auto texture = GetRenderedCubeMapTextureByID(TextureID);
	texture->Delete();
	RenderedCubeMapTextureList.erase(RenderedCubeMapTextureList.begin() + texture->TextureBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteRenderedCubeMapDepthTextureByID(uint32_t TextureID)
{
	auto texture = GetRenderedCubeMapDepthTextureByID(TextureID);
	texture->Delete();
	RenderedCubeMapDepthTextureList.erase(RenderedCubeMapDepthTextureList.begin() + texture->TextureBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteTexture2DByBufferIndex(uint32_t TextureBufferIndex)
{
	auto texture = GetTextureByBufferIndex(TextureBufferIndex);
	texture->Delete();
	Texture2DList.erase(Texture2DList.begin() + TextureBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteTexture3DByBufferIndex(uint32_t TextureBufferIndex)
{
	auto texture = GetTexture3DByBufferIndex(TextureBufferIndex);
	texture->Delete();
	Texture3DList.erase(Texture3DList.begin() + TextureBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteCubeMapTextureByBufferIndex(uint32_t TextureBufferIndex)
{
	auto texture = GetCubeMapTextureByBufferIndex(TextureBufferIndex);
	texture->Delete();
	CubeMapList.erase(CubeMapList.begin() + TextureBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteRenderedColorTextureByBufferIndex(uint32_t TextureBufferIndex)
{
	auto texture = GetRenderedColorTextureByBufferIndex(TextureBufferIndex);
	texture->Delete();
	RenderedColorTextureList.erase(RenderedColorTextureList.begin() + TextureBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteRenderedDepthTextureByBufferIndex(uint32_t TextureBufferIndex)
{
	auto texture = GetCubeMapTextureByBufferIndex(TextureBufferIndex);
	texture->Delete();
	RenderedDepthTextureList.erase(RenderedDepthTextureList.begin() + TextureBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteRenderedCubeMapTextureByBufferIndex(uint32_t TextureBufferIndex)
{
	auto texture = GetCubeMapTextureByBufferIndex(TextureBufferIndex);
	texture->Delete();
	RenderedCubeMapTextureList.erase(RenderedCubeMapTextureList.begin() + TextureBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::DeleteRenderedCubeMapDepthTextureByBufferIndex(uint32_t TextureBufferIndex)
{
	auto texture = GetRenderedCubeMapDepthTextureByBufferIndex(TextureBufferIndex);
	texture->Delete();
	RenderedCubeMapDepthTextureList.erase(RenderedCubeMapDepthTextureList.begin() + TextureBufferIndex);
	UpdateBufferIndex();
	EnginePtr::GetEnginePtr()->UpdateRendererFlag = true;
}

void TextureManager::Destory()
{
	vkDestroySampler(EnginePtr::GetEnginePtr()->Device, NullSampler, nullptr);
	NullSampler = VK_NULL_HANDLE;

	for (auto& texture : Texture2DList)
	{
		texture->Delete();
	}

	for (auto& texture : Texture3DList)
	{
		texture->Delete();
	}

	for (auto& texture : CubeMapList)
	{
		texture->Delete();
	}

	for (auto& texture : RenderedColorTextureList)
	{
		texture->Delete();
	}

	for (auto& texture : RenderedDepthTextureList)
	{
		texture->Delete();
	}

	for (auto& texture : RenderedCubeMapTextureList)
	{
		texture->Delete();
	}

	for (auto& texture : RenderedCubeMapDepthTextureList)
	{
		texture->Delete();
	}
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

std::shared_ptr<Texture> TextureManager::GetCubeMapTextureByID(uint32_t TextureID)
{
	for (auto& texture : CubeMapList)
	{
		if (texture->TextureID == TextureID)
		{
			return texture;
		}
	}

	std::cout << "CubeMap with ID: " << TextureID << "not found." << std::endl;
	return nullptr;
}

std::shared_ptr<RenderedColorTexture> TextureManager::GetRenderedColorTextureByID(uint32_t TextureID)
{
	for (auto& texture : RenderedColorTextureList)
	{
		if (texture->TextureID == TextureID)
		{
			return texture;
		}
	}

	std::cout << "RenderedColorTexture with ID: " << TextureID << "not found." << std::endl;
	return nullptr;
}

std::shared_ptr<RenderedDepthTexture> TextureManager::GetRenderedDepthTextureByID(uint32_t TextureID)
{
	for (auto& texture : RenderedDepthTextureList)
	{
		if (texture->TextureID == TextureID)
		{
			return texture;
		}
	}

	std::cout << "RenderedDepthTexture with ID: " << TextureID << "not found." << std::endl;
	return nullptr;
}

std::shared_ptr<RenderedCubeMapTexture> TextureManager::GetRenderedCubeMapTextureByID(uint32_t TextureID)
{
	for (auto& texture : RenderedCubeMapTextureList)
	{
		if (texture->TextureID == TextureID)
		{
			return texture;
		}
	}

	std::cout << "RenderedCubeMapTexture with ID: " << TextureID << "not found." << std::endl;
	return nullptr;
}

std::shared_ptr<RenderedCubeMapDepthTexture> TextureManager::GetRenderedCubeMapDepthTextureByID(uint32_t TextureID)
{
	for (auto& texture : RenderedCubeMapDepthTextureList)
	{
		if (texture->TextureID == TextureID)
		{
			return texture;
		}
	}

	std::cout << "RenderedCubeMapDepthTexture with ID: " << TextureID << "not found." << std::endl;
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

	for (int x = 0; x < RenderedColorTextureList.size(); x++)
	{
		RenderedColorTextureList[x]->TextureBufferIndex = x;
	}

	for (int x = 0; x < RenderedDepthTextureList.size(); x++)
	{
		RenderedDepthTextureList[x]->TextureBufferIndex = x;
	}

	for (int x = 0; x < RenderedCubeMapTextureList.size(); x++)
	{
		RenderedCubeMapTextureList[x]->TextureBufferIndex = x;
	}

	for (int x = 0; x < RenderedCubeMapDepthTextureList.size(); x++)
	{
		RenderedCubeMapDepthTextureList[x]->TextureBufferIndex = x;
	}
}

void TextureManager::ClearTextures()
{
	for (auto& texture : Texture2DList)
	{
		texture->Delete();
	}

	for (auto& texture : Texture3DList)
	{
		texture->Delete();
	}

	for (auto& texture : CubeMapList)
	{
		texture->Delete();
	}

	for (auto& texture : RenderedColorTextureList)
	{
		texture->Delete();
	}

	for (auto& texture : RenderedDepthTextureList)
	{
		texture->Delete();
	}

	for (auto& texture : RenderedCubeMapTextureList)
	{
		texture->Delete();
	}

	for (auto& texture : RenderedCubeMapDepthTextureList)
	{
		texture->Delete();
	}

	Texture2DList.clear();
	Texture3DList.clear();
	CubeMapList.clear();
	RenderedColorTextureList.clear();
	RenderedDepthTextureList.clear();
	RenderedCubeMapTextureList.clear();
	RenderedCubeMapDepthTextureList.clear();
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

std::vector<VkDescriptorImageInfo> TextureManager::GetTexture2DBufferDescriptorList()
{
	std::vector<VkDescriptorImageInfo> DescriptorImageList;
	if (Texture2DList.size() == 0)
	{
		VkDescriptorImageInfo nullBuffer;
		nullBuffer.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		nullBuffer.imageView = VK_NULL_HANDLE;
		nullBuffer.sampler = NullSampler;
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

std::vector<VkDescriptorImageInfo> TextureManager::GetTexture3DBufferDescriptorList()
{
	std::vector<VkDescriptorImageInfo> DescriptorImageList;
	if (Texture3DList.size() == 0)
	{
		VkDescriptorImageInfo nullBuffer;
		nullBuffer.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		nullBuffer.imageView = VK_NULL_HANDLE;
		nullBuffer.sampler = NullSampler;
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

std::vector<VkDescriptorImageInfo> TextureManager::GetCubeMapTextureBufferDescriptorList()
{
	std::vector<VkDescriptorImageInfo> DescriptorImageList;
	if (CubeMapList.size() == 0)
	{
		VkDescriptorImageInfo nullBuffer;
		nullBuffer.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		nullBuffer.imageView = VK_NULL_HANDLE;
		nullBuffer.sampler = NullSampler;
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

std::vector<VkDescriptorImageInfo> TextureManager::GetRenderedColorTextureBufferDescriptorList()
{
	std::vector<VkDescriptorImageInfo> DescriptorImageList;
	if (RenderedColorTextureList.size() == 0)
	{
		VkDescriptorImageInfo nullBuffer;
		nullBuffer.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		nullBuffer.imageView = VK_NULL_HANDLE;
		nullBuffer.sampler = NullSampler;
		DescriptorImageList.emplace_back(nullBuffer);
	}
	else
	{
		for (auto texture : RenderedColorTextureList)
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

std::vector<VkDescriptorImageInfo> TextureManager::GetRenderedDepthTextureBufferDescriptorList()
{
	std::vector<VkDescriptorImageInfo> DescriptorImageList;
	if (RenderedDepthTextureList.size() == 0)
	{
		VkDescriptorImageInfo nullBuffer;
		nullBuffer.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		nullBuffer.imageView = VK_NULL_HANDLE;
		nullBuffer.sampler = NullSampler;
		DescriptorImageList.emplace_back(nullBuffer);
	}
	else
	{
		for (auto texture : RenderedDepthTextureList)
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

std::vector<VkDescriptorImageInfo> TextureManager::GetRenderedCubeMapTextureBufferDescriptorList()
{
	std::vector<VkDescriptorImageInfo> DescriptorImageList;
	if (RenderedCubeMapTextureList.size() == 0)
	{
		VkDescriptorImageInfo nullBuffer;
		nullBuffer.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		nullBuffer.imageView = VK_NULL_HANDLE;
		nullBuffer.sampler = NullSampler;
		DescriptorImageList.emplace_back(nullBuffer);
	}
	else
	{
		for (auto texture : RenderedCubeMapTextureList)
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

std::vector<VkDescriptorImageInfo> TextureManager::GetRenderedCubeMapDepthTextureBufferDescriptorList()
{
	std::vector<VkDescriptorImageInfo> DescriptorImageList;
	if (RenderedCubeMapDepthTextureList.size() == 0)
	{
		VkDescriptorImageInfo nullBuffer;
		nullBuffer.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		nullBuffer.imageView = VK_NULL_HANDLE;
		nullBuffer.sampler = NullSampler;
		DescriptorImageList.emplace_back(nullBuffer);
	}
	else
	{
		for (auto texture : RenderedCubeMapDepthTextureList)
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

uint32_t TextureManager::GetRenderedColorTextureBufferCount()
{
	if (RenderedColorTextureList.size() > 0)
	{
		return RenderedColorTextureList.size();
	}
	else
	{
		return 1;
	}
}

uint32_t TextureManager::GetRenderedDepthTextureBufferCount()
{
	if (RenderedDepthTextureList.size() > 0)
	{
		return RenderedDepthTextureList.size();
	}
	else
	{
		return 1;
	}
}

uint32_t TextureManager::GetRenderedCubeMapTextureBufferCount()
{
	if (RenderedCubeMapTextureList.size() > 0)
	{
		return RenderedCubeMapTextureList.size();
	}
	else
	{
		return 1;
	}
}

uint32_t TextureManager::GetRenderedCubeMapDepthTextureBufferCount()
{
	if (RenderedCubeMapDepthTextureList.size() > 0)
	{
		return RenderedCubeMapDepthTextureList.size();
	}
	else
	{
		return 1;
	}
}
