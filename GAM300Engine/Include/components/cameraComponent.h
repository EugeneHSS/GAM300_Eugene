/*!*************************************************************************
****
\file cameraComponent.h
\author Go Ruo Yan, Matthew Cheung
\par DP email: ruoyan.go@digipen.edu, j.cheung@digipen.edu
\date 28-9-2023
\brief  This program declares the functions in the cameraComponent class
****************************************************************************
***/

#ifndef CAMERACOMPONENT
#define CAMERACOMPONENT

#include "ecs/ecs.h"

namespace TDS
{
	class CameraComponent : public IComponent
	{
	public:
		/*!*************************************************************************
		Initializes the CameraComponent when created
		****************************************************************************/
		DLL_API CameraComponent();

		//DLL_API Mat4 GetViewMatrix() const;

		//DLL_API TDSCamera(float Yaw, float pitch, float zNear = 0.1f, float zFar = 10.f, Vec3 position = { 0.0f, 0.0f, 3.0f }, Vec3 up = { 0.0f, 1.0f, 0.0f });

		/*!*************************************************************************
		Initializes the CameraComponent when created, given another CameraComponent 
		to move (for ECS)
		****************************************************************************/
		DLL_API CameraComponent(CameraComponent&& toMove) noexcept;
		/*!*************************************************************************
		Destructor for the CameraComponent class
		****************************************************************************/
		DLL_API ~CameraComponent() = default;

		/*!*************************************************************************
		Getter functions
		****************************************************************************/
		DLL_API float& getYaw() { return m_Yaw; }
		DLL_API float& getPitch() { return m_Pitch; }
		DLL_API Vec3& getPosition() { return m_Position; }
		DLL_API float& getSpeed() { return m_Speed; }
		DLL_API float& getMouseSensitivity() { return m_mouseSensitivity; }
		DLL_API float& getFOV() { return m_Fov; }


		/*!*************************************************************************
		Setter functions
		****************************************************************************/
		DLL_API void setYaw(float yaw) { m_Yaw = yaw; }
		DLL_API void setPitch(float pitch) { m_Pitch = pitch; }
		DLL_API void setPosition(Vec3 position) { m_Position = position; }
		DLL_API void setPosition(float positionX, float positionY, float positionZ) { m_Position = Vec3(positionX, positionY, positionZ); }
		DLL_API void setSpeed(float speed) { m_Speed = speed; }
		DLL_API void setMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }
		DLL_API void setFOV(float fov) { m_Fov = fov; }
		DLL_API Vec3 getForwardVector() { return m_Front; }
		DLL_API void setForwardVector(Vec3 front) { m_Front = front; }
		DLL_API Vec3 getRightVector() { return m_Right; }
		DLL_API void setRightVector(Vec3 right) { m_Right = right; }
		RTTR_ENABLE(IComponent);
		RTTR_REGISTRATION_FRIEND
				
	private:

		//camera attributes
		Vec3	m_Position;

		//Mat4	m_ProjMatrix;
		//euler angles
		float	m_Yaw;
		float	m_Pitch;
		float	m_Fov;
		float	m_Speed;
		float	m_mouseSensitivity;

		Vec3 m_Front;
		Vec3 m_Right;

	};
		DLL_API CameraComponent* GetCameraComponent(EntityID entityID);
}

#endif // CAMERACOMPONENT