#pragma once
#include <Windows.h>

#include <cstdint>
#include <string>
#include <queue>

class Window
{
public:
	struct Message
	{
		UINT msg;
		WPARAM wParam;
		LPARAM lParam;
	};
private:
	HWND windowHandler;
	std::wstring className = L"ShutdownScheduler";
	bool isWorking;
	std::queue<Message> messages;

	void createWindowClass(HINSTANCE hInstance);
	void createNativeWindow(std::wstring windowName, uint16_t x, uint16_t y, uint16_t width, uint16_t height, HINSTANCE hInstance);

	static LRESULT CALLBACK ApplicationProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	Window(std::string windowName, uint16_t width, uint16_t height);
	~Window();

	bool getPollEvent(Window::Message& msg);
	bool isOpen() const;

	void close();
	void setVisibility(bool state);

	HWND _getNativeHandler() const;
};