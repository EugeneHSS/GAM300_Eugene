#ifndef TDS_CAMERA
#define TDS_CAMERA

#include "dotnet/ImportExport.h"

namespace TDS
{
	class TDSCamera
	{
	public:

		DLL_API TDSCamera(float Yaw, float pitch,/* float aspectRatio,*/ float zNear = 0.1f, float zFar = 1000.f, Vec3 position = { 0.0f, 0.0f, 3.0f }, Vec3 up = { 0.0f, 1.0f, 0.0f });

		//using lookat and Euler Angles
		DLL_API Mat4 GetViewMatrix() const;

		DLL_API void UpdateCamera(float deltaTime, bool gameIsPlaying);

		DLL_API bool moving();

		DLL_API void ProcessMouseMovement(float mousex, float mousey);

		DLL_API Vec3 getPosition()const { return m_Position; }

		// Setters for Game Camera System
		DLL_API void setPosition(Vec3 position) { m_Position = position; }
		DLL_API void setYaw(float yaw) { m_Yaw = yaw; }
		DLL_API void setPitch(float pitch) { m_Pitch = pitch; }
		DLL_API void setSpeed(float speed) { m_Speed = speed; }
		DLL_API void setMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }
		DLL_API void setForwardVector(Vec3 fwdvector) { m_Front = fwdvector; }
		DLL_API void setRightVector(Vec3 rgtvector) { m_Right = rgtvector; }
		DLL_API void setFov(float fov) { m_Fov = fov; }
		DLL_API void GetUpdateViewMatrix() { updateViewMatrix(); } // calling updateViewMatrix() for CameraSystem
		DLL_API void setEditorCamera(bool editorCamera) { m_EditorCamera = editorCamera; }
		DLL_API bool getEditorCamera() const { return m_EditorCamera; }
		DLL_API bool getScrollWheel() const { return m_ScrollWheel; }
		DLL_API void setScrollWheel(bool scrollWheel) { m_ScrollWheel = scrollWheel; }
		DLL_API Vec3 getForwardVector() { return m_Front; }
		DLL_API Vec3 getRightVector() { return m_Right; }

		float m_Fov{ 45.f };
	private:

		struct {
			bool left{ false };
			bool right{ false };
			bool up{ false };
			bool down{ false };
		}keys;
		//camera attributes
		Vec3	m_Position{};
		Vec3	m_Front{};
		Vec3	m_Up{ 0.f,0.f,1.f };
		Vec3	m_Right{};
		Vec3	m_Down{};
		Vec3	m_WorldUp{};

		Mat4	m_ProjMatrix{};
		//euler angles
		float	m_Yaw{};
		float	m_Pitch{};

		//option to toggle with
		float	m_Speed{ 5.0f };
		float	m_mouseSensitivity{ 0.1f }; //should it be in input???
		float	m_ZoomLevel{};
		float	m_Width{};
		float	m_Height{};

		void updateViewMatrix();
		//void updateProjection();
		void translate(const float& deltaWheel); // act like zoom?

		bool m_EditorCamera = true;
		bool m_ScrollWheel = true;

	};

}//namespace TDS


#endif // !TDS_CAMERA