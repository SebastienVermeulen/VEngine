#pragma once
#include "pch.h"

class FileManager abstract final
{
public:
	/// <summary>
	/// Static function allows for the retrieval of the full path of the currently running executable.
	/// </summary>
	/// <returns>Full path of the currently running executable. (Without the 'name.exe')</returns>
	static inline std::wstring GetAbsoluteExePath()
	{
		if(m_ExeFullPath.length())
		{
			return m_ExeFullPath;
		}

		WCHAR fullPath[MAX_PATH];
		if (!GetModuleFileNameW(NULL, fullPath, sizeof(fullPath) / 2))
		{
			return L"";
		}

		std::wstring str(fullPath);
		std::wregex reg(L"(.+\\\\).+\\.exe$");
		std::wsmatch matches;
		std::regex_match(str, matches, reg);

		std::match_results results = std::match_results(matches);
		if (!results.size()) 
		{
			return L"";
		}

		m_ExeFullPath = results[1].str();
		return m_ExeFullPath;
	}

private:
	static std::wstring m_ExeFullPath;
};

std::wstring FileManager::m_ExeFullPath = L"";