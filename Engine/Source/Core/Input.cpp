#include "enpch.h"
#include "Input.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	Input::Input(Window& windowref)
		: m_WindowRef(windowref)
	{
		ENGINE_TRACE("Creating Input");
	}

	Input::~Input()
	{

	}

	void Input::Update()
	{

	}

	uint32_t Input::IsKeyboardKeyDown(uint32_t keycode)
	{
		return (uint32_t)glfwGetKey(m_WindowRef.GetNativeWindow(), keycode);
	}

	uint32_t Input::IsMouseButtonDown(uint32_t buttoncode)
	{
		return (uint32_t)glfwGetMouseButton(m_WindowRef.GetNativeWindow(), buttoncode);
	}

	std::pair<double, double> Input::GetCurrentMousePosition()
	{
		std::pair<double, double> pos;
		glfwGetCursorPos(m_WindowRef.GetNativeWindow(), &pos.first, &pos.second);

		return pos;
	}
}