#include "enpch.h"
#include "Input.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	SharedPointer<Input> Input::Create(SharedPointer<Window>& window)
	{
		return CreateSharedPointer<Input>(window);
	}

	Input::Input(SharedPointer<Window>& window)
		: m_Window(window)
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
		return (uint32_t)glfwGetKey(m_Window->GetNativeWindow(), keycode);
	}

	uint32_t Input::IsMouseButtonDown(uint32_t buttoncode)
	{
		return (uint32_t)glfwGetMouseButton(m_Window->GetNativeWindow(), buttoncode);
	}

	std::pair<double, double> Input::GetCurrentMousePosition()
	{
		std::pair<double, double> pos;
		glfwGetCursorPos(m_Window->GetNativeWindow(), &pos.first, &pos.second);

		return pos;
	}
}