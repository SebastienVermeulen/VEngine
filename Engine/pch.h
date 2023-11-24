//Written by Sebastien Vermeulen | 14/04/2021 - Present
//VEngine - DirectX rasterisation framework

#pragma once
#pragma region PrecompilerStatements
#if !defined(DEBUG_APP) && (defined(DEBUG) || defined(_DEBUG))
		#define DEBUG_APP
#endif

#ifndef BUILD_APP
	// TODO: In the future make a clean and proper way to change this
	#ifndef DEBUG_APP
		#undef BUILD_APP
		#define BUILD_APP 0
	#endif
#endif
#pragma endregion

#pragma region Windows
//Include the basic windows header file  and the Direct3D header files
#include <windows.h>
#include <windowsx.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <chrono>
#include <regex>
#include <locale>
#include <codecvt>
#include <string>
#include <cstddef>
#include <concepts>
#pragma endregion

#pragma region DirectX
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")

#pragma warning (push, 0) //Ignore all warnings
#include <directxmath.h>
#pragma warning (pop)

//Helps with effects 11
#pragma warning (push, 0) //Ignore all warnings
#include <d3dcompiler.h>
#pragma warning (pop)
#pragma comment(lib, "d3dcompiler.lib")

#pragma warning (push, 0) //Ignore all warnings
#include <d3dcommon.h>
#pragma warning (pop)

//https://learn.microsoft.com/en-us/windows/win32/api/d3dcompiler/nf-d3dcompiler-d3dcompile
#define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)
#pragma endregion

#pragma region ThirdParty
//Include the FBX SDK
#pragma warning (push, 0) //Ignore all warnings, be specific due to high warning level
#pragma warning (disable : 26495)
#pragma warning (disable : 26812)
#pragma warning (disable : 26451)
#include <fbxsdk.h>
#pragma comment (lib, "libfbxsdk.lib")
#pragma warning (pop)

//Effects 11 (Helper for loading Effects (D3DX11))
//https://fx11.codeplex.com/
#pragma warning (push, 0) //Ignore all warnings
#include "d3dx11effect.h"
#pragma warning (pop)
#ifdef DEBUG_APP
	#pragma comment(lib, "Effects11d.lib")
#else 
	#pragma comment(lib, "Effects11.lib")
#endif

//Commented as it caused issues on other systems, uncomment might cause errors
//#ifdef DEBUG_APP
//	#pragma warning (push, 0) //Ignore all warnings
//	#include <vld.h>
//	#pragma warning (pop)
//#endif

#pragma warning (push, 0) //Ignore all warnings
#include <DDSTextureLoader.h>
#pragma warning (pop)

#include "../AdditionalDependencies/Includes/ImGUI/Include/imgui.h"
#include "../AdditionalDependencies/Includes/ImGUI/Include/imgui_impl_win32.h"
#include "../AdditionalDependencies/Includes/ImGUI/Include/imgui_impl_dx11.h"
#pragma endregion

#pragma region Deletion/Release
template<class Instance>
inline static void SafeRelease(Instance& interfaceToRelease)
{
	if (interfaceToRelease != nullptr)
	{
		interfaceToRelease->Release();
		interfaceToRelease = nullptr;
	}
}
template<class Instance>
inline static void SafeRelease(std::vector<Instance>& interfaceToRelease)
{
	for (int i = 0; i < interfaceToRelease.size(); ++i)
	{
		if (interfaceToRelease[i] != nullptr)
		{
			interfaceToRelease[i]->Release();
			interfaceToRelease[i] = nullptr;
		}
	}
	interfaceToRelease.clear();
}
template<typename Key, class Instance>
inline static void SafeRelease(std::unordered_map<Key, Instance>& interfaceToRelease)
{
	if (interfaceToRelease.empty())
	{
		return;
	}

	for (Instance* pToRelease : interfaceToRelease)
	{
		SafeRelease(pToRelease);
	}
	interfaceToRelease.clear();
}
template<class T>
inline static void SafeDelete(T& objectToDelete)
{
	if (objectToDelete != nullptr)
	{
		delete(objectToDelete);
		objectToDelete = nullptr;
	}
}
template<class T>
inline static void SafeDelete(std::vector<T>& objectToDelete)
{
	for (int i = 0; i < objectToDelete.size(); ++i)
	{
		if (objectToDelete[i] != nullptr)
		{
			delete(objectToDelete[i]);
			objectToDelete[i] = nullptr;
		}
	}
	objectToDelete.clear();
}
template<typename Key, class T>
inline static void SafeDelete(std::unordered_map<Key, T>& objectToDelete)
{
	if (objectToDelete.empty())
	{
		return;
	}

	for (T* pToRelease : objectToDelete)
	{
		delete(pToRelease);
		pToRelease = nullptr;
	}
	objectToDelete.clear();
}
#pragma endregion

#pragma region WCharConversions
//Source https://www.py4u.net/discuss/64164
//Convert a wide Unicode string to an UTF8 string
inline static std::string UTF8Encode(const std::wstring& wstr)
{
	if (wstr.empty())
	{
		return std::string();
	}
	size_t sizeNeeded = (size_t)WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string newString(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &newString[0], (int)sizeNeeded, NULL, NULL);
	return newString;
}
inline static std::string UTF8Encode(const wchar_t wstr[])
{
	size_t wcsChars = wcslen(wstr);
	if (wcsChars == 0)
	{
		return std::string();
	}
	size_t sizeNeeded = (size_t)WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	std::string newString(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr, (int)wcsChars, &newString[0], (int)sizeNeeded, NULL, NULL);
	return newString;
}
//Convert an UTF8 string to a wide Unicode String
inline static std::wstring UTF8Decode(const std::string& str)
{
	if (str.empty())
	{
		return std::wstring();
	}
	size_t sizeNeeded = (size_t)MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring newWstring(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &newWstring[0], (int)sizeNeeded);
	return newWstring;
}
inline static std::wstring UTF8Decode(const char str[])
{
	size_t strChars = strlen(str);
	if (strChars == 0)
	{
		return std::wstring();
	}
	size_t sizeNeeded = (size_t)MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	std::wstring newString(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, str, (int)strChars, &newString[0], (int)sizeNeeded);
	return newString;
}
#pragma endregion

#pragma region WCharConversionMacros
#include "Logger.h"

//String concat unspecified number of params
#define V_TEXT(wideText, ...) UTF8Encode(wideText)
#define V_WTEXT(text, ...) UTF8Decode(text)

#if defined(BUILD_APP) && BUILD_APP 
	#define V_LOG(verbosity, log, ...) ;
#else
	#define V_LOG(verbosity, log, ...) Logger::Log(verbosity, log);
#endif
#pragma endregion

#pragma region Math/Types
#include "VMath.h"
#pragma endregion
