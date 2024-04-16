#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Window.hpp"

class Application {
public:
	static Application& GetInstance();

	void Init(const char* title, int width, int height);
	void Run();
	void Exit();


private:
	std::unique_ptr<Window> m_context;

	Application() {}
	~Application() {}
};

#endif // !APPLICATION_HPP