/*!*************************************************************************
****
\file component.h
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program declares the helper functions for component class
****************************************************************************
***/

#ifndef COMPONENTS
#define COMPONENTS

#include "ai.h"
#include "boxCollider.h"
#include "cameraComponent.h"
#include "capsuleCollider.h"
#include "graphicsComponent.h"
#include "nameTag.h"
#include "rigidBody.h"
#include "sphereCollider.h"
#include "sprite.h"
#include "tag.h"
#include "transform.h"
#include "winData.h"
#include "UiSprite.h"
#include "PointLightComponent.h"
#include "DirLightComponent.h"
#include "SpotLightComponent.h"
#include "fmod_engine/AudioEngine.h"
#include "SoundInfo.h"

// Helper functions for components
namespace TDS
{
	// May want to put in another file next time
	DLL_API float GetDeltaTime();
	DLL_API std::string GetAssetFolder();
	DLL_API float RandomNumber(float min, float max);
	DLL_API float GetScreenWidth();
	DLL_API float GetScreenHeight();

	DLL_API struct ScriptValues
	{
		std::string name;
		std::string type;

		std::string value;

		// For game objects and components
		EntityID referenceEntityID = 0;

		// For vector info
		float vectorValueX;
		float vectorValueY;
		float vectorValueZ;
		float vectorValueW;

		// Header
		std::string headerString;
	};

	/*!*************************************************************************
	This function is a helper function to add components by string name
	****************************************************************************/
	DLL_API IComponent* addComponentByName(std::string componentName, const EntityID& entityID);
	/*!*************************************************************************
	This function is a helper function to add components by string name
	(overloaded for rttr)
	****************************************************************************/
	rttr::instance addComponentByName(rttr::type& componentName, const EntityID& entityID);
	/*!*************************************************************************
	This function is a helper function to get components by string name
	****************************************************************************/
	DLL_API IComponent* getComponentByName(std::string componentName, const EntityID& entityID);
	/*!*************************************************************************
	This function is a helper function to get components by string name
	(overloaded for rttr)
	****************************************************************************/
	rttr::instance getComponentByName(rttr::type& componentName, const EntityID& entityID);
	/*!*************************************************************************
	This function is a helper function to remove components by string name
	****************************************************************************/
	DLL_API void removeComponentByName(std::string componentName, const EntityID& entityID);
	/*!*************************************************************************
	This function is a helper function to toggle components
	****************************************************************************/
	DLL_API void setComponentIsEnable(std::string componentName, const EntityID& entityID, bool isEnabled);
	/*!*************************************************************************
	This function is a helper function to return if component is enabled
	****************************************************************************/
	DLL_API bool getComponentIsEnable(std::string componentName, const EntityID& entityID);
}

#endif