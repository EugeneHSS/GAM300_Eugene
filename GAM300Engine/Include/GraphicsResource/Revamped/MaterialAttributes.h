#pragma once
#include <rttr/registration.h>
#include <rttr/registration_friend.h>
namespace TDS
{
#define ADD_SHADING_ENUM(x, y) x = y,

#define MATERIAL_SHADING_LIST\
	ADD_SHADING_ENUM(SHADING_NONE, 0)\
	ADD_SHADING_ENUM(SHADING_PHONG_BLINN, 1)\
	ADD_SHADING_ENUM(SHADING_PBR,2)

#define MATERIAL_PATH "../assets/materialsInfo/"


	struct PBR
	{
		std::string		m_NormalTex;
		std::string		m_RoughnessTex;
		std::string		m_MetallicTex;
		std::string		m_EmissiveTex;
		std::string		m_AOTex;

		float			m_Metalness;
		float			m_Roughness;
		float			m_Emissive;
		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND

	};

	struct PhongBlinn
	{
		std::string		m_DiffuseTex;
		std::string		m_SpecularTex;
		std::string		m_AmbientTex;
		std::string		m_EmissiveTex;
		std::string		m_ShininessTex;

		Vec3			m_DiffuseColor;
		Vec3			m_SpecularColor;
		Vec3			m_EmissiveColor;
		Vec3			m_AmbientColor;
		float			m_Shininess;
		float			m_ShininessStrength;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};

	struct MaterialAttributes
	{

		enum SHADING_TYPE : int
		{
			MATERIAL_SHADING_LIST
		};



		SHADING_TYPE	m_shading = SHADING_NONE;
		PBR				m_PBRAttributes;
		PhongBlinn		m_phongBlinn;
		std::string		m_AlbedoTex;
		std::string		m_MaterialName;

		//Other properties that we might need
		float			m_Reflectivity;
		Vec3			m_Reflectance;
		std::string		m_ReflectanceTexture;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
		

	};





	struct TextureToLoad
	{
		std::string m_TexturePath;
		int m_UMode, m_VMode;

		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};

	struct MaterialData
	{
		std::map<int, MaterialAttributes>				m_materialAttributes;
		std::vector<TextureToLoad>						m_TextureToload;
		std::string										m_ModelName;

		static DLL_API bool Serialize(MaterialData& materials, std::string_view path, bool read = false);
		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND
	};
}