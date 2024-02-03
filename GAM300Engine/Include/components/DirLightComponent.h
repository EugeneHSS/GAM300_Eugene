#pragma once
#include "ecs/ecs.h"

namespace TDS
{
	class DirectionalLightComponent : public IComponent
	{
	public:
		DLL_API DirectionalLightComponent();
		DLL_API DirectionalLightComponent(DirectionalLightComponent&& toMove) noexcept;
		DLL_API ~DirectionalLightComponent() = default;

		RTTR_ENABLE(IComponent);
		RTTR_REGISTRATION_FRIEND

		
	public:
		Vec4							m_Direction;
		Vec4							m_color;



	};
	DLL_API DirectionalLightComponent* GetDirLightComponent(EntityID entityID);
}