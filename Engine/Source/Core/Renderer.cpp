#include "enpch.h"
#include "Renderer.h"

namespace Engine
{
	Renderer::Renderer(Window& window)
		: m_Window(window)
	{
		ENGINE_TRACE("Creating Renderer");

		m_GraphicsContext = GraphicsContext::CreateGraphicsContext(m_Window);
	}

	Renderer::~Renderer()
	{

	}

	void Renderer::Update()
	{

	}
}