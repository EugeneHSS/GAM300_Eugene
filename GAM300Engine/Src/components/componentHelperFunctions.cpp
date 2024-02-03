/*!*************************************************************************
****
\file componentHelperFunctions.cpp
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program defines the helper functions for the component class
****************************************************************************
***/

#include "components/components.h"
#include "Timestep/Timestep.h"
#include "SceneManager/SceneManager.h"
#include "../GAM300Engine/Include/Rendering/GraphicsManager.h"

namespace TDS
{
	float GetDeltaTime()
	{
		return TimeStep::GetDeltaTime();
	}

	std::string GetAssetFolder()
	{
		return SceneManager::GetInstance()->getAssetPath();
	}

	float RandomNumber(float min, float max)
	{
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 gen(rd()); // seed the generator
		std::uniform_int_distribution<> distr(min, max); // define the range

		return distr(gen);
	}

	float GetScreenWidth()
	{
		return GraphicsManager::getInstance().GetWindow()->getWidth();
	}

	float GetScreenHeight()
	{
		return GraphicsManager::getInstance().GetWindow()->getHeight();
	}

	/*!*************************************************************************
	This function is a helper function to add components by string name
	****************************************************************************/
	IComponent* addComponentByName(std::string componentName, const EntityID& entityID)
	{
		if (componentName == "AI")
			return ecs.addComponent<AI>(entityID);
		else if (componentName == "Box Collider")
			return ecs.addComponent<BoxCollider>(entityID);
		else if (componentName == "Camera Component")
			return ecs.addComponent<CameraComponent>(entityID);
		else if (componentName == "Capsule Collider")
			return ecs.addComponent<CapsuleCollider>(entityID);
		else if (componentName == "Graphics Component")
			return ecs.addComponent<GraphicsComponent>(entityID);
		else if (componentName == "Name Tag")
			return ecs.addComponent<NameTag>(entityID);
		else if (componentName == "Rigid Body")
			return ecs.addComponent<RigidBody>(entityID);
		else if (componentName == "Sphere Collider")
			return ecs.addComponent<SphereCollider>(entityID);
		else if (componentName == "Sprite")
			return ecs.addComponent<Sprite>(entityID);
		else if (componentName == "Tag")
			return ecs.addComponent<Tag>(entityID);
		else if (componentName == "Transform")
			return ecs.addComponent<Transform>(entityID);
		else if (componentName == "Win Data")
			return ecs.addComponent<WinData>(entityID);
		else if (componentName == "UI Sprite")
			return ecs.addComponent<UISprite>(entityID);
		else if (componentName == "Audio")
			return ecs.addComponent<SoundInfo>(entityID);
		else if (componentName == "DirectionalLight")
			return ecs.addComponent<DirectionalLightComponent>(entityID);
		else if (componentName == "SpotLight")
			return ecs.addComponent<SpotLightComponent>(entityID);
		else if (componentName == "PointLight")
			return ecs.addComponent<PointLightComponent>(entityID);
		
		else 
			return nullptr;
	}

	/*!*************************************************************************
	This function is a helper function to add components by string name
	(overloaded for rttr)
	****************************************************************************/
	rttr::instance addComponentByName(rttr::type& componentName, const EntityID& entityID)
	{
		if (componentName == rttr::type::get<AI>())
			return *(ecs.addComponent<AI>(entityID));
		else if (componentName == rttr::type::get<BoxCollider>())
			return *(ecs.addComponent<BoxCollider>(entityID));
		else if (componentName == rttr::type::get<CameraComponent>())
			return *(ecs.addComponent<CameraComponent>(entityID));
		else if (componentName == rttr::type::get<CapsuleCollider>())
			return *(ecs.addComponent<CapsuleCollider>(entityID));
		else if (componentName == rttr::type::get<GraphicsComponent>())
			return *(ecs.addComponent<GraphicsComponent>(entityID));
		else if (componentName == rttr::type::get<NameTag>())
			return *(ecs.addComponent<NameTag>(entityID));
		else if (componentName == rttr::type::get<RigidBody>())
			return *(ecs.addComponent<RigidBody>(entityID));
		else if (componentName == rttr::type::get<SphereCollider>())
			return *(ecs.addComponent<SphereCollider>(entityID));
		else if (componentName == rttr::type::get<Sprite>())
			return *(ecs.addComponent<Sprite>(entityID));
		else if (componentName == rttr::type::get<Tag>())
			return *(ecs.addComponent<Tag>(entityID));
		else if (componentName == rttr::type::get<Transform>())
			return *(ecs.addComponent<Transform>(entityID));
		else if (componentName == rttr::type::get<UISprite>())
			return *(ecs.addComponent<UISprite>(entityID));
		else if (componentName == rttr::type::get<WinData>())
			return *(ecs.addComponent<WinData>(entityID));
		else if (componentName == rttr::type::get<SoundInfo>())
			return *(ecs.addComponent<SoundInfo>(entityID));
		else if (componentName == rttr::type::get<DirectionalLightComponent>())
			return *(ecs.addComponent<DirectionalLightComponent>(entityID));
		else if (componentName == rttr::type::get<SpotLightComponent>())
			return *(ecs.addComponent<SpotLightComponent>(entityID));
		else if (componentName == rttr::type::get<DirectionalLightComponent>())
			return *(ecs.addComponent<DirectionalLightComponent>(entityID));
	}

