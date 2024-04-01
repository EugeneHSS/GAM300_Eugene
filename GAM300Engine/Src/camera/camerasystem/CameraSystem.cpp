/*!*************************************************************************
****
\file CameraSystem.cpp
\author Matthew Cheung
\par DP email: j.cheung@digipen.edu
\date  10-11-2023
\brief  This program defines functions of CameraSystem class
****************************************************************************
***/

#include "camera/Camerasystem/CameraSystem.h"
#include "Rendering/GraphicsManager.h"
namespace TDS 
{
	TDSCamera* CameraSystem::m_GameCamera = nullptr;
	void CameraSystem::CameraSystemInit()
	{
	}

	void CameraSystem::CameraSystemUpdate(const float dt, const std::vector<EntityID>& entities, Transform* _transform, CameraComponent* _cameracomponent)
	{
		static bool set = false;

		if (!entities.size())
		{
			return;
		}

		int entityIndex = 0;
		for (; entityIndex < entities.size() && (!ecs.getEntityIsEnabled(entities[entityIndex]) || !ecs.getComponentIsEnabled<CameraComponent>(entities[entityIndex])); ++entityIndex);
		if (entityIndex == entities.size())
		{
			set = false;
			return;
		}

		if (!GetIsPlaying())
		{
			set = true;
			m_GameCamera = &GraphicsManager::getInstance().GetCamera();
			SetGameCamera(_cameracomponent[entityIndex]);
			SetIsPlaying(true);
		}

		//static float pitch = _transform[entityIndex].GetRotation().x;
		//static float yaw = _transform[entityIndex].GetRotation().y;

		m_GameCamera->setPitch(_transform[entityIndex].GetRotation().x);
		m_GameCamera->setYaw(_transform[entityIndex].GetRotation().y + 90);
		m_GameCamera->setPosition(_transform[entityIndex].GetPosition());
		m_GameCamera->GetUpdateViewMatrix();
		//_cameracomponent[entityIndex].setForwardVector(m_GameCamera->getForwardVector());
		_cameracomponent[entityIndex].setForwardVector(GraphicsManager::getInstance().GetCamera().getForwardVector());
		_cameracomponent[entityIndex].setRightVector(GraphicsManager::getInstance().GetCamera().getRightVector());

		//pitch = _transform[entityIndex].GetRotation().x;
		//yaw = _transform[entityIndex].GetRotation().y;
	}

	void CameraSystem::SetGameCamera(CameraComponent& _camera)
	{
		m_GameCamera->setYaw(_camera.getYaw());
		m_GameCamera->setPitch(_camera.getPitch());
		m_GameCamera->setPosition(_camera.getPosition());
		m_GameCamera->setSpeed(_camera.getSpeed());
		m_GameCamera->setFov(_camera.getFOV());
		m_GameCamera->setMouseSensitivity(_camera.getMouseSensitivity());
		m_GameCamera->setForwardVector(GraphicsManager::getInstance().GetCamera().getForwardVector());
		m_GameCamera->setRightVector(GraphicsManager::getInstance().GetCamera().getRightVector());
	}

	/*void CameraSystem::UpdateViewMatrixSystem(CameraComponent* _cameracomponent)
	{
		// calculate the new Front vector
		Vec3 front;
		front.x = Mathf::Cos(_cameracomponent->getYaw() * Mathf::Deg2Rad) * Mathf::Cos(_cameracomponent->getPitch() * Mathf::Deg2Rad);
		front.y = Mathf::Sin(_cameracomponent->getPitch() * Mathf::Deg2Rad);
		front.z = Mathf::Sin(_cameracomponent->getYaw() * Mathf::Deg2Rad) * Mathf::Cos(_cameracomponent->getPitch() * Mathf::Deg2Rad);
		m_Front = Vec3::Normalize(front);
		// also re-calculate the Right and Up vector
		m_Right = Vec3::Normalize(Vec3::Cross(m_Front, _cameracomponent->getmWorldUp()));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		m_Up = Vec3::Normalize(Vec3::Cross(m_Right, m_Front));

		m_Down = -m_Up;
		TDS_INFO(m_Front);
		TDS_INFO(m_Right);
		TDS_INFO(m_Up);
	}*/

	bool CameraSystem::movingCameraSystem()
	{
		//key keys;
		//if (Input::keystatus == Input::KeyStatus::PRESSED || Input::keystatus == Input::KeyStatus::REPEATED)
		//{
		//	switch (Input::keyCode)
		//	{
		//	case TDS_W:
		//		return keys.up = true;
		//		break;

		//	case TDS_A:
		//		return keys.left = true;
		//		break;

		//	case TDS_S:
		//		return keys.down = true;
		//		break;

		//	case TDS_D:
		//		return keys.right = true;

		//	default:
		//		return false;
		//	}
		//}
		//else
		//{
		//	keys.up = false;
		//	keys.left = false;
		//	keys.down = false;
		//	keys.right = false;
		//}
		//return false;

		auto& inputSystem = InputSystem::GetInstance();

		//return inputSystem->isKeyPressed('W') || inputSystem->isKeyPressed('A') || inputSystem->isKeyPressed('S') || inputSystem->isKeyPressed('D') ||
		return inputSystem->isKeyDown('W') || inputSystem->isKeyDown('A') || inputSystem->isKeyDown('S') || inputSystem->isKeyDown('D');
	}
}
