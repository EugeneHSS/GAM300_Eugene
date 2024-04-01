#pragma once
#include <unordered_set>
#include "dotnet/ImportExport.h"
#include "Timestep/Timestep.h"
#include <Vector2.h>
#include <Windows.h>

namespace TDS
{
	class DLL_API InputSystem
	{
	public:
		struct Point
		{
			int x, y;
		};

		InputSystem();
		~InputSystem();

		static std::unique_ptr<InputSystem>& GetInstance();

		void update();

		bool isKeyPressed(int key);
		bool isKeyDown(int key);
		bool isKeyReleased(int key);
		bool isMousePressed(int key);
		bool isMouseDown(int key);
		bool isMouseReleased(int key);
		bool isMouseScrollUp();
		bool isMouseScrollDown();
		bool mouseMoved();
		TDS::Vec2 getGlobalMousePos();
		int getGlobalMousePosX();
		int getGlobalMousePosY();
		TDS::Vec2 getMouseDelta();
		int getMouseDeltaX();
		int getMouseDeltaY();
		void processMouseScroll(WPARAM _wParam);
		TDS::Vec2 getRawMouseInput();
		int getRawMouseInputX();
		int getRawMouseInputY();
		int getAxisX();
		int getAxisY();
		int getHorizontalAxis();
		int getVerticalAxis();
		void setRawMouseInput(int x, int y);
		void setCursorVisible(bool visible);
		bool getCursorVisible();
		void hideMouse();
		void lockMouseCenter(HWND hwnd);
		void setMouseLock(bool lock);
		bool getMouseLock();
		TDS::Vec2 getWindowCenter();
		int getWindowCenterX();
		int getWindowCenterY();
		void setWindowCenter(int x, int y);
		TDS::Vec2 getLocalMousePos();
		int getLocalMousePosX();
		int getLocalMousePosY();
		void setLocalMousePos(int x, int y);

		short& getWheelDelta();


		static void setUIMousePos(Vec2 mousePos);
		float getUIMousePosX();
		float getUIMousePosY();

		float getSensitivity() { return m_sensitivity; }
		void setSensitivity(float value) { m_sensitivity = value; }

		struct KeyState
		{
			bool pressed;
			bool down;
			bool up;
		};

		int accumulatedMouseX = 0;
		int accumulatedMouseY = 0;

	private:
		static std::unique_ptr<InputSystem> m_instance;
		float m_KeyDelayTimer;
		unsigned char m_keys_state[256] = {};
		unsigned char m_old_keys_state[256] = {};
		KeyState allKeyStates[256];
		Point m_old_mouse_pos;
		bool m_first_time;
		bool m_mouseMoved;
		Point m_mouse_delta;
		short wheelDelta;
		bool m_mouseScrollUp;
		bool m_mouseScrollDown;
		// Gets Raw Mouse Input Acceleration (Relative Mouse Movement) (-ve or +ve intger for xy depending on how fast mouse accelerated)
		Point m_rawMouseInput;
		bool isMouseLocked = false;
		Point m_winCenter;
		bool mouseVisible = false;
		Point m_localMousePos;
		static Vec2 m_uiMousePos;
		float m_sensitivity = 0.1f;
		bool m_previous = mouseVisible;
	};
}
