#pragma once
#include "ecs/ecs.h"

namespace TDS
{
	class SpotLightComponent : public IComponent
	{
	public:
		DLL_API SpotLightComponent();
		DLL_API SpotLightComponent(SpotLightComponent&& toMove) noexcept;
		DLL_API ~SpotLightComponent() = default;

		RTTR_ENABLE(IComponent);
		RTTR_REGISTRATION_FRIEND
	public:
		Vec4			m_Position;
		Vec4			m_direction;
		Vec4			m_color;
		Vec4			m_attenuation;
		float			m_Angle;

	};

	DLL_API SpotLightComponent* GetSpotLightComponent(EntityID entityID);
}