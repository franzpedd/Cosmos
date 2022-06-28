#pragma once

#include <cstdint>

#define LOG_MAX_SIZE 1024

namespace Engine
{
	class Logger
	{
	public:

		enum class Severity
		{
			TRACE = 0,
			INFO,
			WARN,
			ERROR,
			ASSERTION
		};

	public:

		// sends a message to the console
		static void ToConsole(const char* project, Severity severity, const char* file, uint32_t line, const char* message, ...);

	protected:

		// returns the severity as text
		static const char* SeverityToConstChar(Severity severity);

	};
}

// always enable assertions
#define ENGINE_ASSERT(x, ...) { if(!(x)) { ::Engine::Logger::ToConsole("Engine", Engine::Logger::Severity::ASSERTION, __FILE__, __LINE__, __VA_ARGS__); abort();}}
#define EDITOR_ASSERT(x, ...) { if(!(x)) { ::Engine::Logger::ToConsole("Editor", Engine::Logger::Severity::ASSERTION, __FILE__, __LINE__, __VA_ARGS__); abort();}}

// always enable errors and warnings
#define ENGINE_ERROR(...)	::Engine::Logger::ToConsole("Engine", Engine::Logger::Severity::ERROR, __FILE__, __LINE__, __VA_ARGS__);
#define ENGINE_WARN(...)	::Engine::Logger::ToConsole("Engine", Engine::Logger::Severity::WARN, __FILE__, __LINE__, __VA_ARGS__);
#define EDITOR_ERROR(...)	::Engine::Logger::ToConsole("Editor", Engine::Logger::Severity::ERROR, __FILE__, __LINE__, __VA_ARGS__);
#define EDITOR_WARN(...)	::Engine::Logger::ToConsole("Editor", Engine::Logger::Severity::WARN, __FILE__, __LINE__, __VA_ARGS__);

// tracing and info are only enabled on debug version
#if defined(ENGINE_DEBUG)
#define ENGINE_TRACE(...)	::Engine::Logger::ToConsole("Engine", Engine::Logger::Severity::TRACE, __FILE__, __LINE__, __VA_ARGS__);
#define ENGINE_INFO(...)	::Engine::Logger::ToConsole("Engine", Engine::Logger::Severity::INFO, __FILE__, __LINE__, __VA_ARGS__);
#define EDITOR_TRACE(...)	::Engine::Logger::ToConsole("Editor", Engine::Logger::Severity::TRACE, __FILE__, __LINE__, __VA_ARGS__);
#define EDITOR_INFO(...)	::Engine::Logger::ToConsole("Editor", Engine::Logger::Severity::INFO, __FILE__, __LINE__, __VA_ARGS__);
#else
#define ENGINE_TRACE(...)
#define ENGINE_INFO(...)
#define EDITOR_TRACE(...)
#define EDITOR_INFO(...)
#endif