	/*!*************************************************************************
	This function is a helper function to get components by string name
	****************************************************************************/
	IComponent* getComponentByName(std::string componentName, const EntityID& entityID)
	{
		if (componentName == "AI" || componentName == "ScriptAPI.AIComponent")
			return ecs.getComponent<AI>(entityID);
		else if (componentName == "Box Collider" || componentName == "ScriptAPI.BoxColliderComponent")
			return ecs.getComponent<BoxCollider>(entityID);
		else if (componentName == "Camera Component" || componentName == "ScriptAPI.CameraComponent")
			return ecs.getComponent<CameraComponent>(entityID);
		else if (componentName == "Capsule Collider" || componentName == "ScriptAPI.CapsuleColliderComponent")
			return ecs.getComponent<CapsuleCollider>(entityID);
		else if (componentName == "Graphics Component" || componentName == "ScriptAPI.GraphicComponent")
			return ecs.getComponent<GraphicsComponent>(entityID);
		else if (componentName == "Name Tag" || componentName == "ScriptAPI.NameTagComponent")
			return ecs.getComponent<NameTag>(entityID);
		else if (componentName == "Rigid Body" || componentName == "ScriptAPI.RigidBodyComponent")
			return ecs.getComponent<RigidBody>(entityID);
		else if (componentName == "Sphere Collider" || componentName == "ScriptAPI.SphereColliderComponent")
			return ecs.getComponent<SphereCollider>(entityID);
		else if (componentName == "Sprite" || componentName == "ScriptAPI.SpriteComponent")
			return ecs.getComponent<Sprite>(entityID);
		else if (componentName == "Tag" || componentName == "ScriptAPI.TagComponent")
			return ecs.getComponent<Tag>(entityID);
		else if (componentName == "Transform" || componentName == "ScriptAPI.TransformComponent")
			return ecs.getComponent<Transform>(entityID);
		else if (componentName == "Win Data" || componentName == "ScriptAPI.WinDataComponent")
			return ecs.getComponent<WinData>(entityID);
		else if (componentName == "UI Sprite" || componentName == "ScriptAPI.UISpriteComponent")
			return ecs.getComponent<UISprite>(entityID);
		else if (componentName == "Audio" || componentName == "ScriptAPI.AudioComponent")
			return ecs.getComponent<SoundInfo>(entityID);
		else if (componentName == "DirectionalLight")
			return ecs.getComponent<DirectionalLightComponent>(entityID);
		else if (componentName == "SpotLight")
			return ecs.getComponent<SpotLightComponent>(entityID);
		else if (componentName == "PointLight")
			return ecs.getComponent<PointLightComponent>(entityID);

		else
			return nullptr;
	}

