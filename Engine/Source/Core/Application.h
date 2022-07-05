#pragma once

#include "Input.h"
#include "Window.h"
#include "Renderer.h"

namespace Engine
{
	class Application
	{
	public:

		// constructor
		Application();

		// destructor
		~Application();

	public:

		// returns a reference to the application's input
		inline const Input& GetInput() const { return m_Input; }

		// returns a reference to the application's window
		inline const Window& GetWindow() const { return m_Window; }

		//returns a reference to the application's renderer
		inline const Renderer& GetRenderer() const { return m_Renderer; }

	public:

		// main loop
		void Run();

	private:

		Input m_Input;
		Window m_Window;
		Renderer m_Renderer;

	};
}