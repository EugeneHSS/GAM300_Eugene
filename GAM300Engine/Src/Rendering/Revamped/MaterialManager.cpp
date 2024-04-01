#include "Rendering/Revamped/MaterialManager.h"
#include "AssetManagement/AssetManager.h"
#include "Serialization/JSONSerializer.h"

#define MAX_MATERIALS_TO_LOAD 600;
namespace TDS
{

	void MaterialManager::InsertMaterialPipeline(MaterialAttributes::SHADING_TYPE shadingType, std::shared_ptr<VulkanPipeline> pipeline)
	{
		m_MaterialShaders.insert(std::make_pair(shadingType, pipeline));
	}

	void MaterialManager::PreloadMaterialInfos()
	{
		auto getSamplerAddressMode = [](int mode) -> VkSamplerAddressMode
		{
			switch (mode)
			{
			case 0: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
			case 1: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case 2: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			case 3: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

			default: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			}
		};

		auto RemoveFileExtension = [](std::string& fileName)
		{
			size_t lastindex = fileName.find_last_of(".");
			fileName = fileName.substr(0, lastindex);
		};

		auto& texFactory = AssetManager::GetInstance()->GetTextureFactory();

		std::uint32_t NumOfLoadedTexture = 0;
		std::filesystem::path dir = MATERIAL_PATH;

		if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir))
		{
			std::cout << "Invalid directory" << std::endl;
			return;
		}

		for (const auto& entry : std::filesystem::directory_iterator(dir))
		{
			const std::filesystem::path& path = entry.path();
			if (path.extension() == ".json")
			{
				std::string fileName = path.filename().string();

				MaterialData newData{};
				MaterialData::Serialize(newData, path.string(), true);
				m_ModelToMaterials[newData.m_ModelName] = newData;

				for (auto& texMat : newData.m_TextureToload)
				{
					TextureInfo ReloadInfo{};
					ReloadInfo.m_USampleAddressMode = getSamplerAddressMode(texMat.m_UMode);
					ReloadInfo.m_USampleAddressMode = getSamplerAddressMode(texMat.m_VMode);

					std::filesystem::path fileName(texMat.m_TexturePath);
					std::string fileNameStr = fileName.string();
					//RemoveFileExtension(fileNameStr);

					std::string textureFullPath = TEXTURE_PATH + fileNameStr;

					texFactory.LoadTextureWithInfo(textureFullPath, ReloadInfo);
				}


			}
		}
		m_UpdateMaterialList = true;

	}
	VulkanPipeline& MaterialManager::GetMaterialPipeline(MaterialAttributes::SHADING_TYPE shading)
	{
		return *m_MaterialShaders[shading];
	}
	MaterialData* MaterialManager::GetMaterialData(std::string_view ModelName)
	{
		auto finditr = m_ModelToMaterials.find(ModelName.data());

		if (finditr != m_ModelToMaterials.end())
		{
			return &finditr->second;
		}

		return nullptr;
	}

	void MaterialManager::LoadMaterial(std::string_view MaterialPath)
	{
		auto getSamplerAddressMode = [](int mode) -> VkSamplerAddressMode
		{
			switch (mode)
			{
			case 0: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_REPEAT;
			case 1: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case 2: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			case 3: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

			default: return VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			}
		};


		auto RemoveFileExtension = [](std::string& fileName)
		{
			size_t lastindex = fileName.find_last_of(".");
			fileName = fileName.substr(0, lastindex);
		};

		auto& texFactory = AssetManager::GetInstance()->GetTextureFactory();
		MaterialData newData{};
		MaterialData::Serialize(newData, MaterialPath, true);
		m_ModelToMaterials[newData.m_ModelName] = newData;

		for (auto& texMat : newData.m_TextureToload)
		{
			TextureInfo ReloadInfo{};
			ReloadInfo.m_USampleAddressMode = getSamplerAddressMode(texMat.m_UMode);
			ReloadInfo.m_USampleAddressMode = getSamplerAddressMode(texMat.m_VMode);

			std::filesystem::path fileName(texMat.m_TexturePath);
			std::string fileNameStr = fileName.string();
			//RemoveFileExtension(fileNameStr);

			std::string textureFullPath = TEXTURE_PATH + fileNameStr/* + ".dds"*/;

			texFactory.LoadTextureWithInfo(textureFullPath, ReloadInfo);
		}
		m_UpdateMaterialList = true;
	}

	void MaterialManager::ShutDown()
	{
		m_ModelToMaterials.clear();
		m_MaterialShaders.clear();
	}
}