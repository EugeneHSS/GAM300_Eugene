#pragma once
#include "ecs/ecs.h"

namespace TDS
{
	class PointLightComponent : public IComponent
	{
	public:
		DLL_API PointLightComponent();
		DLL_API PointLightComponent(PointLightComponent&& toMove) noexcept;
		DLL_API ~PointLightComponent() = default;
		
		//FOR SCRIPTAPI
		DLL_API void SetPointLightColor(float x, float y, float z, float w);
		DLL_API void GetPointLightColorValues(float& x, float& y, float& z, float& w);

		DLL_API void SetPointLightAttenuation(float x, float y, float z, float w);
		DLL_API void GetPointLightAttenuation(float& x, float& y, float& z, float& w);

		DLL_API void SetPointLightColorAlpha(float alpha);
		DLL_API float GetPointLightColorAlpha();

		RTTR_ENABLE(IComponent)
		RTTR_REGISTRATION_FRIEND



	public:
		Vec4 m_Position;
		Vec4 m_color;
		Vec4 m_Attenuation;




	};
	DLL_API PointLightComponent* GetPointLightComponent(EntityID entityID);
}