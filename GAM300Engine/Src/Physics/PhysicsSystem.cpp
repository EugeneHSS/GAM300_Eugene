/*!*****************************************************************************
 * \file          PhysicsSystem.cpp
 * \author        Cheang Wen Ding
 * \par DP email: cheang.w@digipen.edu
 * \par Course:   CSD3400
 * \par Section:  A
 * \date          01/10/2023
 * \brief         Implementation of the physics system.
 *******************************************************************************/
#include "Physics/PhysicsSystem.h"
#include "Rendering/GraphicsManager.h"

namespace TDS
{
	/*!*************************************************************************
	 * Static variables
	 ***************************************************************************/
	const double								PhysicsSystem::fixedDt = 0.0166666f;
	double										PhysicsSystem::accumulatedTime = 0.0f;
	std::unique_ptr<JPH::PhysicsSystem>			PhysicsSystem::m_pSystem;
	std::unique_ptr<JPH::TempAllocatorImpl>		PhysicsSystem::m_pTempAllocator;
	std::unique_ptr<JPH::JobSystemThreadPool>	PhysicsSystem::m_pJobSystem;
	std::unique_ptr<JPH::BodyManager>           PhysicsSystem::m_pBodyManager;
	std::vector<JoltBodyID>					    PhysicsSystem::m_pBodyIDVector;
	std::unordered_map<uint32_t, EntityID>		PhysicsSystem::m_pBodyIDMap;
	MyContactListener*							PhysicsSystem::contact_listener;
	std::unique_ptr<JPH::AllHitCollisionCollector<RayCastBodyCollector>>		PhysicsSystem::collector;

	BPLayerInterfaceImpl						broad_phase_layer_interface;
	ObjectVsBroadPhaseLayerFilterImpl			object_vs_broadphase_layer_filter;
	ObjectLayerPairFilterImpl					object_vs_object_layer_filter;

	void (*PhysicsSystem::OnTriggerEnter)(EntityID trigger, EntityID collider);
	void (*PhysicsSystem::OnTriggerStay)(EntityID trigger, EntityID collider);
	void (*PhysicsSystem::OnTriggerExit)(EntityID trigger, EntityID collider);

	/*!*************************************************************************
	 * Configuration
	 ***************************************************************************/
	const unsigned int cMaxBodies = 1024;				// Maximum number of bodies in the physics system
	const unsigned int cNumBodyMutexes = 0;				// Autodetect
	const unsigned int cMaxBodyPairs = 1024;			// Max amount of body pair for broadphrase detection
	const unsigned int cMaxContactConstraints = 1024;	// Maximum number of contact constraints
	
	static void TraceImpl(const char* inFMT, ...)
	{
		// Format the message
		va_list list;
		va_start(list, inFMT);
		char buffer[1024];
		vsnprintf(buffer, sizeof(buffer), inFMT, list);
		va_end(list);

		// Print to the TTY
		TDS_TRACE(buffer);
	}

#ifdef JPH_ENABLE_ASSERTS
	// Callback for asserts, connect this to your own assert handler if you have one
	static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, JPH::uint inLine)
	{
		// Print to the TTY
		std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << std::endl;
		// Breakpoint
		return true;
	};
