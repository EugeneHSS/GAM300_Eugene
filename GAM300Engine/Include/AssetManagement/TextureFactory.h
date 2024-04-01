#pragma once
#include "FactoryBase.h"
#include "../GraphicsResource/TextureInfo.h"
#define MAX_NUM_PRELOAD_TEXTURE 500
#define TEXTURE_PATH "../assets/textures/"
namespace TDS
{

	template<>
	class AssetFactory<Texture>
	{

	public:

		std::array<Texture, 1000> m_TextureArray;
		std::unordered_map<std::string, std::uint32_t> m_TextureIndices;
		std::unordered_map<std::string, std::uint32_t> m_InstanceCnt;
		std::uint32_t m_CurrentIndex = 0;
		bool m_UpdateTextureArray3D = true;
		bool m_UpdateArrayBatch = true;
		bool m_UpdateArrayInstance = true;
		bool m_UpdateTextureArray2D = true;
		/*void Preload(ResourceAllocator& resourceMgr)
		{
			std::uint32_t NumOfLoadedTexture = 0;
			std::filesystem::path dir = TEXTURE_PATH;

			if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir))
			{
				std::cout << "Invalid directory" << std::endl;
				return;
			}

			for (const auto& entry : std::filesystem::directory_iterator(dir))
			{
				if (NumOfLoadedTexture >= MAX_NUM_PRELOAD_TEXTURE)
					break;
				const std::filesystem::path& path = entry.path();
				if (path.extension() == ".dds")
				{
					TypeReference<Texture> TextureInstance{};

					std::string fileName = path.filename().string();

					TextureInstance.m_AssetName = fileName;

					auto newTexture = resourceMgr.LoadResource(TextureInstance);
					if (newTexture == nullptr)
					{
						std::cout << "Failed to get texture instance!" << std::endl;
						continue;
					}
					newTexture->LoadTexture(path.string());
				}
			}
		}*/
		std::array<Texture, 1000>& GetTextureArray()
		{
			return m_TextureArray;
		}

		std::unordered_map<std::string, std::uint32_t>& GetReferenceCounts()
		{
			return m_InstanceCnt;
		}


		void LoadTextureWithInfo(std::string_view path, TextureInfo& info)
		{
			std::filesystem::path FilePath(path);
			std::string fileName = FilePath.filename().string();


			auto itr = m_TextureIndices.find(fileName);
			if (itr != m_TextureIndices.end())
			{
				/*if (m_InstanceCnt[fileName] == 0)
				{*/
				std::uint32_t index = m_TextureIndices[fileName];

				m_TextureArray[index].Destroy();
				m_TextureArray[index].m_TextureInfo = info;
				m_TextureArray[index].LoadTexture(path);
				m_UpdateTextureArray3D = true;
				m_UpdateTextureArray2D = true;
				m_UpdateArrayInstance = true;
				m_UpdateArrayBatch = true;

				return;
			}
			std::uint32_t& newIndex = m_CurrentIndex;
			m_TextureArray[newIndex].m_TextureInfo = info;
			m_TextureArray[newIndex].LoadTexture(path);
			m_UpdateTextureArray3D = true;
			m_UpdateTextureArray2D = true;
			m_TextureIndices[fileName] = newIndex++;
		}

		void Preload()
		{
			std::uint32_t NumOfLoadedTexture = 0;
			std::filesystem::path dir = TEXTURE_PATH;

			if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir))
			{
				std::cout << "Invalid directory" << std::endl;
				return;
			}

