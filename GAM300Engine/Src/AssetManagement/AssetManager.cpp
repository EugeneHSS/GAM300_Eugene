#include "AssetManagement/AssetManager.h"
#include "Rendering/Revamped/MaterialManager.h"
#include "Serialization/JSONSerializer.h"
namespace TDS
{


	AssetManager::AssetManager()
	{
	
	}
	AssetManager::~AssetManager()
	{
	}
	void AssetManager::Init()
	{
	}
	void AssetManager::PreloadAssets()
	{
		m_MeshFactory.Preload();
		m_TextureFactory.Preload();
		m_FontFactory.Preload();
		m_AnimationFactory.Preload();

	}

	void AssetManager::ShutDown()
	{
		m_MeshFactory.DestroyAllMesh();

		m_TextureFactory.DestroyAllTextures();
		m_FontFactory.DestroyAllFonts();
		m_AnimationFactory.DestryoAllAnimation();

	}

	void DLL_API AssetManager::ResetReferences()
	{
		m_MeshFactory.GetReferenceCounts().clear();
		m_TextureFactory.GetReferenceCounts().clear();
		m_AnimationFactory.GetReferenceCounts().clear();
	}



	AssetFactory<Texture>& AssetManager::GetTextureFactory()
	{
		return m_TextureFactory;
	}

	AssetFactory<FontAtlas>& AssetManager::GetFontFactory()
	{
		return m_FontFactory;
	}

	AssetFactory<MeshController>& AssetManager::GetMeshFactory()
	{
		return m_MeshFactory;
	}

	AssetFactory<AnimationData>& AssetManager::GetAnimationFactory()
	{
		return m_AnimationFactory;
	}

	std::shared_ptr<AssetManager> AssetManager::GetInstance()
	{
		if (m_Instance == nullptr)
			m_Instance = std::make_shared<AssetManager>();
		return m_Instance;
	}

#define SCENE_ASSET_PATH "../assets/sceneAssetInfo/"

	void AssetManager::SerializeMetaData(std::string_view SceneName)
	{
		for (auto& [name,refCnt] : m_MeshFactory.GetReferenceCounts())
		{
			if (refCnt > 0)
			{
				m_AssetMeta.m_ModelName.emplace_back(name);
			}
		}

		for (auto& [name, refCnt] : m_TextureFactory.GetReferenceCounts())
		{
			if (refCnt > 0)
			{
				m_AssetMeta.m_TextureNames.emplace_back(name);
			}
		}

		for (auto& [name, refCnt] : m_AnimationFactory.GetReferenceCounts())
		{
			if (refCnt > 0)
			{
				m_AssetMeta.m_AnimationName.emplace_back(name);
			}
		}
		std::string OutputFile = SCENE_ASSET_PATH;
		OutputFile += SceneName;
		OutputFile += ".json";
		JSONSerializer serializer;

		if (serializer.Open(OutputFile, false) == JSONSerializer::ERROR_TYPE::SUCCESS)
		{
			serializer.StartSerializer(&m_AssetMeta);
			
			serializer.CloseData(false);
		}
	}



}