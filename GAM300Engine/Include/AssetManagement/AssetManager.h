#pragma once
/*!*************************************************************************
****
\file AssetManager.h
\author Ho Shu Sheng Eugene
\par DP email: shushengeugene.ho@digipen.edu
\par Course: CSD3400
\par Section: a
\date 22-9-2023
\brief  Function definitions of the Pipeline Class
****************************************************************************
***/
#include "TextureFactory.h"
#include "FontFactory.h"
#include "Revamped/MeshFactory.h"
#include "dotnet/ImportExport.h"
#include "Identifier/UniqueID.h"

namespace TDS
{



	class AssetManager
	{
	private:
		inline static std::shared_ptr<AssetManager> m_Instance = nullptr;
		AssetFactory<Texture>						m_TextureFactory;
		AssetFactory<FontAtlas>						m_FontFactory;
		AssetFactory<MeshController>				m_MeshFactory;
	public:
		DLL_API AssetManager();
		DLL_API ~AssetManager();
		void DLL_API Init();
		void DLL_API PreloadAssets();
		void DLL_API ShutDown();

		DLL_API AssetFactory<Texture>&	   			GetTextureFactory();
		DLL_API AssetFactory<FontAtlas>&  			GetFontFactory();
		DLL_API AssetFactory<MeshController>&		GetMeshFactory();
		static DLL_API std::shared_ptr<AssetManager> GetInstance();



	};
}