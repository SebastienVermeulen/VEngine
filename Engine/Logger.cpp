#include "pch.h"
#include "Logger.h"

void Logger::Log(const LogVerbosity verbosity, const std::wstring& log)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, (WORD)verbosity); 

	std::wstring warningText = L"";
	switch (verbosity)
	{
	case LogVerbosity::Log:
		warningText = L"Log: ";
		break;
	case LogVerbosity::Warning:
		warningText = L"Warning: ";
		break;
	case LogVerbosity::Error:
		warningText = L"Error: ";
		break;
	case LogVerbosity::Fatal:
		warningText = L"Fatal: ";
		break;
	case LogVerbosity::Core:
		warningText = L"Core: ";
		break;
	}

	OutputDebugStringW((warningText + log).c_str());
	OutputDebugStringW(L"\n");
}
