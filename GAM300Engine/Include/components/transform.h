/*!*************************************************************************
****
\file transform.h
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program declares the functions in the transform component class
****************************************************************************
***/

#ifndef TRANSFORMCOMPONENT
#define TRANSFORMCOMPONENT

#include "TDSMath.h"
#include "ecs/ecs.h"

namespace TDS
{
	class Transform : public IComponent
	{
	public:
		/*!*************************************************************************
		Initializes the Transform component when created
		****************************************************************************/
		DLL_API Transform();
		/*!*************************************************************************
		Initializes the Transform component when created, given another Transform
		component to move (for ECS)
		****************************************************************************/
		DLL_API Transform(Transform&& toMove) noexcept;
		/*!*************************************************************************
		Destructor for the Transform component class
		****************************************************************************/
		DLL_API ~Transform() = default;
		/*!*************************************************************************
		Getter and setter functions for the variables in the Transform component
		class
		****************************************************************************/
		DLL_API Vec3 GetPosition() { return mPosition; }

		DLL_API void SetRealPosition(Vec3 position)
		{
			mPosition = position;
			mIsDirty = true;
		}
		DLL_API void SetRealScale(Vec3 scale)
		{
			mScale = scale;
			mIsDirty = true;
		}
		DLL_API void SetRealRotate(Vec3 rotate)
		{
			mRotation = rotate;
			mIsDirty = true;
		}
		DLL_API void SetPosition(Vec3 position)
		{
			mOldPosition = mPosition;
			mPosition = position;
			mOldFakePosition = mFakePosition;
			mOffsetPos = (mPosition - mOldPosition);
			mFakePosition += (mPosition - mOldPosition);
			mIsDirty = true;
		}
		DLL_API void SetPosition(float positionX, float positionY, float positionZ)
		{
			mOldPosition = mPosition;
			mPosition = Vec3(positionX, positionY, positionZ);
			mOldFakePosition = mFakePosition;
			mFakePosition += (mPosition - mOldPosition);
			mOffsetPos = (mPosition - mOldPosition);
			mIsDirty = true;

		}

		DLL_API Vec3& GetOffsetPos() { return mOffsetPos; };
		DLL_API void SetOffSetPos(Vec3 Pos) { mOffsetPos = Pos; }
		DLL_API void SetOffSetPos(float posX, float posY, float posZ) { mOffsetPos = { posX, posY, posZ }; mIsDirty = true;}

		DLL_API Vec3 GetScale() { return mScale; }
		DLL_API void SetScale(Vec3 scale)
		{
			Vec3 oldScale = mScale;
			mScale = scale;
			mOldFakeScale = mFakeScale;
			mFakeScale += (mScale - oldScale);
			mIsDirty = true;
		}
		DLL_API void SetScale(float scaleX, float scaleY, float scaleZ)
		{
			mOldRotation = mScale;
			mScale = Vec3(scaleX, scaleY, scaleZ);
			mOldFakeScale = mFakeScale;
			mFakeScale += (mScale - mOldRotation);
			mIsDirty = true;
		}

		DLL_API Vec3& GetOffsetScale() { return mOffsetScale; }
		DLL_API void SetOffSetScale(Vec3 Scale) { mOffsetScale = Scale; mIsDirty = true;}
		DLL_API void SetOffSetScale(float scaleX, float scaleY, float scaleZ) { mOffsetScale = { scaleX, scaleY, scaleZ }; mIsDirty = true;}

		DLL_API Vec3 GetRotation() { return mRotation; }
		DLL_API void SetRotation(Vec3 rotation)
		{
			mOldScale = mRotation;
			mRotation = rotation;
			mOldFakeRotation = mFakeRotation;
			mFakeRotation += (mRotation - mOldScale);
			mIsDirty = true;

		}
		DLL_API void SetRotation(float rotationX, float rotationY, float rotationZ)
		{
			Vec3 oldRotation = mRotation;
			mRotation = Vec3(rotationX, rotationY, rotationZ);
			mOldFakeRotation = mFakeRotation;
			mFakeRotation += (mRotation - oldRotation);
			mIsDirty = true;

		}

