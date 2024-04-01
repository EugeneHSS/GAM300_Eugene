#include "GraphicsResource/Revamped/MaterialAttributes.h"
#include "Serialization/JSONSerializer.h"
#include "components/CompReflectionMacro.h"


namespace TDS
{
	RTTR_REGISTRATION
	{
		using namespace TDS;

	rttr::registration::enumeration<MaterialAttributes::SHADING_TYPE>("SHADING_TYPE")
		(
			rttr::value("SHADING_NONE", MaterialAttributes::SHADING_NONE),
			rttr::value("SHADING_PHONG_BLINN", MaterialAttributes::SHADING_PHONG_BLINN),
			rttr::value("SHADING_PBR", MaterialAttributes::SHADING_PBR)
			);

			RTTR_REGISTER_WITH_NAME(PBR, "PBR").constructor<>()
				(
					rttr::policy::ctor::as_object
				)
		RTTR_REGISTER_PROPERTY(PBR, NormalTex)
		RTTR_REGISTER_PROPERTY(PBR, RoughnessTex)
		RTTR_REGISTER_PROPERTY(PBR, MetallicTex)
		RTTR_REGISTER_PROPERTY(PBR, EmissiveTex)
		RTTR_REGISTER_PROPERTY(PBR, AOTex)
		RTTR_REGISTER_PROPERTY(PBR, Metalness)
		RTTR_REGISTER_PROPERTY(PBR, Roughness)
		RTTR_REGISTER_PROPERTY(PBR, Emissive);
			

	RTTR_REGISTER_WITH_NAME(PhongBlinn, "PhongBlinn").constructor<>()
		(
			rttr::policy::ctor::as_object
			)
		RTTR_REGISTER_PROPERTY(PhongBlinn, DiffuseTex)
		RTTR_REGISTER_PROPERTY(PhongBlinn, SpecularTex)
		RTTR_REGISTER_PROPERTY(PhongBlinn, AmbientTex)
		RTTR_REGISTER_PROPERTY(PhongBlinn, EmissiveTex)
		RTTR_REGISTER_PROPERTY(PhongBlinn, ShininessTex)
		RTTR_REGISTER_PROPERTY(PhongBlinn, DiffuseColor)
		RTTR_REGISTER_PROPERTY(PhongBlinn, SpecularColor)
		RTTR_REGISTER_PROPERTY(PhongBlinn, EmissiveColor)
		RTTR_REGISTER_PROPERTY(PhongBlinn, AmbientColor)
		RTTR_REGISTER_PROPERTY(PhongBlinn, Shininess)
		RTTR_REGISTER_PROPERTY(PhongBlinn, ShininessStrength);

	RTTR_REGISTER_WITH_NAME(MaterialAttributes, "MaterialAttributes").constructor<>()
		(
			rttr::policy::ctor::as_object
			)
		RTTR_REGISTER_PROPERTY(MaterialAttributes, shading)
		RTTR_REGISTER_PROPERTY(MaterialAttributes, PBRAttributes)
		RTTR_REGISTER_PROPERTY(MaterialAttributes, MaterialName)
		RTTR_REGISTER_PROPERTY(MaterialAttributes, phongBlinn)
		RTTR_REGISTER_PROPERTY(MaterialAttributes, AlbedoTex)
		RTTR_REGISTER_PROPERTY(MaterialAttributes, MaterialName)
		RTTR_REGISTER_PROPERTY(MaterialAttributes, Reflectivity)
		RTTR_REGISTER_PROPERTY(MaterialAttributes, Reflectance)
		RTTR_REGISTER_PROPERTY(MaterialAttributes, ReflectanceTexture);


		RTTR_REGISTER(MaterialData).constructor<>()
			(
				rttr::policy::ctor::as_object
				)
			RTTR_REGISTER_PROPERTY(MaterialData, materialAttributes)
			RTTR_REGISTER_PROPERTY(MaterialData, TextureToload)
			RTTR_REGISTER_PROPERTY(MaterialData, ModelName);

		RTTR_REGISTER(TextureToLoad).constructor<>()
			(
				rttr::policy::ctor::as_object
				)
			RTTR_REGISTER_PROPERTY(TextureToLoad, TexturePath)
			RTTR_REGISTER_PROPERTY(TextureToLoad, UMode)
			RTTR_REGISTER_PROPERTY(TextureToLoad, VMode);

	}


	bool MaterialData::Serialize(MaterialData& materials, std::string_view path, bool read)
	{
		JSONSerializer Serializer{};

		if (Serializer.OpenData(path, read) == JSONSerializer::ERROR_TYPE::FAILED)
			return false;

		if (read)
			Serializer.StartDeserializer(&materials);

		else
			Serializer.StartSerializer(&materials);

		Serializer.CloseData(read);

		return true;
	}
}