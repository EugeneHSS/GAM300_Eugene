/*!*****************************************************************************
 * \file          PhysicsSystem.h
 * \author        Cheang Wen Ding
 * \par DP email: cheang.w@digipen.edu
 * \par Course:   CSD3400
 * \par Section:  A
 * \date          01/10/2023
 * \brief         The system that handles physics.
 *******************************************************************************/

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H
// Engine includes
#include "ecs/ecs.h"
#include "dotnet/ImportExport.h"
#include "Timestep/Timestep.h"
#include "Logger/Logger.h"
#include "eventmanager/eventHandler.h"
// Component includes
#include "components/rigidBody.h"
#include "components/boxCollider.h"
#include "components/sphereCollider.h"
#include "components/capsuleCollider.h"
#include "components/transform.h"
#include "components/GraphicsComponent.h"
// Physics includes
#include "Physics/JPHLayers.h"
#include "Physics/ContactListenerImpl.h"
#include "Physics/CollisionCollectorImpl.h"
#include "JoltPhysics/Utils/JoltConversionUtils.h"

 // Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhase.h>


JPH_SUPPRESS_WARNINGS

// X = moving left, right
// Y = moving up, down
// Z = moving forward, backward

namespace TDS
{
	class DLL_API PhysicsSystem
	{
	public:
		/*!*************************************************************************
		 * Physics System Init and Update (Will be used by the ECS)
		 ***************************************************************************/
		static void PhysicsSystemInit();
		static void PhysicsSystemUpdate(const float dt, const std::vector<EntityID>& entities, Transform* _transform, RigidBody* _rigibody);
		
		// potentially need move somewhere
		static void SetIsPlaying(bool input) { m_oneTimeInit = input; }
		static bool GetIsPlaying() { return m_oneTimeInit; }
		static std::unique_ptr<JPH::PhysicsSystem>			m_pSystem; // unsafe to be public but for now it is

		static void JPH_SystemShutdown();
		static std::optional<EntityID> findEntityByID(uint32_t key);

		static void (*OnTriggerEnter)(EntityID trigger, EntityID collider);
		static void (*OnTriggerStay)(EntityID trigger, EntityID collider);
		static void (*OnTriggerExit)(EntityID trigger, EntityID collider);

	private:

		static void JPH_SystemUpdate(Transform* _transform, RigidBody* _rigidbody);
		static void JPH_CreateBodyID(const EntityID& entities, Transform* _transform, RigidBody* _rigidbody);
		static void JPH_Raycast(const EntityID& entities, Transform* _transform, RigidBody* _rigidbody, bool isPlayer);
	private:
		// TDS Physics System
		static const double fixedDt;
		static double accumulatedTime;
		inline static bool m_oneTimeInit					= false;

	
		// Container that holds all the bodyID
		static std::vector<JoltBodyID>					    m_pBodyIDVector;
		static std::unordered_map<uint32_t, EntityID>		m_pBodyIDMap;

	private:
		// Jolt Physics Global Settings
		static std::unique_ptr<JPH::TempAllocatorImpl>		m_pTempAllocator;
		static std::unique_ptr<JPH::JobSystemThreadPool>	m_pJobSystem;
		static MyContactListener*							contact_listener;
		static std::unique_ptr<JPH::AllHitCollisionCollector<RayCastBodyCollector>> collector;
		static std::unique_ptr<JPH::BodyManager>            m_pBodyManager;

	};

}



#endif // !PHYSICSSYSTEM_H
