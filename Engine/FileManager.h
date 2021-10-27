#pragma once
#include "pch.h"

struct VertexPNTU;
class Texture;

//TO-DO: save loaded files in managers so they only have to be loaded once
class FileManager abstract final
{
public:
	/// <summary>
	/// Static function allows for the retrieval of the full path of the currently running executable.
	/// </summary>
	/// <returns>Full path of the currently running executable. (Without the 'name.exe')</returns>
	static std::wstring GetAbsoluteExePath();

	/// <summary>
	/// Loads an fbx and places the data into the given buffers.
	/// </summary>
	static HRESULT LoadFBX(std::vector<VertexPNTU>& vertices, std::vector<unsigned int>& indices, std::wstring localFileDir);

	static Texture* LoadDDS(ID3D11Device* pDevice, std::wstring localFileDir);

private:
	static void ReadAllChildNodesFBX(FbxNode* pNode, std::vector<VertexPNTU>& vertices, std::vector<unsigned int>& indices);
	static void ReadMeshFBX(fbxsdk::FbxMesh* pMesh, std::vector<VertexPNTU>& vertices, std::vector<unsigned int>& indices);

	static DirectX::XMFLOAT2 GetUVFBX(const fbxsdk::FbxGeometryElementUV* pUVs, const int polygonNr,
		const int vertexNr, const int vertexIdx, const bool uvUseIndex);
	static DirectX::XMFLOAT3 GetNormalFBX(const fbxsdk::FbxGeometryElementNormal* pNormal, const int polygonNr,
		const int vertexNr, const int vertexIdx, const bool normalUseIndex);
	static DirectX::XMFLOAT3 GetTangentsFBX(const fbxsdk::FbxGeometryElementTangent* pTangent, const int polygonNr,
		const int vertexNr, const int vertexIdx, const bool tangentsUseIndex);
};
