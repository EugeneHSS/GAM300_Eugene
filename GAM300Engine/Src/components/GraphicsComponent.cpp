#include "components/graphicsComponent.h"
#include "components/CompReflectionMacro.h"

RTTR_REGISTRATION
{
	using namespace TDS;




	rttr::registration::class_<GraphicsComponent>("Graphics Component")
		.constructor<>()
		.method("IsPointLight", &GraphicsComponent::IsPointLight)
		.method("TogglePointLight", &GraphicsComponent::TogglePointLight)
		.property("PointLight", &GraphicsComponent::m_Pointlight)
		.property("TextureName", &GraphicsComponent::m_TextureName)
		.method("IsDebugOn", &GraphicsComponent::IsDebugOn)
		.method("ToggleDebug", &GraphicsComponent::ToggleDebug)
		.method("SetModelName", &GraphicsComponent::SetModelName)
		.method("SetView2D", &GraphicsComponent::SetView2D)
		.property("Show Mesh", &GraphicsComponent::m_ShowMesh)
		.property("Debug", &GraphicsComponent::m_Debug)
		//.method("SetTextureName", &GraphicsComponent::SetTextureName)
		//.property("TextureName", &GraphicsComponent::m_TextureName, rttr::select_overload<void(std::string)>(&GraphicsComponent::SetTextureName))
		.property("ModelName", &GraphicsComponent::m_ModelName)
		.property("MeshName", &GraphicsComponent::m_MeshName)
		.property("MeshNodeName", &GraphicsComponent::m_MeshNodeName)
		.method("SetColor", rttr::select_overload<void(Vec4)>(&GraphicsComponent::SetColor))
		.method("SetColor", rttr::select_overload<void(float, float, float, float)>(&GraphicsComponent::SetColor))
		.method("GetColor", &GraphicsComponent::GetColor)
		.property("Color", &GraphicsComponent::m_Color)
		.property("Used In 2D", &GraphicsComponent::m_UsedIn2D)
		.property("Model UID", &GraphicsComponent::m_modelUID)
		.property("m_UsePreloadAlbedo", &GraphicsComponent::m_UsePreloadMaterials)
		.property("use materials", &GraphicsComponent::m_UseMaterials)
		.property("MaterialAttributes", &GraphicsComponent::m_MaterialAttributes);


}

namespace TDS
{
	GraphicsComponent::GraphicsComponent() : m_AssetReference(),
		m_TextureReference(), m_MeshControllerRef(), m_ModelName(), m_MeshName(), m_MeshNodeName(), m_TextureName(), m_modelUID(),
		m_UsedIn2D(false), m_IsAnimated(false), m_UseMaterials(false), m_UsePreloadMaterials(false),/* m_MaterialAttributes(),*/
		m_ShowMesh(true),
		m_Color(1.f, 1.f, 1.f, 1.f),
		m_PointLightID(-1.f),
		m_Pointlight(false),
		m_Debug(false)

	{ }

	GraphicsComponent::GraphicsComponent(GraphicsComponent&& toMove) noexcept : m_AssetReference(std::move(toMove.m_AssetReference)),
		m_TextureReference(std::move(toMove.m_TextureReference)),
		m_ShowMesh(std::move(toMove.m_ShowMesh)),
		m_UsedIn2D(std::move(toMove.m_UsedIn2D)),
		m_TextureName(std::move(toMove.m_TextureName)),
		m_ModelName(std::move(toMove.m_ModelName)),
		m_MeshName(toMove.m_MeshName),
		m_MeshNodeName(toMove.m_MeshNodeName),
		m_Color(toMove.m_Color),
		m_PointLightID(toMove.m_PointLightID),
		m_Pointlight(toMove.m_Pointlight),
		m_Debug(toMove.m_Debug),
		m_IsAnimated(toMove.m_IsAnimated), 
		m_UseMaterials(toMove.m_UseMaterials),
		m_UsePreloadMaterials(toMove.m_UsePreloadMaterials)


	{ }

	GraphicsComponent* GetGraphicsComponent(EntityID entityID)
	{
		return ecs.getComponent<GraphicsComponent>(entityID);
	}
}
