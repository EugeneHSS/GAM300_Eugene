/*!*****************************************************************************
 * \file          TextureInfo.h
 * \author        Eugene Ho Shu Sheng
 * \par DP email: shushengeugene.ho@digipen.edu
 * \par Course:   CSD3400
 * \par Section:  A
 * \date          01/10/2023
 * \brief         Loading of texture using TinyDDS
 *******************************************************************************/
#pragma once
#include <stdarg.h>
#include <vulkan/vulkan.h>
#include "dds/tinyddsloader.h"
#include "dotnet/ImportExport.h"
namespace TDS
{
	/*!*************************************************************************
	 * Loading of texture using TinyDDS
	 ***************************************************************************/

	enum class CUBEFACE
	{
		LEFT,
		RIGHT,
		UP,
		DOWN,
		FRONT,
		BACK
	};
	struct TextureData
	{
		tinyddsloader::DDSFile			m_TextureLoaded;
		std::unordered_map<CUBEFACE, const tinyddsloader::DDSFile::ImageData*> m_CubeMapTexture;
		bool DLL_API LoadTexture(std::string_view path);

	};

	struct TextureInfo
	{
		VkSamplerAddressMode			m_USampleAddressMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		VkSamplerAddressMode			m_VSampleAddressMode = VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		VkBorderColor					m_BorderClr = VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_WHITE;
		VkFilter						m_ImageFilter = VkFilter::VK_FILTER_LINEAR;
		std::uint32_t					mipCount = 1;
		VkFormat						m_Format;
		VkImageType						m_ImageType = VK_IMAGE_TYPE_2D;
		VkExtent2D						m_ExtentDimen = { 50, 50 };
		size_t							m_ImageSize{ 0 };
		void* m_Data = nullptr;
		void* m_CustomTextureID = nullptr;
		bool							m_IsCubeMap = false;
		bool							m_UsingMipMaps = false;
		bool							m_StorageImage = false;
		bool							m_FlipTextureY = false;
		bool							m_UseAnistrophy = true;
	};

	class VulkanTexture;

	struct Texture
	{
		void DLL_API					LoadTexture(std::string_view path);
		bool DLL_API					IsTextureCompressed();
		VkFormat DLL_API				ConvertToVulkanFormat(tinyddsloader::DDSFile::DXGIFormat format);
		size_t DLL_API					ComputeImageSize();
		size_t DLL_API					GetBlockByteSize();
		size_t DLL_API					GetByteSize();
		void DLL_API					LoadCubeMapTexture();
		void DLL_API					Destroy();
		VulkanTexture*					m_VulkanTexture = nullptr;

		TextureData						m_Data;
		TextureInfo						m_TextureInfo;
	};




}