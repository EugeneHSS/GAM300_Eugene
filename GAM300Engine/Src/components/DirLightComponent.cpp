#include "components/DirLightComponent.h"
#include "components/CompReflectionMacro.h"
namespace TDS
{
	
	RTTR_REGISTRATION
	{
		RTTR_REGISTER_WITH_NAME(DirectionalLightComponent, "DirectionalLight")
		RTTR_REGISTER_PROPERTY(DirectionalLightComponent, color)
		RTTR_REGISTER_PROPERTY(DirectionalLightComponent, Direction);
	}


	DirectionalLightComponent::DirectionalLightComponent() :m_color(), m_Direction()
	{
	}

	DirectionalLightComponent::DirectionalLightComponent(DirectionalLightComponent&& toMove)noexcept  :m_color(toMove.m_color), m_Direction(toMove.m_Direction)
	{
	}
	DirectionalLightComponent* GetDirLightComponent(EntityID entityID)
	{
		return ecs.getComponent<DirectionalLightComponent>(entityID);
	}

	

}