#include "components/PointLightComponent.h"
#include "components/CompReflectionMacro.h"

RTTR_REGISTRATION
{
	using namespace TDS;
		RTTR_REGISTER_WITH_NAME(PointLightComponent, "PointLight")
		RTTR_REGISTER_PROPERTY(PointLightComponent, Position)
		RTTR_REGISTER_PROPERTY(PointLightComponent, color)
		RTTR_REGISTER_PROPERTY(PointLightComponent, Attenuation);
}

namespace TDS
{

	PointLightComponent* GetPointLightComponent(EntityID entityID)
	{
		return ecs.getComponent<PointLightComponent>(entityID);
	}

	PointLightComponent::PointLightComponent():m_Position(), m_color(), m_Attenuation()
	{
	}
	PointLightComponent::PointLightComponent(PointLightComponent&& toMove) noexcept :m_Position(toMove.m_Position), m_color(toMove.m_color), m_Attenuation(toMove.m_Attenuation)
	{
	}
}