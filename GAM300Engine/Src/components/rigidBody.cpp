/*!*************************************************************************
****
\file rigidBody.cpp
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program defines the functions in the RigidBody component class
****************************************************************************
***/

#include "components/rigidBody.h"

RTTR_REGISTRATION
{
	using namespace TDS;

	rttr::registration::class_<RigidBody>("Rigid Body")
		.property("Mass", &RigidBody::mMass)
		.property("Linear Velocity", &RigidBody::mLinearVelocity)
		.property("Angular Velocity", &RigidBody::mAngularVelocity)
		.property("Friction", &RigidBody::mFriction)
		.property("Restitution", &RigidBody::mRestitution)
		.property("GravityFactor", &RigidBody::mGravityFactor)
		.property("LinearDamping", &RigidBody::mLinearDamping)
		.property("AngularDamping", &RigidBody::mAngularDamping)
		.property("MotionType", &RigidBody::mMotionType)
		.property("UseGravity", &RigidBody::mUseGravity)
		.property("sensorActivate", &RigidBody::sensorActivated)
		.property("IsRayCast", &RigidBody::mIsRayCast)
		.property("IsPlayer", &RigidBody::mIsPlayer)
		.property("RayOrigin", &RigidBody::mRayOrigin)
		.property("RayDirection", &RigidBody::mRayDirection)
		.property("RayScale", &RigidBody::mRayScale);

	rttr::registration::enumeration<RigidBody::MotionType>("MotionType")
		(
			rttr::value("STATIC", RigidBody::MotionType::STATIC),
			rttr::value("DYNAMIC", RigidBody::MotionType::DYNAMIC)
		);
}

namespace TDS
{
	/*!*************************************************************************
	Initializes the RigidBody component when created
	****************************************************************************/
	RigidBody::RigidBody() : mLinearVelocity(Vec3(0.0f, 0.0f, 0.0f)),
							 mAngularVelocity(Vec3(0.0f, 0.0f, 0.0f)),
							 mFriction(0.2f),
							 mRestitution(0.0f),
							 mMass(1.0f),
							 mGravityFactor(1.0f),
							 mLinearDamping(0.05f),
							 mAngularDamping(0.05f),
							 mBodyID(JoltBodyID()),
							 mMotionType(MotionType::STATIC),
							 mUseGravity(false),
							 sensorActivated(false),
							 mIsRayCast(false),
							 mRayOrigin(Vec3(0.0f, 0.0f, 0.0f)),
							 mRayDirection(Vec3(0.0f, 0.0f, 0.0f)),
							 mRayScale(1.f),
							 mIsRayHit(false),
							 mIsPlayer(false)
	{ }


	/*!*************************************************************************
	Initializes the RigidBody component when created, given another RigidBody
	component to move (for ECS)
	****************************************************************************/
	RigidBody::RigidBody(RigidBody&& toMove) noexcept : mLinearVelocity(toMove.mLinearVelocity),
														mAngularVelocity(toMove.mAngularVelocity),
														mFriction(toMove.mFriction),
														mRestitution(toMove.mRestitution),
														mMass(toMove.mMass),
														mGravityFactor(toMove.mGravityFactor),
														mLinearDamping(toMove.mLinearDamping),
														mAngularDamping(toMove.mAngularDamping),
														mBodyID(toMove.mBodyID),
														mMotionType(toMove.mMotionType),
														mUseGravity(toMove.mUseGravity),
														sensorActivated(toMove.sensorActivated),
														mIsRayCast(toMove.mIsRayCast),
														mRayOrigin(toMove.mRayOrigin),
														mRayDirection(toMove.mRayDirection),
														mRayScale(toMove.mRayScale),
														mIsRayHit(toMove.mIsRayHit),
														mIsPlayer(toMove.mIsPlayer)

	{ }

	RigidBody* GetRigidBody(EntityID entityID)
	{
		return ecs.getComponent<RigidBody>(entityID);
	}
}