			for (const auto& entry : std::filesystem::directory_iterator(dir))
			{
				if (NumOfLoadedTexture >= MAX_NUM_PRELOAD_TEXTURE)
					break;
				const std::filesystem::path& path = entry.path();
				if (path.extension() == ".dds")
				{
					if (path.filename().string() == "skybox1.dds")
					{
						continue;
					}
					std::string fileName = path.filename().string();
					m_TextureArray[m_CurrentIndex].LoadTexture(path.string());
					m_TextureIndices[fileName] = m_CurrentIndex++;
				}
			}
			m_UpdateTextureArray3D = true;
			m_UpdateTextureArray2D = true;
			m_UpdateArrayInstance = true;
			m_UpdateArrayBatch = true;
		}

		//static void Load(std::string_view path, TypeReference<Texture>& textureRef, ResourceAllocator& resourceMgr)
		//{
		//	std::filesystem::path FilePath(path);
		//	std::string fileName = FilePath.filename().string();
		//	textureRef.m_AssetName = fileName;
		//	if (!resourceMgr.GetResource(textureRef))
		//	{
		//		if (!resourceMgr.LoadResource(textureRef))
		//		{
		//			TDS_WARN("Failed to load resource!");
		//			return;
		//		}
		//	}
		//	textureRef.m_ResourcePtr->LoadTexture(path);
		//}
		Texture* GetTexture(std::string_view textureName)
		{
			auto itr = m_TextureIndices.find(textureName.data());
			if (itr != m_TextureIndices.end())
			{
				return &m_TextureArray[itr->second];
			}
			return nullptr;
			TDS_WARN("Texture {} doesnt exist!", textureName.data());
		}
		int GetTextureIndex(std::string_view textureName, TypeReference<Texture>& referenceTexture)
		{
			auto itr = m_TextureIndices.find(textureName.data());
			if (itr != m_TextureIndices.end())
			{
				if (referenceTexture.m_AssetName.empty() == false)
				{
					--m_InstanceCnt[referenceTexture.m_AssetName];
				}
				++m_InstanceCnt[textureName.data()];
				referenceTexture.m_AssetName = textureName;
				return (int)itr->second;
			}
			return -1;
		}
		void Load(std::string_view path)
		{
			std::filesystem::path FilePath(path);
			std::string fileName = FilePath.filename().string();


			auto itr = m_TextureIndices.find(fileName);
			if (itr != m_TextureIndices.end())
			{
				/*if (m_InstanceCnt[fileName] == 0)
				{*/
				std::uint32_t index = m_TextureIndices[fileName];
				m_TextureArray[index].Destroy();
				m_TextureArray[index].LoadTexture(path);
				m_UpdateTextureArray3D = true;
				m_UpdateTextureArray2D = true;
				m_UpdateArrayInstance = true;
				m_UpdateArrayBatch = true;

				return;
				/*	return;
				}
				else
				{
					TDS_INFO("texture {} has {} entity using it.", fileName, m_InstanceCnt[fileName]);
					return;
				}*/
			}
			std::uint32_t& newIndex = m_CurrentIndex;
			m_TextureArray[newIndex].LoadTexture(path);
			m_UpdateTextureArray3D = true;
			m_UpdateTextureArray2D = true;
			m_TextureIndices[fileName] = newIndex++;
		}
		void Load(std::string_view path, TypeReference<Texture>& textureRef)
		{
			std::filesystem::path FilePath(path);
			std::string fileName = FilePath.filename().string();
			textureRef.m_AssetName = fileName;


			auto itr = m_TextureIndices.find(fileName);
			if (itr != m_TextureIndices.end())
			{
				TDS_INFO("texture {} is already loaded!", textureRef.m_AssetName);

				textureRef.m_ResourcePtr = &m_TextureArray[itr->second];
				return;
			}
			std::uint32_t& newIndex = m_CurrentIndex;
			m_TextureArray[newIndex].LoadTexture(path);
			textureRef.m_ResourcePtr = &m_TextureArray[newIndex];
			m_UpdateTextureArray3D = true;
			m_UpdateTextureArray2D = true;
			m_UpdateArrayInstance = true;
			m_UpdateArrayBatch = true;
			m_TextureIndices[fileName] = newIndex++;
		}
		void DestroyAllTextures()
		{
			for (auto& texture : m_TextureArray)
			{
				texture.Destroy();
			}
			m_TextureIndices.clear();
		}

		std::uint32_t GetTextureID(std::string_view TextureName)
		{
			auto itr = m_TextureIndices.find(TextureName.data());

			if (itr != m_TextureIndices.end())
			{
				return itr->second;
			}

			return -1;

		}
	};



}