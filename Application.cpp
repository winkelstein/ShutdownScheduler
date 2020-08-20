#include "Application.h"

void Application::initControls()
{
	HINSTANCE hInstance = GetModuleHandleW(nullptr);
	HWND parent = this->win->_getNativeHandler();

	this->timerButton = CreateWindowExW(NULL, L"BUTTON", L"Start timer", BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, 150, 340, 100, 40, parent, nullptr, hInstance, nullptr);
	this->comboBox = CreateWindowExW(NULL, L"ComboBox", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST, 230, 260, 100, 22, parent, nullptr, hInstance, nullptr);
	this->editBox = CreateWindowExW(NULL, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, 75, 260, 150, 24, parent, nullptr, hInstance, nullptr);
	
	HFONT hFont = CreateFontW(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, 0, 0, DEFAULT_QUALITY, FF_SWISS, L"Roboto");
	SendMessageW(timerButton, WM_SETFONT, TRUE, (LPARAM)hFont);
	SendMessageW(comboBox, WM_SETFONT, TRUE, (LPARAM)hFont);

	SendMessageW(comboBox, CB_ADDSTRING, 1, (LPARAM)L"seconds");
	SendMessageW(comboBox, CB_ADDSTRING, 2, (LPARAM)L"minutes");
	SendMessageW(comboBox, CB_ADDSTRING, 3, (LPARAM)L"hours");
	SendMessageW(comboBox, CB_SETCURSEL, 0, 0L);
}

void Application::adjustProcessPrivileges()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		throw new std::exception("Unable to change process privileges");

	LookupPrivilegeValueW(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, nullptr, 0);

	if (GetLastError() != ERROR_SUCCESS)
		throw new std::exception("Unable to change process privileges");
}

void Application::onCommand(HMENU hMenu, HWND hID)
{
	if (hID == this->timerButton)
		this->onButtonClicked();
}

void Application::ApplicationProc(Window::Message& msg)
{
	switch (msg.msg)
	{
	case WM_CREATE:
		this->initControls();
		try
		{
			this->adjustProcessPrivileges();
		}
		catch (std::exception& e)
		{
			MessageBoxW(this->win->_getNativeHandler(), (wchar_t*)e.what(), L"Some error occured", MB_ICONERROR);
			this->~Application();
			ExitThread(1);
		}

		break;
	case WM_COMMAND:
		this->onCommand((HMENU)msg.wParam, (HWND)msg.lParam);
		break;
	case WM_PAINT:
	{
		static PAINTSTRUCT ps;
		HDC hDC = GetWindowDC(this->win->_getNativeHandler());
		HFONT hFont = CreateFontW(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, 0, 0, DEFAULT_QUALITY, FF_SWISS, L"Roboto");

		SelectObject(hDC, hFont);
		TextOutW(hDC, 75, 120, L"Shutdown computer in", 21);
	}
	break;
	case WM_DESTROY:
		this->win->close();
		break;
	}
}

Application::Application()
{
	HANDLE hMutex = CreateMutexW(nullptr, TRUE, L"ShutdownSchedulerMutex");

	if (!hMutex || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND existingWindow = FindWindowW(L"ShutdownScheduler", L"Shutdown Scheduler");

		if (existingWindow)
		{
			ShowWindow(existingWindow, true);
			SetForegroundWindow(existingWindow);

			this->~Application();
			ExitThread(0);
		}
	}

	this->win = new Window("Shutdown Scheduler", 400, 600);
}

Application::~Application()
{
	delete this->win;
}

void Application::run()
{
	Window::Message msg;

	while (this->win->isOpen())
	{
		while (this->win->getPollEvent(msg))
		{
			this->ApplicationProc(msg);
		}
	}
}
