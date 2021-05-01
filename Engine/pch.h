//Written by Sebastien Vermeulen | 14/04/2021
//VEngine - DirectX rasterisation framework

#pragma once
#define DEBUG

#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>

// include the basic windows header file  and the Direct3D header files
#include <windows.h>
#include <windowsx.h>
#pragma warning (push, 0) //Ignore all warnings from directX
#pragma warning (disable : 26812)
#pragma warning (disable : 26495)
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#pragma warning (pop)

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#ifdef DEBUG
#include <vld.h>
#endif

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
	for (int i = 0; i < interfaceToRelease.size(); i++)
	{
		if (interfaceToRelease[i] != nullptr)
		{
			interfaceToRelease[i]->Release();
			interfaceToRelease[i] = nullptr;
		}
	}
	interfaceToRelease.empty();
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
	for (int i = 0; i < objectToDelete.size(); i++)
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