#endif

	void PhysicsSystem::PhysicsSystemInit()
	{
		// Initialize the Jolt Core
		using namespace JPH;
		// Register allocation hook
		RegisterDefaultAllocator();

		// Install callbacks
		Trace = TraceImpl;
		JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

		// Create a factory
		Factory::sInstance = new Factory();

		// Register all Jolt physics types
		RegisterTypes();

		m_pJobSystem = std::make_unique<JobSystemThreadPool>(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);
		// We need a temp allocator for temporary allocations during the physics update. We're
		// pre-allocating 10 MB to avoid having to do allocations during the physics update.
		// B.t.w. 10 MB is way too much for this example but it is a typical value you can use.
		// If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
		// malloc / free.
		m_pTempAllocator = std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024);

		//m_pSystem = std::make_unique<JPH::PhysicsSystem>();
		m_pSystem = std::make_unique<JPH::PhysicsSystem>();
		m_pSystem->Init(cMaxBodies,
			cNumBodyMutexes,
			cMaxBodyPairs,
			cMaxContactConstraints,
			broad_phase_layer_interface,
			object_vs_broadphase_layer_filter,
			object_vs_object_layer_filter);

		// initialise collector
		collector = std::make_unique<JPH::AllHitCollisionCollector<RayCastBodyCollector>>();
		// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
		// Note that this is called from a job so whatever you do here needs to be thread safe.
		contact_listener = new MyContactListener();
		// Registering one is entirely optional.
		m_pSystem->SetContactListener(contact_listener);
		std::cout << "successfully init Jolt Physics" << '\n';
	}

	void PhysicsSystem::PhysicsSystemUpdate(const float dt, const std::vector<EntityID>& entities, Transform* _transform, RigidBody* _rigidbody)
	{
		// Physics loop
		JPH::BodyInterface* pBodies = &m_pSystem->GetBodyInterface();
		//for (int i = 0; i < entities.size(); ++i)
		//{
		//}
		if (!GetIsPlaying())
		{
			if (m_pSystem->GetNumBodies() != 0)
			{
				pBodies->RemoveBodies(JoltToTDS::ToBodyID(m_pBodyIDVector.data()), m_pBodyIDVector.size());
				pBodies->DestroyBodies(JoltToTDS::ToBodyID(m_pBodyIDVector.data()), m_pBodyIDVector.size());
				m_pBodyIDVector.clear();
				m_pBodyIDMap.clear();
			}
			SetIsPlaying(true);
		}
		for (int i = 0; i < entities.size(); ++i)
		{
			if (_rigidbody[i].GetBodyID().IsInvalid() == false) continue;

			JPH_CreateBodyID(entities[i], &_transform[i], &_rigidbody[i]);
			m_pSystem->OptimizeBroadPhase();				
		}
			
		//TDS_INFO(m_pSystem->GetNumBodies());
		accumulatedTime += dt;		
		while (accumulatedTime >= TimeStep::GetFixedDeltaTime())
		{
			// JPH physics simulation
			for (int i = 0; i < entities.size(); ++i)
			{
				using namespace JoltToTDS;
				EActivation mode = EActivation::Activate;
				//pBodies->SetPosition(ToBodyID(_rigidbody[i]), ToVec3(_transform[i].GetPosition()), mode); // only uncomment for debugging in editor
				if (GetBoxCollider(entities[i])) // Temporary will be remove with animation system
				{
					BoxCollider* vBox = GetBoxCollider(entities[i]);
					auto* pBody = m_pSystem->GetBodyLockInterfaceNoLock().TryGetBody(ToBodyID(_rigidbody[i]));
					if (vBox->GetIsTrigger())
					{
						pBody->SetIsSensor(true);
					}
					else
					{
						pBody->SetIsSensor(false);
					}
				}

			}
			
			m_pSystem->Update(TimeStep::GetFixedDeltaTime(), 1, m_pTempAllocator.get(), m_pJobSystem.get());
			// Update back to the ECS
			for (int j = 0; j < entities.size(); ++j)
			{
				if (!ecs.getEntityIsEnabled(entities[j]) || !ecs.getComponentIsEnabled<RigidBody>(entities[j]))
				{
					continue;
				}
				if (_rigidbody[j].getIsRayCast())
				{
					JPH_Raycast(entities[j], &_transform[j], &_rigidbody[j], &_rigidbody[j].getIsPlayer());
				}
				/*if (GetBoxCollider(entities[j]))
				{
					BoxCollider* vBox = GetBoxCollider(entities[j]);
					if (vBox->GetIsTrigger())
					{
						std::cout << _rigidbody[j].GetBodyID().GetIndexAndSequenceNumber() << '\n';
					}
				}*/
				
				Vec3 pos = _transform[j].GetPosition();
				Vec3 scale = _transform[j].GetScale();
				Vec3 rot = _transform[j].GetRotation();
				JPH_SystemUpdate(&_transform[j], &_rigidbody[j]);
				EventHandler::postChildTransformationEvent(entities[j], pos, scale, rot);

			}
			
			accumulatedTime -= TimeStep::GetFixedDeltaTime();
		}	
	}

	void PhysicsSystem::JPH_SystemUpdate(Transform* _transform, RigidBody* _rigidbody)
	{
		JPH::BodyID JPHBodyID = JoltToTDS::ToBodyID(*_rigidbody);
		JPH::BodyInterface* pBodies = &m_pSystem->GetBodyInterface();
		if (pBodies->GetMotionType(JPHBodyID) == JPH::EMotionType::Dynamic)
		{
			_transform->SetPosition(JoltToTDS::ToVec3(pBodies->GetPosition(JPHBodyID)));
			_rigidbody->SetLinearVel(JoltToTDS::ToVec3(pBodies->GetLinearVelocity(JPHBodyID)));
		}
		if (_rigidbody->getIsRayHit() &&  
			(collector->mHits.data()->mBodyID.GetIndexAndSequenceNumber() != _rigidbody->GetBodyID().GetIndexAndSequenceNumber() ||
			collector->HadHit() == false))
		{
			_rigidbody->setIsRayHit(false);
		}

	}
	void PhysicsSystem::JPH_SystemShutdown()
	{
		// cleanup
		m_pTempAllocator = nullptr;
		m_pSystem = nullptr;
		m_pJobSystem = nullptr;
		collector = nullptr;
		delete contact_listener;
		contact_listener = nullptr;
	}
	void PhysicsSystem::JPH_CreateBodyID(const EntityID& _entityID, Transform* _transform, RigidBody* _rigidbody)
	{
		EMotionType vMotionType = JoltToTDS::ToEMotionType(_rigidbody->GetMotionType());
		if (GetSphereCollider(_entityID))
		{
			SphereCollider* vSphere = GetSphereCollider(_entityID);
			JPH::SphereShapeSettings s_sphereSettings(vSphere->GetColliderRadius());
			JPH::ShapeSettings::ShapeResult result = s_sphereSettings.Create();
			JPH::ShapeRefC sphereShape = result.Get(); // if error, high chance is how the shape is created, radius cannot be 0!
			JPH::BodyCreationSettings b_sphereSetting
			(
				sphereShape,
				JoltToTDS::ToVec3(_transform->GetPosition()),
				JoltToTDS::ToQuat((_transform->GetRotation())),
				vMotionType,
				JoltLayers::GetObjectLayer(_rigidbody->GetMotionTypeInt(), vSphere->GetIsTrigger())
			);
			b_sphereSetting.mFriction = _rigidbody->GetFriction();
			b_sphereSetting.mRestitution = _rigidbody->GetRestitution();
			b_sphereSetting.mGravityFactor = (_rigidbody->GetUseGravity()) ? _rigidbody->GetGravityFactor() : 0.0f;
			b_sphereSetting.mLinearDamping = _rigidbody->GetLinearDamping();
			b_sphereSetting.mAngularDamping = _rigidbody->GetAngularDamping();
			b_sphereSetting.mIsSensor = vSphere->GetIsTrigger();
			b_sphereSetting.mAllowSleeping = false;

			JPH::BodyID sphereID = m_pSystem->GetBodyInterface().CreateAndAddBody(b_sphereSetting, JPH::EActivation::Activate);
			JoltBodyID vJoltBodyID(sphereID.GetIndexAndSequenceNumber());
			m_pBodyIDVector.push_back(vJoltBodyID);
			uint32_t vIndex = sphereID.GetIndex();
			m_pBodyIDMap[vIndex] = _entityID;
			TDS_INFO("Sphere Created!");
			_rigidbody->SetBodyID(vJoltBodyID);
		}
		else if (GetBoxCollider(_entityID))
		{
			BoxCollider* vBox = GetBoxCollider(_entityID);
			Vec3 halfExtents = vBox->GetColliderSize();
			JPH::Vec3 JPHextents = JoltToTDS::ToVec3(halfExtents);
	
			JPH::BoxShapeSettings s_boxSettings(JPHextents);
			JPH::ShapeSettings::ShapeResult result = s_boxSettings.Create();
			JPH::ShapeRefC boxShape = result.Get();

			JPH::BodyCreationSettings b_BoxSetting
				(
					boxShape,
					JoltToTDS::ToVec3(vBox->GetColliderCenter()),
					JoltToTDS::ToQuat((_transform->GetRotation())),
					vMotionType,
					JoltLayers::GetObjectLayer(_rigidbody->GetMotionTypeInt(), vBox->GetIsTrigger())
				);
			b_BoxSetting.mFriction = _rigidbody->GetFriction();
			b_BoxSetting.mRestitution = _rigidbody->GetRestitution();
			b_BoxSetting.mGravityFactor = (_rigidbody->GetUseGravity()) ? _rigidbody->GetGravityFactor() : 0.0f;
			b_BoxSetting.mLinearDamping = _rigidbody->GetLinearDamping();
			b_BoxSetting.mAngularDamping = _rigidbody->GetAngularDamping();
			b_BoxSetting.mIsSensor = vBox->GetIsTrigger();
			b_BoxSetting.mAllowSleeping = false;
			
			JPH::BodyID boxID = m_pSystem->GetBodyInterface().CreateAndAddBody(b_BoxSetting, JPH::EActivation::Activate);

			JoltBodyID vJoltBodyID(boxID.GetIndexAndSequenceNumber());
			m_pBodyIDVector.push_back(vJoltBodyID);

			uint32_t vIndex = boxID.GetIndex();
			m_pBodyIDMap[vIndex] = _entityID;
			TDS_INFO("Box Created!");
			_rigidbody->SetBodyID(vJoltBodyID);
		}
		else if (GetCapsuleCollider(_entityID))
		{
			CapsuleCollider* vCapsule = GetCapsuleCollider(_entityID);
			JPH::CapsuleShapeSettings s_capsuleSettings(vCapsule->GetHeight(), vCapsule->GetRadius());
			JPH::ShapeSettings::ShapeResult result = s_capsuleSettings.Create();
			JPH::ShapeRefC capsuleShape = result.Get();
			JPH::BodyCreationSettings b_capsuleSetting
			(
				capsuleShape,
				JoltToTDS::ToVec3(_transform->GetPosition()),
				JoltToTDS::ToQuat((_transform->GetRotation())),
				vMotionType,
				JoltLayers::GetObjectLayer(_rigidbody->GetMotionTypeInt(), vCapsule->GetIsTrigger())
			);
			b_capsuleSetting.mFriction = _rigidbody->GetFriction();
			b_capsuleSetting.mRestitution = _rigidbody->GetRestitution();
			b_capsuleSetting.mGravityFactor = (_rigidbody->GetUseGravity()) ? _rigidbody->GetGravityFactor() : 0.0f;
			b_capsuleSetting.mLinearDamping = _rigidbody->GetLinearDamping();
			b_capsuleSetting.mAngularDamping = _rigidbody->GetAngularDamping();
			b_capsuleSetting.mAllowSleeping = false;
			//b_capsuleSetting.mIsSensor = b_capsuleSetting->GetIsTrigger();
			
			JPH::BodyID capsuleID = m_pSystem->GetBodyInterface().CreateAndAddBody(b_capsuleSetting, JPH::EActivation::Activate);
			
			JoltBodyID vJoltBodyID(capsuleID.GetIndexAndSequenceNumber());
			m_pBodyIDVector.push_back(vJoltBodyID);

			uint32_t vIndex = capsuleID.GetIndex();
			m_pBodyIDMap[vIndex] = _entityID;

			TDS_INFO("Capsule Created!");
			_rigidbody->SetBodyID(vJoltBodyID);

		}
		else
		{
			TDS_ASSERT(false, "No collider found for entity, for now rigidbody need to have collider")
		}
		
	}


	void PhysicsSystem::JPH_Raycast(const EntityID& entities, Transform* _transform, RigidBody* _rigidbody, bool isPlayer)

	{
		//float vRayScale = _rigidbody->getRayScale();
		////float offsetDistance = _transform->GetScale().x + 100.0f;
		//JPH::Vec3 vRayOrigin = JoltToTDS::ToVec3(_transform->GetPosition());
		//JPH::Vec3 vRayDirection = JoltToTDS::ToVec3(GraphicsManager::getInstance().GetCamera().getForwardVector());

		//JPH::RayCast ray{ vRayOrigin, vRayDirection * 20.0f * vRayScale };
		//collector->Reset();
		//m_pSystem->GetBroadPhaseQuery().CastRay(ray, *collector);
		float offsetDistance = _transform->GetScale().x * 0.5f;
		JPH::Vec3 vRayOrigin = JoltToTDS::ToVec3(_transform->GetPosition() + GraphicsManager::getInstance().GetCamera().getForwardVector() * offsetDistance);
		JPH::Vec3 vRayDirection = JoltToTDS::ToVec3(GraphicsManager::getInstance().GetCamera().getForwardVector());
		JPH::Vec3 vScaledRayDir = vRayDirection * _rigidbody->getRayScale();

		JPH::RayCast ray{ vRayOrigin, vScaledRayDir };
		collector->Reset();
		m_pSystem->GetBroadPhaseQuery().CastRay(ray, *collector);

		const JPH::BroadPhaseQuery* broadphase = &m_pSystem->GetBroadPhaseQuery();

		if (collector->HadHit())
		{
			JPH::BroadPhaseCastResult* result = collector->mHits.data();
			auto entityID = findEntityByID(result->mBodyID.GetIndex());
			if (entityID.has_value())
			{
				GetRigidBody(entityID.value())->setIsRayHit(true);
			}
		}

	}

	std::optional<EntityID> TDS::PhysicsSystem::findEntityByID(uint32_t key)
	{
		auto it = m_pBodyIDMap.find(key);
		if (it != m_pBodyIDMap.end()) {
			// Key found, return the corresponding EntityID
			return it->second;
		}
		else {
			// Key not found
			return std::nullopt;
		}
	}
}
