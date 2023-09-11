#ifndef COMPONENTS
#define COMPONENTS

#include "cameraComponent.h"
#include "collider.h"
#include "nameTag.h"
#include "playerAttributes.h"
#include "rigidBody.h"
#include "sprite.h"
#include "tag.h"
#include "transform.h"
#include "winData.h"

// Helper functions for components
namespace TDS
{
	IComponent* addComponentByName(std::string componentName, EntityID entityID);
}

#endif