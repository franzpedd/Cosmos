#include "enpch.h"
#include "Application.h"

namespace Engine
{
	Application::Application()
	{
		ENGINE_TRACE("Creating Application");

		m_Window = Window::Create("Cosmos Application", 800, 600);
		m_Input = Input::Create(m_Window);
		m_RendererContext = Renderer::Context::Create(m_Window);
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (!m_Window->ShouldClose())
		{
			m_Input->Update();
			m_RendererContext->Update();
			m_Window->Update();
		}

		m_RendererContext->Finish();
	}
}