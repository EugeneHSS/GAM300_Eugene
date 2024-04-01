#include "../Input/InputSystem.h"
#include <Windows.h>

namespace TDS
{
	// Unique pointer to instance of Scene
	std::unique_ptr<InputSystem> InputSystem::m_instance;
	Vec2 InputSystem::m_uiMousePos;

	InputSystem::InputSystem()
	{

	}


	InputSystem::~InputSystem()
	{
	}

	std::unique_ptr<InputSystem>& InputSystem::GetInstance()
	{
		if (m_instance == nullptr)
		{
			m_instance = std::make_unique<InputSystem>();
		}
		return m_instance;
	}

	void InputSystem::update()
	{
		POINT current_mouse_pos = {};
		::GetCursorPos(&current_mouse_pos);

		if (m_first_time)
		{
			m_old_mouse_pos = Point(current_mouse_pos.x, current_mouse_pos.y);
			m_first_time = false;
		}
		m_mouseMoved = false;
		if (current_mouse_pos.x != m_old_mouse_pos.x || current_mouse_pos.y != m_old_mouse_pos.y)
		{
			m_mouseMoved = true;
			m_mouse_delta = Point(current_mouse_pos.x - m_old_mouse_pos.x, current_mouse_pos.y - m_old_mouse_pos.y);
		}
		m_old_mouse_pos = Point(current_mouse_pos.x, current_mouse_pos.y);

		if (::GetKeyboardState(m_keys_state))
		{
			for (unsigned int i = 0; i < 256; i++)
			{
				allKeyStates[i].pressed = false;
				allKeyStates[i].down = false;
				allKeyStates[i].up = false;

				// KEY IS PRESSED
				if ((m_keys_state[i] & 0x80) && m_keys_state[i] != m_old_keys_state[i])
				{
					allKeyStates[i].pressed = true;
					m_KeyDelayTimer = 0.25f;
				}
				// KEY IS DOWN
				else if ((m_keys_state[i] & 0x80) && m_keys_state[i] == m_old_keys_state[i])
				{
					m_KeyDelayTimer -= TDS::TimeStep::GetDeltaTime();

					if (m_KeyDelayTimer <= 0.0f)
					{
						allKeyStates[i].down = true;
					}
				}
				else // KEY IS UP
				{
					if (m_keys_state[i] != m_old_keys_state[i])
					{
						allKeyStates[i].up = true;
					}
				}

			}
			// store current keys state to old keys state buffer
			::memcpy(m_old_keys_state, m_keys_state, sizeof(unsigned char) * 256);
		}

		//PostMessage(hWnd, WM_SETCURSOR, 0, 0);
		//TDS::InputSystem::GetInstance()->hideMouse();
			
	}

	bool InputSystem::isKeyPressed(int key)
	{
		return allKeyStates[key].pressed;
	}
	bool InputSystem::isKeyDown(int key)
	{
		return allKeyStates[key].down;
	}
	bool InputSystem::isKeyReleased(int key)
	{
		return allKeyStates[key].up;
	}
	bool InputSystem::isMousePressed(int key)
	{
		return allKeyStates[key].pressed;
	}
	bool InputSystem::isMouseDown(int key)
	{
		return allKeyStates[key].down;
	}
	bool InputSystem::isMouseReleased(int key)
	{
		return allKeyStates[key].up;
	}
	bool InputSystem::mouseMoved()
	{
		return m_mouseMoved;
	}
	TDS::Vec2 InputSystem::getLocalMousePos()
	{
		return Vec2(m_localMousePos.x, m_localMousePos.y);
	}
	int InputSystem::getLocalMousePosX()
	{
		return m_localMousePos.x;
	}
	int InputSystem::getLocalMousePosY()
	{
		return m_localMousePos.y;
	}

	void InputSystem::setLocalMousePos(int x, int y)
	{
		m_localMousePos = Point(x, y);
	}

	TDS::Vec2 InputSystem::getGlobalMousePos()
	{
		return TDS::Vec2(m_old_mouse_pos.x, m_old_mouse_pos.y);
	}
	int InputSystem::getGlobalMousePosX()
	{
		return m_old_mouse_pos.x;
	}
	int InputSystem::getGlobalMousePosY()
	{
		return m_old_mouse_pos.y;
	}