	/*!*************************************************************************
	This function is a helper function to get components by string name
	(overloaded for rttr)
	****************************************************************************/
	rttr::instance getComponentByName(rttr::type& componentName, const EntityID& entityID)
	{
		if (componentName == rttr::type::get<AI>())
			return *(ecs.getComponent<AI>(entityID));
		else if (componentName == rttr::type::get<BoxCollider>())
			return *(ecs.getComponent<BoxCollider>(entityID));
		else if (componentName == rttr::type::get<CameraComponent>())
			return *(ecs.getComponent<CameraComponent>(entityID));
		else if (componentName == rttr::type::get<CapsuleCollider>())
			return *(ecs.getComponent<CapsuleCollider>(entityID));
		else if (componentName == rttr::type::get<GraphicsComponent>())
			return *(ecs.getComponent<GraphicsComponent>(entityID));
		else if (componentName == rttr::type::get<NameTag>())
			return *(ecs.getComponent<NameTag>(entityID));
		else if (componentName == rttr::type::get<RigidBody>())
			return *(ecs.getComponent<RigidBody>(entityID));
		else if (componentName == rttr::type::get<SphereCollider>())
			return *(ecs.getComponent<SphereCollider>(entityID));
		else if (componentName == rttr::type::get<Sprite>())
			return *(ecs.getComponent<Sprite>(entityID));
		else if (componentName == rttr::type::get<Tag>())
			return *(ecs.getComponent<Tag>(entityID));
		else if (componentName == rttr::type::get<Transform>())
			return *(ecs.getComponent<Transform>(entityID));
		else if (componentName == rttr::type::get<UISprite>())
			return *(ecs.getComponent<UISprite>(entityID));
		else if (componentName == rttr::type::get<WinData>())
			return *(ecs.getComponent<WinData>(entityID));
		else if (componentName == rttr::type::get<SoundInfo>())
			return *(ecs.getComponent<SoundInfo>(entityID));
		else if (componentName == rttr::type::get<DirectionalLightComponent>())
			return *(ecs.getComponent<DirectionalLightComponent>(entityID));
		else if (componentName == rttr::type::get<SpotLightComponent>())
			return *(ecs.getComponent<SpotLightComponent>(entityID));
		else if (componentName == rttr::type::get<PointLightComponent>())
			return  *(ecs.getComponent<PointLightComponent>(entityID));
	}

	/*!*************************************************************************
	This function is a helper function to remove components by string name
	****************************************************************************/
	void removeComponentByName(std::string componentName, const EntityID& entityID)
	{
		if (componentName == "AI")
			ecs.removeComponent<AI>(entityID);
		else if (componentName == "Box Collider")
			ecs.removeComponent<BoxCollider>(entityID);
		else if (componentName == "Camera Component")
			ecs.removeComponent<CameraComponent>(entityID);
		else if (componentName == "Capsule Collider")
			ecs.removeComponent<CapsuleCollider>(entityID);
		else if (componentName == "Graphics Component")
			ecs.removeComponent<GraphicsComponent>(entityID);
		else if (componentName == "Name Tag")
			ecs.removeComponent<NameTag>(entityID);
		else if (componentName == "Rigid Body")
			ecs.removeComponent<RigidBody>(entityID);
		else if (componentName == "Sphere Collider")
			ecs.removeComponent<SphereCollider>(entityID);
		else if (componentName == "Sprite")
			ecs.removeComponent<Sprite>(entityID);
		else if (componentName == "Tag")
			ecs.removeComponent<Tag>(entityID);
		else if (componentName == "Transform")
			ecs.removeComponent<Transform>(entityID);
		else if (componentName == "Win Data")
			ecs.removeComponent<WinData>(entityID);
		else if (componentName == "UI Sprite")
			ecs.removeComponent<UISprite>(entityID);
		else if (componentName == "Audio")
			ecs.removeComponent<SoundInfo>(entityID);
		else if (componentName == "DirectionalLight")
			ecs.removeComponent<DirectionalLightComponent>(entityID);
		else if (componentName == "SpotLight")
			ecs.removeComponent<SpotLightComponent>(entityID);
		else if (componentName == "PointLight")
			ecs.removeComponent<PointLightComponent>(entityID);
	}

