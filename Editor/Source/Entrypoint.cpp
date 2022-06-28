#include <Engine.h>

int main(int argc, char* argv[])
{
	Engine::Application* app = new Engine::Application();
	app->Run();
	delete app;

	return 0;
}