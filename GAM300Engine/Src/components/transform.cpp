/*!*************************************************************************
****
\file transform.cpp
\author Go Ruo Yan
\par DP email: ruoyan.go@digipen.edu
\date 28-9-2023
\brief  This program defines the functions in the Transform component class
****************************************************************************
***/

#include "components/transform.h"

RTTR_REGISTRATION
{
	using namespace TDS;

	rttr::registration::class_<Transform>("Transform")
		.property("Position", &Transform::mPosition)
		.property("Scale", &Transform::mScale)
		.property("Rotation", &Transform::mRotation)
		.property("FakePosition", &Transform::mFakePosition)
		.property("FakeRotation", &Transform::mFakeRotation)
		.property("FakeScale", &Transform::mFakeScale)
		.property("OldPosition", &Transform::mOldPosition)
		.property("OldScale", &Transform::mOldScale)
		.property("OldRotation", &Transform::mOldRotation)
		.property("OldFakePosition", &Transform::mOldFakePosition)
		.property("OldFakeScale", &Transform::mOldFakeScale)
		.property("OldFakeRotation", &Transform::mOldFakeRotation)
		.property("ParentPosition", &Transform::mParentPosition);

}

namespace TDS
{
	/*!*************************************************************************
	Initializes the Transform component when created
	****************************************************************************/
	Transform::Transform() : mPosition(Vec3(0.0f, 0.0f, 0.0f)),
		mScale(Vec3(1.0f, 1.0f, 1.0f)),
		mRotation(Vec3(0.0f, 0.f, 0.0f)),
		mFakePosition(Vec3(0.0f, 0.0f, 0.0f)),
		mFakeScale(Vec3(1.0f, 1.0f, 1.0f)),
		mFakeRotation(Vec3(0.0f, 0.0f, 0.0f)),
		mTransformMatrix(Mat4::zero()),
		mFakeTransform(Mat4::zero()),
		mOffsetScale(Vec3(0.f, 0.f, 0.f)),
		mOffsetPos(Vec3(0.f, 0.f, 0.f)),
		mOffsetMatrix(Mat4::zero())
	{ }

	/*!*************************************************************************
	Initializes the Transform component when created, given another Transform
	component to move (for ECS)
	****************************************************************************/
	Transform::Transform(Transform&& toMove) noexcept :
		mPosition(toMove.mPosition),
		mScale(toMove.mScale),
		mRotation(toMove.mRotation),
		mTransformMatrix(toMove.mTransformMatrix),
		mFakePosition(toMove.mFakePosition),
		mFakeRotation(toMove.mFakeRotation),
		mFakeScale(toMove.mFakeScale),
		mFakeTransform(toMove.mFakeTransform)
	{ }

	Vec4 Transform::getLocalPosition(EntityID parent)
	{

		Mat4 parentTransformationMatrix = GetTransform(parent)->GenerateTransform().inverse();
		Vec4 localPosition = parentTransformationMatrix * Vec4(mPosition, 1.f);

		localPosition.x /= localPosition.w;
		localPosition.y /= localPosition.w;
		localPosition.z /= localPosition.w;

		return localPosition;


		return mPosition;
	}
	void Transform::setLocalPosition(EntityID parent, Vec4 localPosition)
	{

		localPosition.x *= localPosition.w;
		localPosition.y *= localPosition.w;
		localPosition.z *= localPosition.w;

		Vec3 newPosition = GetTransform(parent)->GenerateTransform() * localPosition;
		SetPosition(newPosition);
		/*mPosition = GetTransform(parent)->GenerateTransform() * localPosition;*/
		//mFakePosition = GetTransform(parent)->GenerateFakeTransform() * localPosition;
	}

	Vec4 Transform::getLocalScale(EntityID parent)
	{
		Mat4 parentTransformationMatrix = GetTransform(parent)->GenerateTransform().inverse();
		Vec4 localScale = parentTransformationMatrix * Vec4(mScale, 1.f);

		localScale.x /= localScale.w;
		localScale.y /= localScale.w;
		localScale.z /= localScale.w;

		return localScale;
	}
	void Transform::setLocalScale(EntityID parent, Vec4 localScale)
	{
		localScale.x *= localScale.w;
		localScale.y *= localScale.w;
		localScale.z *= localScale.w;

		Vec3 newPosition = GetTransform(parent)->GenerateTransform() * localScale;
		SetPosition(newPosition);
		//mPosition = GetTransform(parent)->GenerateTransform() * localScale;
	}

	Vec4 Transform::getLocalRotation(EntityID parent)
	{
		Mat4 parentTransformationMatrix = GetTransform(parent)->GenerateTransform().inverse();
		Vec4 localRotation = parentTransformationMatrix * Vec4(mRotation, 1.f);

		localRotation.x /= localRotation.w;
		localRotation.y /= localRotation.w;
		localRotation.z /= localRotation.w;

		return localRotation;
	}
	void Transform::setLocalRotation(EntityID parent, Vec4 localRotation)
	{
		localRotation.x *= localRotation.w;
		localRotation.y *= localRotation.w;
		localRotation.z *= localRotation.w;

		Vec3 newPosition = GetTransform(parent)->GenerateTransform() * localRotation;
		SetPosition(newPosition);
	}



	DLL_API Mat4 Transform::GenerateFakeTransform()
	{
		Vec3 offsetFromFakeToReal = mPosition - mFakePosition;
		Quat qRot = Quat(mFakeRotation);
		Mat4 scaleMatrix = Mat4::Scale(mFakeScale);
		Mat4 rotationMatrix = Mat4(Quat::toMat4(qRot));
		Mat4 translationMatrix = Mat4::Translate(mFakePosition);

		Mat4 transform = translationMatrix * rotationMatrix * scaleMatrix;
		mFakeTransform = transform;
		mFakePosition = transform.GetPosition();
		Vec3 newOffset = mPosition - mFakePosition;

		if (newOffset != offsetFromFakeToReal)
		{
			mPosition += newOffset;
		}


		mIsDirty = false;
		return mFakeTransform;

	}

	DLL_API Mat4 Transform::GenerateChildFakeTransform()
	{
		Vec3 offsetFromFakeToReal = mPosition - mFakePosition;
		Vec3 offsetToOrigin = mPosition - mParentPosition;
		Mat4 translateToOrigin = Mat4::Translate(offsetToOrigin);

		Quat qRot = Quat(mFakeRotation);
		Mat4 scaleMatrix = Mat4::Scale(mFakeScale);
		Mat4 rotationMatrix = Mat4(Quat::toMat4(qRot));
		Mat4 translationMatrix = Mat4::Translate(mFakePosition);

		Mat4 transform = translationMatrix * rotationMatrix * scaleMatrix;


		Vec3 offsetBackToParent = mParentPosition - mPosition;
		Mat4 translateBackToParent = Mat4::Translate(offsetBackToParent);

		mFakeTransform =  translateBackToParent * transform * translateToOrigin;
		//mFakePosition = mFakeTransform.GetPosition();

		//Vec3 newOffset = mPosition - mFakePosition;
		//Vec3 position = mPosition;
		//if (newOffset != offsetFromFakeToReal)
		//{
		//	position += newOffset;
		//}
		//SetPosition(position);
		mIsDirty = false;
		return mFakeTransform;
	}

	Transform* GetTransform(EntityID entityID)
	{
		return ecs.getComponent<Transform>(entityID);
	}
}