	/*!*************************************************************************
	This function is a helper function to toggle components
	****************************************************************************/
	void setComponentIsEnable(std::string componentName, const EntityID& entityID, bool isEnabled)
	{
		if (componentName == "AI")
			ecs.setComponentIsEnabled<AI>(entityID, isEnabled);
		else if (componentName == "Box Collider")
			ecs.setComponentIsEnabled<BoxCollider>(entityID, isEnabled);
		else if (componentName == "Camera Component")
			ecs.setComponentIsEnabled<CameraComponent>(entityID, isEnabled);
		else if (componentName == "Capsule Collider")
			ecs.setComponentIsEnabled<CapsuleCollider>(entityID, isEnabled);
		else if (componentName == "Graphics Component")
			ecs.setComponentIsEnabled<GraphicsComponent>(entityID, isEnabled);
		else if (componentName == "Name Tag")
			ecs.setComponentIsEnabled<NameTag>(entityID, isEnabled);
		else if (componentName == "Rigid Body")
			ecs.setComponentIsEnabled<RigidBody>(entityID, isEnabled);
		else if (componentName == "Sphere Collider")
			ecs.setComponentIsEnabled<SphereCollider>(entityID, isEnabled);
		else if (componentName == "Sprite")
			ecs.setComponentIsEnabled<Sprite>(entityID, isEnabled);
		else if (componentName == "Tag")
			ecs.setComponentIsEnabled<Tag>(entityID, isEnabled);
		else if (componentName == "Transform")
			ecs.setComponentIsEnabled<Transform>(entityID, isEnabled);
		else if (componentName == "Win Data")
			ecs.setComponentIsEnabled<WinData>(entityID, isEnabled);
		else if (componentName == "UI Sprite")
			ecs.setComponentIsEnabled<UISprite>(entityID, isEnabled);
		else if (componentName == "Audio")
			ecs.setComponentIsEnabled<SoundInfo>(entityID, isEnabled);
		else if (componentName == "DirectionalLight")
			ecs.setComponentIsEnabled<DirectionalLightComponent>(entityID, isEnabled);
		else if (componentName == "SpotLight")
			ecs.setComponentIsEnabled<SpotLightComponent>(entityID, isEnabled);
		else if (componentName == "PointLight")
			ecs.setComponentIsEnabled<PointLightComponent>(entityID, isEnabled);
	}

	/*!*************************************************************************
	This function is a helper function to return if component is enabled
	****************************************************************************/
	bool getComponentIsEnable(std::string componentName, const EntityID& entityID)
	{
		if (componentName == "AI")
			return ecs.getComponentIsEnabled<AI>(entityID);
		else if (componentName == "Box Collider")
			return ecs.getComponentIsEnabled<BoxCollider>(entityID);
		else if (componentName == "Camera Component")
			return ecs.getComponentIsEnabled<CameraComponent>(entityID);
		else if (componentName == "Capsule Collider")
			return ecs.getComponentIsEnabled<CapsuleCollider>(entityID);
		else if (componentName == "Graphics Component")
			return ecs.getComponentIsEnabled<GraphicsComponent>(entityID);
		else if (componentName == "Name Tag")
			return ecs.getComponentIsEnabled<NameTag>(entityID);
		else if (componentName == "Rigid Body")
			return ecs.getComponentIsEnabled<RigidBody>(entityID);
		else if (componentName == "Sphere Collider")
			return ecs.getComponentIsEnabled<SphereCollider>(entityID);
		else if (componentName == "Sprite")
			return ecs.getComponentIsEnabled<Sprite>(entityID);
		else if (componentName == "Tag")
			return ecs.getComponentIsEnabled<Tag>(entityID);
		else if (componentName == "Transform")
			return ecs.getComponentIsEnabled<Transform>(entityID);
		else if (componentName == "Win Data")
			return ecs.getComponentIsEnabled<WinData>(entityID);
		else if (componentName == "UI Sprite")
			return ecs.getComponentIsEnabled<UISprite>(entityID);
		else if (componentName == "Audio")
			return ecs.getComponentIsEnabled<SoundInfo>(entityID);
		else if (componentName == "DirectionalLight")
			return  ecs.getComponentIsEnabled<DirectionalLightComponent>(entityID);
		else if (componentName == "SpotLight")
			return  ecs.getComponentIsEnabled<SpotLightComponent>(entityID);
		else if (componentName == "PointLight")
			return  ecs.getComponentIsEnabled<PointLightComponent>(entityID);
		else
			return false;
	}
}
