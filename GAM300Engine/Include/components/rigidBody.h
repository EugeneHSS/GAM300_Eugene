/*!*************************************************************************
****
\file rigidBody.h
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program declares the functions in the rigidBody component class
****************************************************************************
***/

#ifndef RIGIDBODYCOMPONENT
#define RIGIDBODYCOMPONENT

#include "Vector3.h"
#include "ecs/ecs.h"
#include "JoltPhysics/Implementation/Body/JoltBodyID.h"

namespace TDS
{
	class RigidBody : public IComponent
	{
	public:
		enum class MotionType : uint8_t//EMotionType in Jolt/.../MotionType.h
		{
			STATIC = 0,		///< Non movable
			DYNAMIC,		///< Responds to forces as a normal physics object
		};

		/*!*************************************************************************
		Initializes the RigidBody component when created
		****************************************************************************/
		DLL_API RigidBody();
		/*!*************************************************************************
		Initializes the RigidBody component when created, given another RigidBody
		component to move (for ECS)
		****************************************************************************/
		DLL_API RigidBody(RigidBody&& toMove) noexcept;
		/*!*************************************************************************
		Destructor for the RigidBody component class
		****************************************************************************/
		DLL_API ~RigidBody() = default;
		/*!*************************************************************************
		Getter and setter functions for the variables in the RigidBody component class
		****************************************************************************/
		DLL_API float& GetMass() { return mMass; }
		DLL_API void SetMass(float mass) { mMass = mass; }

		DLL_API Vec3& GetLinearVel() { return mLinearVelocity; }
		DLL_API void SetLinearVel(Vec3 velocity) { mLinearVelocity = velocity; }
		DLL_API void SetLinearVel(float velocityX, float velocityY, float velocityZ) { mLinearVelocity = Vec3(velocityX, velocityY, velocityZ); }

		DLL_API Vec3& GetAngularVel() { return mAngularVelocity; }
		DLL_API void SetAngularVel(Vec3 velocity) { mAngularVelocity = velocity; }
		DLL_API void SetAngularVel(float velocityX, float velocityY, float velocityZ) { mAngularVelocity = Vec3(velocityX, velocityY, velocityZ); }
		
		DLL_API float& GetFriction() { return mFriction; }
		DLL_API void SetFrictionCoefficient(float friction) { mFriction = friction; }

		DLL_API float& GetRestitution() { return mRestitution; }
		DLL_API void SetRestitution(float restitution) { mRestitution = restitution; }

		DLL_API float& GetGravityFactor() { return mGravityFactor; }
		DLL_API void SetGravityFactor(float gravityFactor) { mGravityFactor = gravityFactor; }
		
		DLL_API float& GetLinearDamping() { return mLinearDamping; }
		DLL_API void SetLinearDamping(float damping) { mLinearDamping = damping; }
		
		DLL_API float& GetAngularDamping() { return mAngularDamping; }
		DLL_API void SetAngularDamping(float damping) { mAngularDamping = damping; }

		DLL_API MotionType& GetMotionType() { return mMotionType; }
		DLL_API int GetMotionTypeInt() { return static_cast<int>(mMotionType); }
		DLL_API void SetMotionType(MotionType motionType) { mMotionType = motionType; }

		DLL_API JoltBodyID& GetBodyID() { return mBodyID; }
		DLL_API void SetBodyID(JoltBodyID bodyID) { mBodyID = bodyID; }

		DLL_API bool& GetUseGravity() { return mUseGravity; }
		DLL_API void SetUseGravity(bool useGravity) { mUseGravity = useGravity; }
		
		DLL_API bool& getSensorActivate() { return sensorActivated; }
		DLL_API void setSensorActivate(bool input) { sensorActivated = input; }

		DLL_API bool& getIsRayCast() { return mIsRayCast; }
		DLL_API void setIsRayCast(bool isRayCast) { mIsRayCast = isRayCast; }

		DLL_API Vec3& getRayDirection() { return mRayDirection; }
		DLL_API void setRayDirection(Vec3 direction) { mRayDirection = direction; }

		DLL_API Vec3& getRayOrigin() { return mRayOrigin; }
		DLL_API void setRayOrigin(Vec3 origin) { mRayOrigin = origin; }

		DLL_API float& getRayScale() { return mRayScale; }
		DLL_API void setRayScale(float scale) { mRayScale = scale; }

		DLL_API bool& getIsRayHit() { return mIsRayHit; }
		DLL_API void setIsRayHit(bool hit) { mIsRayHit = hit; }

		DLL_API bool& getIsPlayer() { return mIsPlayer; }
		DLL_API void setIsPlayer(bool isPlayer) { mIsPlayer = isPlayer; }

		RTTR_ENABLE(IComponent);
		RTTR_REGISTRATION_FRIEND

	private:
		float mMass;

		Vec3 mLinearVelocity;
		Vec3 mAngularVelocity;

		float mFriction;
		float mRestitution;
		float mGravityFactor;
		float mLinearDamping;
		float mAngularDamping;

		bool mUseGravity;
		bool sensorActivated;
		
		// Raycast
		Vec3 mRayDirection;
		Vec3 mRayOrigin;
		float mRayScale;
		bool mIsRayCast;
		bool mIsRayHit;
		bool mIsPlayer;

		MotionType	mMotionType;
		JoltBodyID	mBodyID;

	};

	DLL_API RigidBody* GetRigidBody(EntityID entityID);
}

#endif //RIGIDBODYCOMPONENT