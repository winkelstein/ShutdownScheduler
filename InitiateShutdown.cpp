#include "Application.h"
#include <sstream>
#include <thread>

void Application::onButtonClicked()
{
	uint16_t item = SendMessageW(this->comboBox, CB_GETCURSEL, NULL, NULL);

	uint32_t time = pow(60, item);

	std::wstring str;
	str.resize(32);
	GetWindowTextW(this->editBox, &str[0], 32);
	str.erase(str.find(L'\0'));

	if (str.empty())
		MessageBoxW(this->win->_getNativeHandler(), L"Input field is empty", L"Enter valid time", MB_ICONWARNING);
	else
	{
		std::wstringstream wss;
		wss << str;

		uint32_t delay;
		wss >> delay;

		this->win->setVisibility(false);

		std::thread thr([&delay, &time](uint32_t time) {
			std::this_thread::sleep_for(std::chrono::seconds(time));
			InitiateSystemShutdownW(NULL, NULL, 0, TRUE, FALSE);
			}, time*delay);

		thr.detach();
	}
}