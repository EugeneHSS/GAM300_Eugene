/*!*****************************************************************************
 * \file          TextureInfo.cpp
 * \author        Eugene Ho Shu Sheng
 * \par DP email: shushengeugene.ho@digipen.edu
 * \par Course:   CSD3400
 * \par Section:  A
 * \date          01/10/2023
 * \brief         This file contains the implementation of the TextureInfo class.
 *******************************************************************************/
#define TINYDDSLOADER_IMPLEMENTATION
#include "GraphicsResource/TextureInfo.h"
#include "vulkanTools/VulkanTexture.h"
#include <iostream>


namespace TDS
{
	bool TextureData::LoadTexture(std::string_view path)
	{

		tinyddsloader::Result result = m_TextureLoaded.Load(path.data());
		return (result == tinyddsloader::Result::Success);

	}

	void Texture::LoadTexture(std::string_view path)
	{
		if (m_VulkanTexture != nullptr)
		{
			Destroy();
			delete m_VulkanTexture;
			m_VulkanTexture = nullptr;

		}
		if (!m_Data.LoadTexture(path))
		{
			TDS_ERROR("Failed to load texture {}", path.data());
			return;
		}
		m_TextureInfo.m_Format = ConvertToVulkanFormat(m_Data.m_TextureLoaded.GetFormat());
		m_TextureInfo.m_ImageSize = ComputeImageSize();
		m_TextureInfo.m_ExtentDimen = { m_Data.m_TextureLoaded.GetWidth(), m_Data.m_TextureLoaded.GetHeight() };
		m_TextureInfo.mipCount = m_Data.m_TextureLoaded.GetMipCount();
		m_TextureInfo.m_Data = m_Data.m_TextureLoaded.GetImageData()->m_mem;
		m_TextureInfo.m_IsCubeMap = m_Data.m_TextureLoaded.IsCubemap();

		m_VulkanTexture = new VulkanTexture();
		if (m_TextureInfo.m_IsCubeMap)
		{
			uint32_t size = m_Data.m_TextureLoaded.GetSize();
			m_TextureInfo.m_ImageSize = m_Data.m_TextureLoaded.GetSize();
			m_VulkanTexture->CreateCubeMapTexture(m_TextureInfo, m_Data);
		}
		else
		{
			m_TextureInfo.m_FlipTextureY = m_Data.m_TextureLoaded.Flip();
			m_VulkanTexture->CreateTexture(m_TextureInfo);
		}

	}
	bool DLL_API Texture::IsTextureCompressed()
	{
		switch (m_TextureInfo.m_Format)
		{
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
		case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
		case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
		case VK_FORMAT_BC2_UNORM_BLOCK:
		case VK_FORMAT_BC2_SRGB_BLOCK:
		case VK_FORMAT_BC3_UNORM_BLOCK:
		case VK_FORMAT_BC3_SRGB_BLOCK:
		case VK_FORMAT_BC4_UNORM_BLOCK:
		case VK_FORMAT_BC4_SNORM_BLOCK:
		case VK_FORMAT_BC5_UNORM_BLOCK:
		case VK_FORMAT_BC5_SNORM_BLOCK:
		case VK_FORMAT_BC6H_UFLOAT_BLOCK:
		case VK_FORMAT_BC6H_SFLOAT_BLOCK:
		case VK_FORMAT_BC7_UNORM_BLOCK:
		case VK_FORMAT_BC7_SRGB_BLOCK:
			return true;
		default:
			return false;
		}
	}
	VkFormat Texture::ConvertToVulkanFormat(tinyddsloader::DDSFile::DXGIFormat format)
	{
		switch (format)
		{
		case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_Float:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_UInt:
			return VK_FORMAT_R32G32B32A32_UINT;
		case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_SInt:
			return VK_FORMAT_R32G32B32A32_SINT;
		case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_Float:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_UInt:
			return VK_FORMAT_R32G32B32_UINT;
		case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_SInt:
			return VK_FORMAT_R32G32B32_SINT;
		case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_Float:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_UNorm:
			return VK_FORMAT_R16G16B16A16_UNORM;
		case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_UInt:
			return VK_FORMAT_R16G16B16A16_UINT;
		case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_SNorm:
			return VK_FORMAT_R16G16B16A16_SNORM;
		case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_SInt:
			return VK_FORMAT_R16G16B16A16_SINT;
		case tinyddsloader::DDSFile::DXGIFormat::R32G32_Float:
			return VK_FORMAT_R32G32_SFLOAT;
		case tinyddsloader::DDSFile::DXGIFormat::R32G32_UInt:
			return VK_FORMAT_R32G32_UINT;
		case tinyddsloader::DDSFile::DXGIFormat::R32G32_SInt:
			return VK_FORMAT_R32G32_SINT;
		case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_UNorm:
			return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
		case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_UInt:
			return VK_FORMAT_A2B10G10R10_UINT_PACK32;
		case tinyddsloader::DDSFile::DXGIFormat::R11G11B10_Float:
			return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm_SRGB:
			return VK_FORMAT_R8G8B8A8_SRGB;
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UInt:
			return VK_FORMAT_R8G8B8A8_UINT;
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SNorm:
			return VK_FORMAT_R8G8B8A8_SNORM;
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SInt:
			return VK_FORMAT_R8G8B8A8_SINT;
		case tinyddsloader::DDSFile::DXGIFormat::R16G16_Float:
			return VK_FORMAT_R16G16_SFLOAT;
		case tinyddsloader::DDSFile::DXGIFormat::R16G16_UNorm:
			return VK_FORMAT_R16G16_UNORM;
		case tinyddsloader::DDSFile::DXGIFormat::R16G16_UInt:
			return VK_FORMAT_R16G16_UINT;
		case tinyddsloader::DDSFile::DXGIFormat::R16G16_SNorm:
			return VK_FORMAT_R16G16_SNORM;
		case tinyddsloader::DDSFile::DXGIFormat::R16G16_SInt:
			return VK_FORMAT_R16G16_SINT;
		case tinyddsloader::DDSFile::DXGIFormat::R32_Float:
			return VK_FORMAT_R32_SFLOAT;
		case tinyddsloader::DDSFile::DXGIFormat::R32_UInt:
			return VK_FORMAT_R32_UINT;
		case tinyddsloader::DDSFile::DXGIFormat::R32_SInt:
			return VK_FORMAT_R32_SINT;
		case tinyddsloader::DDSFile::DXGIFormat::R8G8_UNorm:
			return VK_FORMAT_R8G8_UNORM;
		case tinyddsloader::DDSFile::DXGIFormat::R8G8_UInt:
			return VK_FORMAT_R8G8_UINT;
		case tinyddsloader::DDSFile::DXGIFormat::R8G8_SNorm:
			return VK_FORMAT_R8G8_SNORM;
		case tinyddsloader::DDSFile::DXGIFormat::R8G8_SInt:
			return VK_FORMAT_R8G8_SINT;
		case tinyddsloader::DDSFile::DXGIFormat::R16_Float:
			return VK_FORMAT_R16_SFLOAT;
		case tinyddsloader::DDSFile::DXGIFormat::R16_UNorm:
			return VK_FORMAT_R16_UNORM;
		case tinyddsloader::DDSFile::DXGIFormat::R16_UInt:
			return VK_FORMAT_R16_UINT;
		case tinyddsloader::DDSFile::DXGIFormat::R16_SNorm:
			return VK_FORMAT_R16_SNORM;
		case tinyddsloader::DDSFile::DXGIFormat::R16_SInt:
			return VK_FORMAT_R16_SINT;
		case tinyddsloader::DDSFile::DXGIFormat::R8_UNorm:
			return VK_FORMAT_R8_UNORM;
		case tinyddsloader::DDSFile::DXGIFormat::R8_UInt:
			return VK_FORMAT_R8_UINT;
		case tinyddsloader::DDSFile::DXGIFormat::R8_SNorm:
			return VK_FORMAT_R8_SNORM;
		case tinyddsloader::DDSFile::DXGIFormat::R8_SInt:
			return VK_FORMAT_R8_SINT;
		case tinyddsloader::DDSFile::DXGIFormat::A8_UNorm:
			return VK_FORMAT_R8_UNORM;
		case tinyddsloader::DDSFile::DXGIFormat::R9G9B9E5_SHAREDEXP:
			return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
		case tinyddsloader::DDSFile::DXGIFormat::R8G8_B8G8_UNorm:
			return VK_FORMAT_G8B8G8R8_422_UNORM;
		case tinyddsloader::DDSFile::DXGIFormat::G8R8_G8B8_UNorm:
			return VK_FORMAT_B8G8R8G8_422_UNORM;
		case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm:
			return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm_SRGB:
			return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
		case tinyddsloader::DDSFile::DXGIFormat::BC2_UNorm:
			return VK_FORMAT_BC2_UNORM_BLOCK;
		case tinyddsloader::DDSFile::DXGIFormat::BC2_UNorm_SRGB:
			return VK_FORMAT_BC2_SRGB_BLOCK;
		case tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm:
			return VK_FORMAT_BC3_UNORM_BLOCK;
		case tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm_SRGB:
			return VK_FORMAT_BC3_SRGB_BLOCK;
		case tinyddsloader::DDSFile::DXGIFormat::BC4_UNorm:
			return VK_FORMAT_BC4_UNORM_BLOCK;
		case tinyddsloader::DDSFile::DXGIFormat::BC4_SNorm:
			return VK_FORMAT_BC4_SNORM_BLOCK;
		case tinyddsloader::DDSFile::DXGIFormat::BC5_UNorm:
			return VK_FORMAT_BC5_UNORM_BLOCK;
		case tinyddsloader::DDSFile::DXGIFormat::BC5_SNorm:
			return VK_FORMAT_BC5_SNORM_BLOCK;
		case tinyddsloader::DDSFile::DXGIFormat::BC6H_UF16:
			return VK_FORMAT_BC6H_UFLOAT_BLOCK;
		case tinyddsloader::DDSFile::DXGIFormat::BC6H_SF16:
			return VK_FORMAT_BC6H_SFLOAT_BLOCK;
		case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm:
			return VK_FORMAT_BC7_UNORM_BLOCK;
		case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm_SRGB:
			return VK_FORMAT_BC7_SRGB_BLOCK;
		default:
			return VK_FORMAT_UNDEFINED;
		}
	}
	size_t DLL_API Texture::ComputeImageSize()
	{
		size_t totalSize = 0;

		std::uint32_t mipCount = m_TextureInfo.m_UsingMipMaps ? m_Data.m_TextureLoaded.GetMipCount() : 1;
		std::uint32_t i = 0;
		for (; i < mipCount; ++i)
		{
			if (IsTextureCompressed())
			{
				size_t blockSize = GetBlockByteSize();
				std::uint32_t numBlocksWide = std::max<std::uint32_t>(1U, (m_Data.m_TextureLoaded.GetWidth() + 3) / 4);
				std::uint32_t numBlocksHeight = std::max<std::uint32_t>(1U, (m_Data.m_TextureLoaded.GetHeight() + 3) / 4);
				totalSize += numBlocksWide * numBlocksHeight * blockSize;
			}
			else
			{
				size_t pixelSize = GetByteSize();
				totalSize += m_Data.m_TextureLoaded.GetWidth() * m_Data.m_TextureLoaded.GetHeight() * pixelSize;
			}

		}

		return totalSize;


	}
	size_t DLL_API Texture::GetBlockByteSize()
	{
		//For compressed texture only
		switch (m_TextureInfo.m_Format)
		{
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
		case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
		case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
		case VK_FORMAT_BC4_UNORM_BLOCK:
		case VK_FORMAT_BC4_SNORM_BLOCK:
			return 8;
		case VK_FORMAT_BC2_UNORM_BLOCK:
		case VK_FORMAT_BC2_SRGB_BLOCK:
		case VK_FORMAT_BC3_UNORM_BLOCK:
		case VK_FORMAT_BC3_SRGB_BLOCK:
		case VK_FORMAT_BC5_UNORM_BLOCK:
		case VK_FORMAT_BC5_SNORM_BLOCK:
		case VK_FORMAT_BC6H_UFLOAT_BLOCK:
		case VK_FORMAT_BC6H_SFLOAT_BLOCK:
		case VK_FORMAT_BC7_UNORM_BLOCK:
		case VK_FORMAT_BC7_SRGB_BLOCK:
			return 16;
		default:
			return 0;
		}
	}
	size_t DLL_API Texture::GetByteSize()
	{
		//Only non compressed texture
		switch (m_TextureInfo.m_Format)
		{
		case VK_FORMAT_R32G32_UINT:
		case VK_FORMAT_R32G32_SINT:
		case VK_FORMAT_R32G32_SFLOAT:
		case VK_FORMAT_R16G16B16A16_UNORM:
		case VK_FORMAT_R16G16B16A16_UINT:
		case VK_FORMAT_R16G16B16A16_SINT:
		case VK_FORMAT_R16G16B16A16_SFLOAT:
			return 8;
		case VK_FORMAT_R32G32B32A32_UINT:
		case VK_FORMAT_R32G32B32A32_SINT:
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			return 16;
		case VK_FORMAT_R8G8B8_UNORM:
		case VK_FORMAT_R8G8B8_SRGB:
			return 3;
		case VK_FORMAT_R8G8_UNORM:
		case VK_FORMAT_R16_UNORM:
		case VK_FORMAT_R16_UINT:
		case VK_FORMAT_R16_SINT:
		case VK_FORMAT_R16_SFLOAT:
			return 2;
		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R8G8B8A8_SRGB:
		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_R16G16_UNORM:
		case VK_FORMAT_R16G16_UINT:
		case VK_FORMAT_R16G16_SINT:
		case VK_FORMAT_R16G16_SFLOAT:
		case VK_FORMAT_R32_UINT:
		case VK_FORMAT_R32_SINT:
		case VK_FORMAT_R32_SFLOAT:
		case VK_FORMAT_G8B8G8R8_422_UNORM:
		case VK_FORMAT_B8G8R8G8_422_UNORM:
			return 4;
		case VK_FORMAT_R8_UNORM:
			return 1;
		default:
			return 0;



		}
	}

	void DLL_API Texture::LoadCubeMapTexture()
	{
		if (!m_Data.m_TextureLoaded.IsCubemap())
			return;



		for (uint32_t i = 0; i < 6; i++)
		{
			const tinyddsloader::DDSFile::ImageData* faceData = m_Data.m_TextureLoaded.GetImageData(0, i);

			if (faceData != nullptr)
				m_Data.m_CubeMapTexture[static_cast<CUBEFACE>(i)] = faceData;

		}


	}

	void DLL_API Texture::Destroy()
	{
		if (m_VulkanTexture != nullptr)
		{
			m_VulkanTexture->Destroy();
			delete m_VulkanTexture;
			m_VulkanTexture = nullptr;
		}
	}

}