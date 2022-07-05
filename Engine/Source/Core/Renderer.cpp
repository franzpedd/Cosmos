#include "enpch.h"
#include "Renderer.h"

namespace Engine
{
	Renderer::Renderer(Window& windowref)
		: m_WindowRef(windowref)
	{
		ENGINE_TRACE("Creating Renderer");

		m_GraphicsContext = GraphicsContext::CreateGraphicsContext(m_WindowRef);
	}

	Renderer::~Renderer()
	{

	}

	void Renderer::Update()
	{

	}
}