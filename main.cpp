#pragma comment(linker,  "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR nCmdLine, int nCmdShow)
{
	Application app;
	app.run();
}