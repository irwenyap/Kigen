#include <crtdbg.h> // To check for memory leaks
#include "Application.hpp"

int main() {

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Application::GetInstance().Init("KigenEngine", 1920, 1080);
	Application::GetInstance().Run();
	Application::GetInstance().Exit();
}
