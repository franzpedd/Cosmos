#include "enpch.h"
#include "Logger.h"

namespace Engine
{
	void Logger::ToConsole(const char* project, Severity severity, const char* file, uint32_t line, const char* message, ...)
	{
		printf("[%s:%s]", project, SeverityToConstChar(severity));

		if (severity >= Severity::WARN)
		{
			printf("[Line %d:File %s]", line, file);
		}

		char buffer[LOG_MAX_SIZE];
		va_list args;
		va_start(args, message);
		vsnprintf(buffer, LOG_MAX_SIZE, message, args);
		va_end(args);

		printf(" %s\n", buffer);
	}

	const char* Logger::SeverityToConstChar(Severity severity)
	{
		switch (severity)
		{
		case Severity::TRACE: return "Trace";
		case Severity::INFO: return "Info";
		case Severity::WARN: return "Warn";
		case Severity::ERROR: return "Error";
		case Severity::ASSERTION: return "Assertion";
		}
		return "Unknown";
	}
}