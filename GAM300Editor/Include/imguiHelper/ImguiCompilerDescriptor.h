#pragma once

#include "ImguiHelper.h"
#include "imguiHelper/ImguiHierarchy.h"
#include "Tools/TextureCompressor.h"
namespace TDS
{




	class CompilerDescriptors : public LevelEditorPanel
	{
	public:

		CompilerDescriptors();
		~CompilerDescriptors();
		void		init();
		void		update();
		

		enum DESCRIPTOR_TYPE
		{
			DESC_GEOMETRY = 0,
			DESC_TEXTURE,
			DESC_TOTAL
		};

		void		DisplaySettings(DESCRIPTOR_TYPE descType);


		struct CompilerDisplay
		{
			bool				m_IsSelected = false;	
			DESCRIPTOR_TYPE		m_DescType = DESC_TOTAL;
		};

		struct GeomDescDisplay : public CompilerDisplay
		{
			GeomDescDisplay()
			{
				m_IsSelected = false;
				m_DescType = DESC_GEOMETRY;
			}

			GeomDescriptor		m_GeomDecriptor{};
		};


		struct TextureDescDisplay : public CompilerDisplay
		{
			TextureDescDisplay()
			{
				m_IsSelected = false;
				m_DescType = DESC_TEXTURE;
			}
			TextureCompilerSetting	m_TextureDescriptor{};
		};

		std::array<CompilerDisplay*, DESC_TOTAL>	m_CompilerDescriptors;


	};

}