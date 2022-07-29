#pragma once

#include "Input.h"
#include "Window.h"

#include "Renderer/Context.h"

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

		// returns the application's input
		inline const SharedPointer<Input>& GetInput() const { return m_Input; }

		// returns the application's window
		inline const SharedPointer<Window>& GetWindow() const { return m_Window; }

		// returns the renderer context
		inline const SharedPointer<Renderer::Context>& GetRenderer() { return m_RendererContext; }

	public:

		// main loop
		void Run();

	private:

		SharedPointer<Input> m_Input;
		SharedPointer<Window> m_Window;
		SharedPointer<Renderer::Context> m_RendererContext;

	};
}