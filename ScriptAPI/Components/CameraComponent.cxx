#include "CameraComponent.hxx"
#include "../TypeConversion.hxx"
#include "../EngineInterface.hxx"

namespace ScriptAPI
{
	// ISTRIGGER =============================================================================
	// Private
	Vector3 CameraComponent::CameraOffset::get()
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return Vector3(0.f, 0.f, 0.f);
		}

		return Vector3(TDS::GetCameraComponent(entityID)->getPosition());
	}
	void CameraComponent::CameraOffset::set(Vector3 value)
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::GetCameraComponent(entityID)->setPosition(value.X, value.Y, value.Z);
	}

	// Public
	Vector3 CameraComponent::GetCameraOffset()
	{
		return CameraOffset;
	}
	void CameraComponent::SetCameraOffset(Vector3 value)
	{
		CameraOffset = value;
	}
	void CameraComponent::SetCameraOffsetX(float valueX)
	{
		Vector3 oldCameraOffset = CameraOffset;
		CameraOffset = Vector3(valueX, oldCameraOffset.Y, oldCameraOffset.Z);
	}
	void CameraComponent::SetCameraOffsetY(float valueY)
	{
		Vector3 oldCameraOffset = CameraOffset;
		CameraOffset = Vector3(oldCameraOffset.X, valueY, oldCameraOffset.Z);
	}
	void CameraComponent::SetCameraOffsetZ(float valueZ)
	{
		Vector3 oldCameraOffset = CameraOffset;
		CameraOffset = Vector3(oldCameraOffset.X, oldCameraOffset.Y, valueZ);
	}

	// YAW ===================================================================================
	// Private
	float CameraComponent::Yaw::get()
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return 0.f;
		}

		return TDS::GetCameraComponent(entityID)->getYaw();
	}
	void CameraComponent::Yaw::set(float value)
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::GetCameraComponent(entityID)->setYaw(value);
	}

	// Public
	float CameraComponent::GetYaw()
	{
		return Yaw;
	}
	void CameraComponent::SetYaw(float value)
	{
		Yaw = value;
	}

	// PITCH =================================================================================
	// Private
	float CameraComponent::Pitch::get()
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return 0.f;
		}

		return TDS::GetCameraComponent(entityID)->getPitch();
	}
	void CameraComponent::Pitch::set(float value)
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::GetCameraComponent(entityID)->setPitch(value);
	}

	// Public
	float CameraComponent::GetPitch()
	{
		return Pitch;
	}
	void CameraComponent::SetPitch(float value)
	{
		Pitch = value;
	}

	// SPEED =================================================================================
	// Private
	float CameraComponent::Speed::get()
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return 0.f;
		}

		return TDS::GetCameraComponent(entityID)->getSpeed();
	}
	void CameraComponent::Speed::set(float value)
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::GetCameraComponent(entityID)->setSpeed(value);
	}

	// Public
	float CameraComponent::GetSpeed()
	{
		return Speed;
	}
	void CameraComponent::SetSpeed(float value)
	{
		Speed = value;
	}

	// FIELDOFVIEW ===========================================================================
	// Private
	float CameraComponent::FieldOfView::get()
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return 0.f;
		}

		return TDS::GetCameraComponent(entityID)->getFOV();
	}
	void CameraComponent::FieldOfView::set(float value)
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::GetCameraComponent(entityID)->setFOV(value);
	}

	// Public
	float CameraComponent::GetFieldOfView()
	{
		return FieldOfView;
	}
	void CameraComponent::SetFieldOfView(float value)
	{
		FieldOfView = value;
		TDS::GetCameraComponent(entityID)->setFOV(value);
	}

	// MOUSESENSITIVITY ======================================================================
	// Private
	float CameraComponent::MouseSensitivity::get()
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return 0.f;
		}

		return TDS::GetCameraComponent(entityID)->getMouseSensitivity();
	}
	void CameraComponent::MouseSensitivity::set(float value)
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		TDS::GetCameraComponent(entityID)->setMouseSensitivity(value);
	}

	// Public
	float CameraComponent::GetMouseSensitivity()
	{
		return MouseSensitivity;
	}
	void CameraComponent::SetMouseSensitivity(float value)
	{
		MouseSensitivity = value;
	}

	// FORWARDVECTOR =========================================================================
	// 
	Vector3 CameraComponent::getForwardVector()
	{		
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return Vector3(0.f, 0.f, 0.f);
		}

		return Vector3(TDS::GetCameraComponent(entityID)->getForwardVector());
	}
	void CameraComponent::setForwardVector(float angle)
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		//TDS::Vec3 forwardVector = TDS::GetCameraComponent(entityID)->getForwardVector();
		//TDS::Mat3 rotatedMatrix = TDS::Mat3::RotateY(angle);
		//TDS::GetCameraComponent(entityID)->setForwardVector(rotatedMatrix * forwardVector);
	}

	// RIGHTVECTOR =========================================================================
	// 

	Vector3 CameraComponent::getRightVector()
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return Vector3(0.f, 0.f, 0.f);
		}

		return Vector3(TDS::GetCameraComponent(entityID)->getRightVector());
	}
	void CameraComponent::setRightVector(float angle)
	{
		// May wanna change to a function
		if (!TDS::GetCameraComponent(entityID))
		{
			// throw error instead (not sure how)
			return;
		}

		//TDS::Vec3 forwardVector = TDS::GetCameraComponent(entityID)->getForwardVector();
		//TDS::Mat3 rotatedMatrix = TDS::Mat3::RotateY(angle);
		//TDS::GetCameraComponent(entityID)->setForwardVector(rotatedMatrix * forwardVector);
	}

	// CONSTRUCTOR ===========================================================================
	CameraComponent::CameraComponent(TDS::EntityID ID) : entityID (ID), transform(TransformComponent(ID))
	{
		gameObject = EngineInterface::GetGameObject(ID);
	}

	void CameraComponent::SetEntityID(TDS::EntityID ID)
	{
		entityID = ID;
		transform = TransformComponent(ID);
		gameObject = EngineInterface::GetGameObject(ID);
	}

	TDS::EntityID CameraComponent::GetEntityID()
	{
		return entityID;
	}

	void CameraComponent::SetEnabled(bool enabled)
	{
		TDS::setComponentIsEnable("Camera Component", GetEntityID(), enabled);
	}
	bool CameraComponent::GetEnabled()
	{
		return TDS::getComponentIsEnable("Camera Component", GetEntityID());
	}

	/*void CameraComponent::SetGameCamera(CameraComponent camera)
	{
		TDS::GetCameraComponent(entityID)->setYaw(camera.GetYaw());
		TDS::GetCameraComponent(entityID)->setPitch(camera.GetPitch());
		TDS::GetCameraComponent(entityID)->setPosition(TDS::Vec3(camera.transform.GetPosition().X,
			camera.transform.GetPosition().Y, camera.transform.GetPosition().Z));
		TDS::GetCameraComponent(entityID)->setSpeed(camera.GetSpeed());
		TDS::GetCameraComponent(entityID)->setFOV(camera.GetFieldOfView());
		TDS::GetCameraComponent(entityID)->setMouseSensitivity(camera.GetMouseSensitivity());
	}*/
}