		DLL_API Mat4 GetTransformMatrix() const { return mTransformMatrix; }
		DLL_API Mat4 GetFakeTransform() const { return mFakeTransform; }
		DLL_API Mat4 GetOffsetMatrix() const { return mOffsetMatrix; }
		DLL_API void SetTransform(Vec3 translate, Vec3 rotate, Vec3 scale)
		{
			Quat qRot = Quat(rotate);
			Mat4 scaleM4 = Mat4::Scale(scale);
			Mat4 rotM4 = Mat4(Quat::toMat4(qRot));
			Mat4 transM4 = Mat4::Translate(translate);
			mTransformMatrix = transM4 * rotM4 * scaleM4;
		}
		DLL_API Mat4 GenerateTransform() {
			Quat qRot = Quat(mRotation);
			Mat4 scaleM4 = Mat4::Scale(mScale);
			Mat4 rotM4 = Mat4(Quat::toMat4(qRot));
			Mat4 transM4 = Mat4::Translate(mPosition);
			mTransformMatrix = transM4 * rotM4 * scaleM4;

			scaleM4 = Mat4::Scale(mScale + mOffsetScale);
			transM4 = Mat4::Translate(mPosition - mOffsetPos);
			mOffsetMatrix = transM4 * rotM4 * scaleM4;

			return mTransformMatrix;
		}
		DLL_API Mat4 GenerateTransformInverse() 
		{
			GenerateTransform();
			return mTransformMatrix.inverse();
		}

		DLL_API Vec4 getLocalPosition(EntityID parent);
		DLL_API void setLocalPosition(EntityID parent, Vec4 localPosition);

		DLL_API Vec4 getLocalScale(EntityID parent);
		DLL_API void setLocalScale(EntityID parent, Vec4 localScale);

		DLL_API Vec4 getLocalRotation(EntityID parent);
		DLL_API void setLocalRotation(EntityID parent, Vec4 localRotation);

		DLL_API Vec3 getForwardVector()
		{
			Vec3 rotationRad = mRotation * Mathf::Deg2Rad;
			Quat quaternionRotation = Quat(rotationRad);
			return (quaternionRotation * Vec3(0, 0, 1));
		}
		DLL_API Vec3 getRightVector()
		{
			Vec3 forwardVector = getForwardVector();
			Vec3 rightVector = Vec3::Cross(Vec3(0, 1, 0), forwardVector).normalize();
			return rightVector;
		}

		DLL_API Vec3 GetFakePosition()
		{
			return mFakePosition;
		}
		DLL_API Vec3 GetFakeRotation()
		{
			return mFakeRotation;
		}
		DLL_API Vec3 GetFakeScale()
		{
			return mFakeScale;
		}
		DLL_API void SetDirty(bool condition)
		{
			mIsDirty = condition;
		}
		DLL_API bool isDirty()
		{
			return mIsDirty;
		}

		DLL_API Mat4 GenerateFakeTransform();

		DLL_API Mat4 GenerateChildFakeTransform();

		DLL_API void SetParentPosition(Vec3 parentPosition)
		{
			mParentPosition = parentPosition;
		}


		RTTR_ENABLE(IComponent);
		RTTR_REGISTRATION_FRIEND

	private:
		bool mIsDirty{ true };
		Vec3 mPosition;
		Vec3 mScale;
		Vec3 mRotation;

		Vec3 mOffsetScale;
		Vec3 mOffsetPos;

		Vec3 mFakeRotation;
		Vec3 mFakeScale;
		Vec3 mFakePosition;

		Vec3 mOldFakePosition;
		Vec3 mOldFakeRotation;
		Vec3 mOldFakeScale;

		Vec3 mOldPosition;
		Vec3 mOldScale;
		Vec3 mOldRotation;

		Vec3 mParentPosition = Vec3(0.f, 0.f, 0.f);

		Mat4 mTransformMatrix;
		Mat4 mOffsetMatrix;
		Mat4 mFakeTransform;

		




	};

	DLL_API Transform* GetTransform(EntityID entityID);
}

#endif // TRANSFORMCOMPONENT
