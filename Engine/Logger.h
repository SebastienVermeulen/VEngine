#pragma once
#include "Singleton.h"

// ANSI escape color codes
enum class LogVerbosity : UINT8
{
	Log = 37,
	Warning = 33,
	Error = 91,
	Fatal = 31,
	Core = 97,
};

class Logger final
{
public:
	static void Log(const LogVerbosity verbosity, const std::wstring& log);
};