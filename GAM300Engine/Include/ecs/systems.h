/*!*************************************************************************
****
\file systems.h
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program declares and registers the systems in the ECS
****************************************************************************
***/

#ifndef SYSTEMS
#define SYSTEMS

#include "ecs/ecs.h"
#include "components/components.h"

// DONT INCLUDE THIS ANYWHERE BUT ECS.CPP

namespace TDS
{
	DLL_API ECS ecs;

	// Layer 1 - Run only on play
	// Layer 3 - Event Handler (always running)
	// Layer 3 - Graphics (always running)

	ECSSystem<Transform, RigidBody>					_PhysicsSystem(1);
	ECSSystem<AI>									_AISystem(1);
	ECSSystem<Transform, CameraComponent>			_CameraSystem(1);
	ECSSystem<SoundInfo>							_AudioSystem(1);

	ECSSystem<Transform>							_EventHandler_ChildTransform(2);
	ECSSystem<NameTag>								_EventHandler_Enable(2);
	ECSSystem<Transform, GraphicsComponent>			_ColliderDebugSystem(2);

	ECSSystem<Transform, GraphicsComponent>			_RenderSystem(3);
	ECSSystem<Transform, UISprite>					_UiSystem(3);



}

#endif // SYSTEMS