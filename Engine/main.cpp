#include <crtdbg.h> // To check for memory leaks
#include "Application.hpp"

#if defined(NDEBUG) && defined(_WIN32)
#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
#endif

int main() {

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Application &app = Application::GetInstance();
	app.Init();
	app.Run();
	app.Exit();
}