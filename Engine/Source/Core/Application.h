#pragma once

#include "Window.h"

namespace Engine
{
	class Application
	{

	public:

		// constructor
		Application();

		// destructor
		~Application();

		// main loop
		void Run();

	private:

		Window m_Window{ "Cosmos Engine", 800, 600 };

	};
}