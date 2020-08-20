#include "Window.h"

void Window::createWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEXW));
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = this->className.c_str();
	wcex.lpfnWndProc = Window::ApplicationProc;
	wcex.hCursor = LoadCursorW(wcex.hInstance, IDC_ARROW);
	wcex.hbrBackground = 0;
	wcex.hIcon = LoadCursorW(wcex.hInstance, IDI_APPLICATION);
	wcex.hIconSm = LoadCursorW(wcex.hInstance, IDI_APPLICATION);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpszMenuName = nullptr;

	if (!RegisterClassExW(&wcex))
		throw new std::exception("Unable to register window class");
}

void Window::createNativeWindow(std::wstring windowName, uint16_t x, uint16_t y, uint16_t width, uint16_t height, HINSTANCE hInstance)
{
	this->windowHandler = CreateWindowExW(NULL, this->className.c_str(), windowName.c_str(), WS_VISIBLE | WS_SYSMENU, x, y, width, height, nullptr, nullptr, hInstance, this);

	if (!this->windowHandler)
		throw new std::exception("Unable to create window");
}

LRESULT Window::ApplicationProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* win = nullptr;
	if (msg == WM_NCCREATE)
	{
		win = static_cast<Window*>(reinterpret_cast<LPCREATESTRUCTW>(lParam)->lpCreateParams);

		if (SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(win)))
			if (GetLastError())
				ExitProcess(1);
	}
	else
	{
		win = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	}

	if (win)
	{
		win->windowHandler = hWnd;

		Window::Message message;
		message.msg = msg;
		message.wParam = wParam;
		message.lParam = lParam;
		win->messages.push(message);
	}

	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

Window::Window(std::string windowName, uint16_t width, uint16_t height)
{
	HINSTANCE hInstance = GetModuleHandleW(nullptr);
	uint16_t x = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	uint16_t y = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	std::wstring _windowName(windowName.begin(), windowName.end());
	this->isWorking = true;

	try
	{
		this->createWindowClass(hInstance);
		this->createNativeWindow(_windowName, x, y, width, height, hInstance);
	}
	catch (std::exception& e)
	{
		MessageBoxA(NULL, "Some error occured", e.what(), MB_ICONERROR);
		delete& e;
	}
}

Window::~Window()
{
	UnregisterClassW(this->className.c_str(), GetModuleHandleW(nullptr));
	this->isWorking = false;
	DestroyWindow(this->windowHandler);
}

bool Window::getPollEvent(Window::Message& msg)
{
	if (this->messages.empty())
	{
		MSG msg;
		while (PeekMessageW(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	if (!this->messages.empty())
	{
		msg = this->messages.front();
		this->messages.pop();

		return true;
	}

	return false;
}

bool Window::isOpen() const
{
	return this->isWorking;
}

void Window::close()
{
	this->~Window();
}

void Window::setVisibility(bool state)
{
	ShowWindow(this->windowHandler, state);
}

HWND Window::_getNativeHandler() const
{
	return this->windowHandler;
}
