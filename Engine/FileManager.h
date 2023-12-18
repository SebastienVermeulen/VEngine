#pragma once
#include "pch.h"

class Texture;
class MeshAsset;

//TO-DO: save loaded files in managers so they only have to be loaded once
// Use only as an interface to extract data from files
class FileManager abstract final
{
public:
	/// <summary>
	/// Static function allows for the retrieval of the absolute path of the folder a file is located in.
	/// </summary>
	/// <returns>Absolute path of the folder a file is located in</returns>
	static std::wstring GetAbsoluteFolderPathofFile(const std::wstring& localFilePath);
	/// <summary>
	/// Static function allows for the retrieval of the local path of the folder a file is located in..
	/// </summary>
	/// <returns>Local path of the folder a file is located in</returns>
	static std::wstring GetLocalFolderPathofFile(const std::wstring& localFilePath);
	/// <summary>
	/// Static function allows for the retrieval of the Absolute path of the currently running executable.
	/// </summary>
	/// <returns>Absolute path of the currently running executable. (Without the 'name.exe')</returns>
	static std::wstring GetAbsoluteExePath();
	static std::wstring GetFileName(const std::wstring& localFilePath);
	static std::wstring GetFullFilePath(const std::wstring& localFilePath);

#pragma region Meshes
public:
	// TO-DO: Extend the .OBJ format: https://devtalk.blender.org/t/extended-obj-format-with-skeletal-animation-info/447
	// Other file formats are just too cumbursome to even bother with ngl

	/// <summary>
	/// Loads an obj and places the data into the given buffers.
	/// </summary>
	static HRESULT LoadOBJ(std::wstring localFileDir, MeshAsset* pMeshAsset);
	static HRESULT OBJParseBasicData(std::string& objFileContent, int& outVertexCount, int& outFaceCount);
	static HRESULT OBJParseVertexData(std::string& objFileContent, const int vertexCount,
		std::vector<DirectX::XMFLOAT3>& positions,
		std::vector<std::vector<DirectX::XMFLOAT2>>& uvs,
		std::vector<DirectX::XMFLOAT3>& normals);
	static HRESULT OBJParseIndices(std::string& objFileContent, const int faceCount, std::vector<DirectX::XMINT3>& indices);
private:

#pragma endregion

#pragma region Textures
public:
	/// <summary>
	/// Loads an dds file from a local filepath into a texture.
	/// </summary>
	static Texture* LoadDDS(ID3D11Device* pDevice, std::wstring localFileDir);
#pragma endregion
};
