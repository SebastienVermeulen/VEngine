//Written by Sebastien Vermeulen | 14/04/2021 - Present
//VEngine - DirectX rasterisation framework

#pragma once
#pragma region Windows
//Include the basic windows header file  and the Direct3D header files
#include <windows.h>
#include <windowsx.h>

#include <iostream>
#include <fstream>
#include <vector>
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

//TO-DO: Make own temp types that takes care of this before storing to buffers etc.
inline static bool operator==(const DirectX::XMFLOAT3& self, const DirectX::XMFLOAT3& other)
{
	return self.x == other.x && self.y == other.y && self.z == other.z;
}
inline static bool operator==(const DirectX::XMFLOAT2& self, const DirectX::XMFLOAT2& other)
{
	return self.x == other.x && self.y == other.y;
}
//END-TO-DO
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
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "Effects11d.lib")
#else 
#pragma comment(lib, "Effects11.lib")
#endif

//Commented as it caused issues on other systems, uncomment might cause errors
//#ifdef _DEBUG
//#pragma warning (push, 0) //Ignore all warnings
//#include <vld.h>
//#pragma warning (pop)
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
	int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string newString(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &newString[0], sizeNeeded, NULL, NULL);
	return newString;
}
//Convert an UTF8 string to a wide Unicode String
inline static std::wstring UTF8Decode(const std::string& str)
{
	if (str.empty())
	{
		return std::wstring();
	}
	int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring newWstring(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &newWstring[0], sizeNeeded);
	return newWstring;
}
#pragma endregion
