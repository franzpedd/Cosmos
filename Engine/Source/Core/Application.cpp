#include "enpch.h"
#include "Application.h"

namespace Engine
{
	Application::Application()
		: m_Window("Cosmos Application", 800, 600), m_Renderer(m_Window), m_Input(m_Window)
	{
		ENGINE_TRACE("Creating Application");
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (!m_Window.ShouldClose())
		{
			m_Input.Update();
			m_Window.Update();
			m_Renderer.Update();
		}
	}
}