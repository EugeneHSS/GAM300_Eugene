#include "windowswindow.h"
#include <iostream>

namespace TDS
{
	WindowsWin::WindowsWin(HINSTANCE hInstance, int nCmdShow, const wchar_t* className)
		:m_handleWindows{ nullptr }, m_hInstance{ hInstance }, m_Width{ 0 }, m_Height{ 0 },
		m_classname{ className }, m_cmdshow{ nCmdShow }
	{
	}

	WindowsWin::~WindowsWin()
	{
		if (m_handleWindows != nullptr)
		{
			DestroyWindow(m_handleWindows);
		}
	}

	uint32_t WindowsWin::getWidth()  const noexcept
	{
		return static_cast<uint32_t>(m_Width);
	}

	uint32_t WindowsWin::getHeight() const noexcept
	{
		return static_cast<uint32_t>(m_Height);
	}

	bool WindowsWin::registerWindow(const WNDPROC& wndproc)
	{
		if (WNDCLASS checkClass{}; GetClassInfo(m_hInstance, L"TDSWindowClass", &checkClass))
		{
			return true;// class has been registered before
		}
		WNDCLASSEX windowclass{};
		windowclass.cbSize = sizeof(decltype(windowclass));
		windowclass.style = CS_HREDRAW | CS_VREDRAW;
		windowclass.lpfnWndProc = wndproc;
		windowclass.cbClsExtra = 0;
		windowclass.cbWndExtra = 0;
		windowclass.hInstance = m_hInstance;
		windowclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		windowclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		windowclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
		windowclass.lpszMenuName = 0;
		windowclass.lpszClassName = m_classname.data();

		return RegisterClassEx(&windowclass) ? true : false;
	}

	bool WindowsWin::createWindow(const WNDPROC& wndproc, int _width, int _height)
	{
		if (false == registerWindow(wndproc))
		{
			std::cerr << "Failed to register window class\n";
			return false;
		}

		if (m_handleWindows != nullptr) //means already exist
			return true;

		const int screenWidth{ GetSystemMetrics(SM_CXSCREEN) };
		const int screenHeight{ GetSystemMetrics(SM_CYSCREEN) };

		m_Width = _width;
		m_Height = _height;

		RECT windowRect;
		SetRect(&windowRect, static_cast<decltype(windowRect.left)>(screenWidth / 2 - m_Width / 2),
			static_cast<decltype(windowRect.left)>(screenHeight / 2 - m_Height / 2),
			static_cast<decltype(windowRect.left)>(screenWidth / 2 + m_Width / 2),
			static_cast<decltype(windowRect.left)>(screenHeight / 2 + m_Height / 2));

		DWORD style = WS_OVERLAPPEDWINDOW; // WS_THICKFRAME to resize will include it next time

		AdjustWindowRectEx(&windowRect, style, FALSE, 0);

		m_handleWindows = CreateWindowEx
		(
			0,
			m_classname.data(),
			L"TEAR DROP STUDIO",
			style,
			windowRect.left,
			windowRect.top,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			NULL,
			NULL,
			m_hInstance,
			NULL
		);

		if (m_handleWindows == nullptr)
			return false;

		ShowWindow(m_handleWindows, SW_SHOW);
		UpdateWindow(m_handleWindows);

		return true;

	}

	bool WindowsWin::processInputEvent()
	{
		for (MSG msg; PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE); TranslateMessage(&msg), DispatchMessage(&msg))
		{
			if (msg.message == WM_QUIT)
				return false;
		

		}
		return true;
	}
	DLL_API bool WindowsWin::GetWindowPos(int& xPos, int& yPos) const
	{
		RECT windowRect;
		if (GetWindowRect(m_handleWindows, &windowRect)) {
			xPos = windowRect.left;
			yPos = windowRect.top;
			return true;
		}

		return false;
	}
}//namespace TDS