	TDS::Vec2 InputSystem::getMouseDelta()
	{
		return TDS::Vec2(m_mouse_delta.x, m_mouse_delta.y);
	}
	int InputSystem::getMouseDeltaX()
	{
		return m_mouse_delta.x;
	}
	int InputSystem::getMouseDeltaY()
	{
		return m_mouse_delta.y;
	}
	bool InputSystem::isMouseScrollDown()
	{
		return m_mouseScrollDown;
	}
	bool InputSystem::isMouseScrollUp()
	{
		return m_mouseScrollUp;
	}

	void InputSystem::processMouseScroll(WPARAM _wParam)
	{
		wheelDelta = GET_WHEEL_DELTA_WPARAM(_wParam);
		if (wheelDelta > 0)
		{
			m_mouseScrollUp = true;
			m_mouseScrollDown = false;
		}
		else if (wheelDelta < 0)
		{
			m_mouseScrollUp = false;
			m_mouseScrollDown = true;
		}
		else
		{
			m_mouseScrollUp = false;
			m_mouseScrollDown = false;
		}
	}

	TDS::Vec2 InputSystem::getRawMouseInput()
	{
		return TDS::Vec2(m_rawMouseInput.x, m_rawMouseInput.y);
	}

	int InputSystem::getRawMouseInputX()
	{
		return m_rawMouseInput.x;
	}

	int InputSystem::getRawMouseInputY()
	{
		return m_rawMouseInput.y;
	}

	int InputSystem::getAxisX()
	{
		//if (m_rawMouseInput.x < 0)
		//{
		//	return -1;
		//}
		//if (m_rawMouseInput.x > 0)
		//{
		//	return 1;
		//}
		//return 0;
		//return Mathf::Clamp(m_rawMouseInput.x, -1, 1);
		return accumulatedMouseX;
	}

	int InputSystem::getAxisY()
	{
		//if (m_rawMouseInput.y < 0)
		//{
		//	return -1;
		//}
		//if (m_rawMouseInput.y > 0)
		//{
		//	return 1;
		//}
		//return 0;
		//return Mathf::Clamp(m_rawMouseInput.y, -1, 1);
		return accumulatedMouseY;
	}

	int InputSystem::getHorizontalAxis()
	{
		if (isKeyPressed('A') || isKeyDown('A'))
		{
			return -1;
		}
		if (isKeyPressed('D') || isKeyDown('D'))
		{
			return 1;
		}
		return 0;
	}

	int InputSystem::getVerticalAxis()
	{
		if (isKeyPressed('W') || isKeyDown('W'))
		{
			return 1;
		}
		if (isKeyPressed('S') || isKeyDown('S'))
		{
			return -1;
		}
		return 0;
	}

	void InputSystem::setRawMouseInput(int x, int y)
	{
		m_rawMouseInput = Point(x, y);
	}

	void InputSystem::setCursorVisible(bool visible)
	{
		mouseVisible = visible;
	}

	void InputSystem::hideMouse()
	{
		if (mouseVisible)
		{
			while (ShowCursor(TRUE) <= 0);
		}
		else
			while (ShowCursor(FALSE) >= 0);
	}

	bool InputSystem::getCursorVisible()
	{
		return mouseVisible;
	}

	void InputSystem::lockMouseCenter(HWND hwnd) 
	{
		POINT centerScreen = { m_winCenter.x, m_winCenter.y };
		ClientToScreen(hwnd, &centerScreen);
		SetCursorPos(centerScreen.x, centerScreen.y);
	}

	void InputSystem::setMouseLock(bool lock) {
		isMouseLocked = lock;
	}

	bool InputSystem::getMouseLock()
	{
		return isMouseLocked;
	}

	TDS::Vec2 InputSystem::getWindowCenter()
	{
		return TDS::Vec2(m_winCenter.x, m_winCenter.y);
	}

	int InputSystem::getWindowCenterX()
	{
		return m_winCenter.x;
	}

	int InputSystem::getWindowCenterY()
	{
		return m_winCenter.y;
	}

	void InputSystem::setWindowCenter(int x, int y)
	{
		m_winCenter = Point(x, y);
	}

	short& InputSystem::getWheelDelta()
	{
		return wheelDelta;
	}

	void InputSystem::setUIMousePos(Vec2 mousePos)
	{
		m_uiMousePos = mousePos;
	}

	float InputSystem::getUIMousePosX()
	{
		return m_uiMousePos.x;
	}

	float InputSystem::getUIMousePosY()
	{
		return m_uiMousePos.y;
	}
}