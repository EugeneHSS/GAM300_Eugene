#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#include "MathCommon.h"
#include <rttr/registration.h>
#include <rttr/registration_friend.h>
namespace TDS
{


#define ADD_MATERIAL_TYPE(x, y) MATERIAL_##x = y,


#define DEFINE_MATERIAL_TYPE_LIST \
	ADD_MATERIAL_TYPE(DIFFUSE, 1)\
	ADD_MATERIAL_TYPE(SPECULAR, 2)\
	ADD_MATERIAL_TYPE(AMBIENT, 3)\
	ADD_MATERIAL_TYPE(EMISSIVE, 4)\
	ADD_MATERIAL_TYPE(HEIGHT, 5)\
	ADD_MATERIAL_TYPE(NORMALS, 6)\
	ADD_MATERIAL_TYPE(SHININESS, 8)\
	ADD_MATERIAL_TYPE(OPACITY,8)\
	ADD_MATERIAL_TYPE(DISPLACEMENT, 9)\
	ADD_MATERIAL_TYPE(LIGHTMAP, 10)\
	ADD_MATERIAL_TYPE(REFLECTION, 11)\
	ADD_MATERIAL_TYPE(PBR_ALBEDO, 12)\
	ADD_MATERIAL_TYPE(PBR_NORMAL, 13)\
	ADD_MATERIAL_TYPE(PBR_EMISSION, 14)\
	ADD_MATERIAL_TYPE(PBR_METALNESS, 15)\
	ADD_MATERIAL_TYPE(PBR_ROUGHNESS, 16)\
	ADD_MATERIAL_TYPE(PBR_OCCLUSION, 17)\
	ADD_MATERIAL_TYPE(SHEEN, 19)\
	ADD_MATERIAL_TYPE(CLEARCOAT,20)\
	ADD_MATERIAL_TYPE(TRANSMISSION,21)




	typedef enum : std::uint16_t
	{
		DEFINE_MATERIAL_TYPE_LIST
	}
	MATERIALS_TEXTURE;


#undef ADD_MATERIAL_TYPE

#define ADD_MATERIAL_TYPE(x, y)\
		materialMapList[y] = #x;




	struct MaterialMapList
	{
		static auto& Get()
		{
			static std::map<int, std::string> materialMapList;

			if (materialMapList.empty())
				DEFINE_MATERIAL_TYPE_LIST

				return materialMapList;
		}
	};


	/*struct MaterialInfo
	{
		std::string		m_MaterialName;
		size_t			m_MaterialGUID;
	};



	struct MaterialList
	{
		std::vector<MaterialInfo> m_MaterialInfos;
	};


	enum class SHADING_TYPE
	{
		PHONG_BLINN = 0,
		PBR,
		GOURAUD,
		TOOL,
		UNLIGHT,
		UNKNOWN,
	};

	enum MATERIAL_COLOR
	{
		MATERIAL_COLOR_DIFFUSE = 0,
		MATERIAL_COLOR_SPECULAR,
		MATERIAL_COLOR_AMBIENT,
		MATERIAL_COLOR_EMISSIVE,
	};

	enum MATERIAL_FLOAT
	{
		MATERIAL_FLOAT_OPACITY = 0,
		MATERIAL_FLOAT_SHININESS,
		MATERIAL_FLOAT_SHININESS_STRENGTH
	};


	enum class ADDRESS_MODE : std::uint8_t
	{
		TILE, MIRROR, CLAMP
	};


	struct MaterialTextureInfo
	{
		ADDRESS_MODE	m_UMode;
		ADDRESS_MODE	m_VMode;
		std::string		m_TextureName;

	};

	struct MaterialProperty
	{
		SHADING_TYPE											m_ShadingType;
		int														m_MatID;
		std::size_t												m_GUID;
		std::string												m_MaterialName;
		std::unordered_map<MATERIAL_FLOAT, float>				m_MaterialFloat;
		std::unordered_map<MATERIAL_COLOR, Vec4>				m_MaterialColor;
		std::unordered_map<MATERIALS_TEXTURE, std::vector<MaterialTextureInfo>>	m_TexturesPath;

		static bool Serialize(MaterialProperty& materials, std::string_view path, bool read = false);

	};

	struct MaterialLoader
	{
		std::vector<MaterialProperty>				m_MaterialInfos;

		DLL_API static bool Serialize(MaterialLoader& materials, std::string_view path, bool read = false);

	};*/


}
#endif