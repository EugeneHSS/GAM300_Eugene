#include "components/SpotLightComponent.h"
#include "components/CompReflectionMacro.h"


namespace TDS
{

	RTTR_REGISTRATION{
		RTTR_REGISTER_WITH_NAME(SpotLightComponent, "SpotLight")
		RTTR_REGISTER_PROPERTY(SpotLightComponent, Position)
		RTTR_REGISTER_PROPERTY(SpotLightComponent, direction)
		RTTR_REGISTER_PROPERTY(SpotLightComponent, color)
		RTTR_REGISTER_PROPERTY(SpotLightComponent, attenuation)
		RTTR_REGISTER_PROPERTY(SpotLightComponent, Angle);
	}

	SpotLightComponent* GetSpotLightComponent(EntityID entityID)
	{
		return ecs.getComponent<SpotLightComponent>(entityID);
	}
	SpotLightComponent::SpotLightComponent():m_Position(),m_direction(),m_color(),m_attenuation(),m_Angle()
	{
	}
	SpotLightComponent::SpotLightComponent(SpotLightComponent&& toMove) noexcept :m_Position(toMove.m_Position), m_direction(toMove.m_direction), m_color(toMove.m_color),
		m_attenuation(toMove.m_attenuation)
	{
	}
}