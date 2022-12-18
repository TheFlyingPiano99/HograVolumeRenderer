#pragma once
#include "GlobalInclude.h"
#include <string>

namespace Hogra {
	class HoGraEngineApplication
	{
	public:
		HoGraEngineApplication() = default;

		int Init(const char* windowName, int argc = 0, char* argv[] = nullptr);

		void Loop();

		void Destroy();

	private:

		void setFullScreenMode(GLFWwindow*& window, bool isFullScreenMode);

		void SetIcon();

		void PrintWelcomeConsoleText();

		GLFWwindow* window = nullptr;
		std::string windowName;
	};
}

