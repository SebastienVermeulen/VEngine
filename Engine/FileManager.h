#pragma once
#include "pch.h"

class Texture;
class MeshAsset;

//TO-DO: save loaded files in managers so they only have to be loaded once
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
	static std::wstring GetFullFileName(const std::wstring& localFilePath);

#pragma region Meshes
public:
	/// <summary>
	/// Loads an fbx and places the data into the given buffers.
	/// </summary>
	static HRESULT LoadFBX(std::wstring localFileDir, MeshAsset* pMeshAsset);

private:
	static void ReadAllChildNodesFBX(fbxsdk::FbxNode* pNode, MeshAsset* pMeshAsset);
	static void ReadMeshFBX(fbxsdk::FbxMesh* pMesh, MeshAsset* pMeshAsset);

	static DirectX::XMFLOAT2 GetUVFBX(const int polygonNr, const int vertexNr, fbxsdk::FbxMesh* pMesh, fbxsdk::FbxStringList& list);
	static DirectX::XMFLOAT3 GetColorFBX(const fbxsdk::FbxGeometryElementVertexColor* pColor, const int polygonNr, const int vertexNr, 
		const int vertexIdx, const bool colorUseIndex, const bool colorsMappingByControl);
	static DirectX::XMFLOAT3 GetNormalFBX(const fbxsdk::FbxGeometryElementNormal* pNormal, const int polygonNr, const int vertexNr, 
		const int vertexIdx, const bool normalUseIndex, const bool normalsMappingByControl);
	static DirectX::XMFLOAT3 GetTangentsFBX(const fbxsdk::FbxGeometryElementTangent* pTangent, const int polygonNr, const int vertexNr, 
		const int vertexIdx, const bool tangentsUseIndex, const bool tangentsMappingByControl);
	static DirectX::XMFLOAT3 GetBinormalFBX(const fbxsdk::FbxGeometryElementBinormal* pBinormal, const int polygonNr, const int vertexNr, 
		const int vertexIdx, const bool binormalUseIndex, const bool binormalsMappingByControl);
#pragma endregion

#pragma region Textures
public:
	/// <summary>
	/// Loads an dds file from a local filepath into a texture.
	/// </summary>
	static Texture* LoadDDS(ID3D11Device* pDevice, std::wstring localFileDir);
#pragma endregion
};
