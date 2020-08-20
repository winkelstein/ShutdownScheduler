#pragma once
#include "Window.h"

class Application
{
private:
	HWND comboBox;
	HWND editBox;
	HWND timerButton;
private:
	Window* win;

	void ApplicationProc(Window::Message& msg);

	void initControls();
	void adjustProcessPrivileges();
	void onCommand(HMENU hMenu, HWND hID);
	void onButtonClicked();
public:
	Application();
	~Application();

	void run();
};

