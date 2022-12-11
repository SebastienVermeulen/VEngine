#include "pch.h"
#include "FileManager.h"
#include "ShaderStructures.h"
#include "Texture.h"
#include "Transform.h"
#include "MeshAsset.h"

std::wstring FileManager::GetAbsoluteFolderPathofFile(const std::wstring& localFilePath)
{
	return GetAbsoluteExePath() + GetLocalFolderPathofFile(localFilePath);
}
std::wstring FileManager::GetLocalFolderPathofFile(const std::wstring& localFilePath)
{
	const size_t last_slash_idx = localFilePath.rfind(L'\\');
	if (std::wstring::npos != last_slash_idx)
	{
		return localFilePath.substr(0, last_slash_idx);
	}
	else 
	{
		V_LOG(LogVerbosity::Warning, V_WTEXT("FileManager: Failed getting LocalFolderPathofFile."));
		return L"";
	}
}
std::wstring FileManager::GetAbsoluteExePath()
{
	//Get the full path
	WCHAR fullPath[MAX_PATH];
	if (!GetModuleFileNameW(NULL, fullPath, sizeof(fullPath) / 2))
	{
		return L"";
	}

	//Regex search for the full path withouth the .exe
	std::wstring str(fullPath);
	std::wregex reg(L"(.+\\\\).+\\.exe$");
	std::wsmatch matches;
	std::regex_match(str, matches, reg);

	if (!matches.size())
	{
		return L"";
	}

	return matches[1].str();
}
std::wstring FileManager::GetFileName(const std::wstring& filePath)
{
	std::wstring fullFileName = GetFullFileName(filePath);

	//Regex search
	std::wregex reg(L"(.+)[.].+$");
	std::wsmatch matches;
	std::regex_match(fullFileName, matches, reg);

	if (!matches.size())
	{
		return L"";
	}

	return matches[1].str();
}
std::wstring FileManager::GetFullFileName(const std::wstring& filePath)
{
	//Regex search for the full path withouth the .exe
	std::wregex reg(L"(.+\\\\).+[.].+$");
	std::wsmatch matches;
	std::regex_match(filePath, matches, reg);

	if (!matches.size())
	{
		return L"";
	}

	return matches[1].str();
}

#pragma region Meshes
HRESULT FileManager::LoadFBX(std::wstring localFileDir, MeshAsset* pMeshAsset)
{
	//Create importer settings and scene
	FbxManager* pFbxSdkManager = nullptr;
	pFbxSdkManager = FbxManager::Create();

	FbxIOSettings* pIOsettings = FbxIOSettings::Create(pFbxSdkManager, IOSROOT);
	pFbxSdkManager->SetIOSettings(pIOsettings);

	FbxImporter* pImporter = FbxImporter::Create(pFbxSdkManager, "");
	FbxScene* pFbxScene = FbxScene::Create(pFbxSdkManager, "");

	//Init the importer based on the path
	std::wstring fullPath = FileManager::GetAbsoluteExePath();
	fullPath += localFileDir;
	int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &fullPath[0], (int)fullPath.size(), NULL, 0, NULL, NULL);
	std::string strTo(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, &fullPath[0], (int)fullPath.size(), &strTo[0], sizeNeeded, NULL, NULL);
	bool success = pImporter->Initialize(strTo.c_str(), -1, pFbxSdkManager->GetIOSettings());
	if (!success)
	{
		pFbxSdkManager->Destroy();
		return E_FAIL;
	}

	//Try and load the data
	success = pImporter->Import(pFbxScene);
	if (!success)
	{
		pFbxSdkManager->Destroy();
		return E_FAIL;
	}
	pImporter->Destroy();

	//Go over the data and extract the needed
	int nodeCount = pFbxScene->GetNodeCount();
	for (int i = 0; i < nodeCount; ++i)
	{
		FbxNode* pNode = pFbxScene->GetNode(i);
		if (pNode->GetNodeAttribute() == NULL)
		{
			continue;
		}

		FbxNodeAttribute::EType AttributeType = pNode->GetNodeAttribute()->GetAttributeType();
		if (AttributeType != FbxNodeAttribute::eMesh)
		{
			continue;
		}

		fbxsdk::FbxMesh* pMesh = pNode->GetMesh();
		ReadMeshFBX(pMesh, pMeshAsset);

		ReadAllChildNodesFBX(pNode, pMeshAsset);
	}

	return S_OK;
}

void FileManager::ReadAllChildNodesFBX(FbxNode* pNode, MeshAsset* pMeshAsset)
{
	int childCount = pNode->GetChildCount();
	if (childCount <= 0)
	{
		return;
	}

	for (int i = 0; i < childCount; ++i)
	{
		FbxNode* pChildNode = pNode->GetChild(i);
		if (pNode->GetNodeAttribute() == NULL)
		{
			continue;
		}

		FbxNodeAttribute::EType AttributeType = pNode->GetNodeAttribute()->GetAttributeType();
		if (AttributeType != FbxNodeAttribute::eMesh)
		{
			continue;
		}

		fbxsdk::FbxMesh* pMesh = pNode->GetMesh();
		ReadMeshFBX(pMesh, pMeshAsset);

		ReadAllChildNodesFBX(pChildNode, pMeshAsset);
	}
}
void FileManager::ReadMeshFBX(fbxsdk::FbxMesh* pMesh, MeshAsset* pMeshAsset)
{
	std::vector<DirectX::XMFLOAT3> positions{};
	std::vector<DirectX::XMFLOAT3> colors{};
	std::vector<DirectX::XMFLOAT2> uvs{};
	std::vector<DirectX::XMFLOAT3> normals{};
	std::vector<DirectX::XMFLOAT3> tangents{};
	std::vector<DirectX::XMFLOAT3> binormals{};

	//Try to access the mesh's parameters
	FbxVector4* pVertices = pMesh->GetControlPoints();
	const fbxsdk::FbxGeometryElementNormal* pNormals = nullptr;
	pNormals = pMesh->GetElementNormal(0);
	const fbxsdk::FbxGeometryElementUV* pUVs = nullptr;
	pUVs = pMesh->GetElementUV(0);
	const fbxsdk::FbxGeometryElementVertexColor* pColor = nullptr;
	pColor = pMesh->GetElementVertexColor(0);
	const fbxsdk::FbxGeometryElementTangent* pTangents = nullptr;
	pTangents = pMesh->GetElementTangent(0);
	const fbxsdk::FbxGeometryElementBinormal* pBinormals = nullptr;
	pBinormals = pMesh->GetElementBinormal(0);

	//Mesh settings
	bool normalUseIndex;
	bool normalsMappingByControl;
	if (pNormals)
	{
		normalUseIndex = pNormals->GetReferenceMode() == FbxGeometryElement::eDirect;
		normalsMappingByControl = pNormals->GetMappingMode() == FbxGeometryElement::eByControlPoint;
	}
	fbxsdk::FbxStringList list{};
	if (pUVs)
	{
		pMesh->GetUVSetNames(list);
	}
	bool colorUseIndex;
	bool colorsMappingByControl;
	if (pColor)
	{
		colorUseIndex = pColor->GetReferenceMode() == FbxGeometryElement::eDirect;
		colorsMappingByControl = pColor->GetMappingMode() == FbxGeometryElement::eByControlPoint;
	}
	bool tangentUseIndex;
	bool tangentsMappingByControl;
	if (pTangents)
	{
		tangentUseIndex = pTangents->GetReferenceMode() == FbxGeometryElement::eDirect;
		tangentsMappingByControl = pTangents->GetMappingMode() == FbxGeometryElement::eByControlPoint;
	}
	bool binormalUseIndex;
	bool binormalsMappingByControl;
	if (pBinormals)
	{
		binormalUseIndex = pBinormals->GetReferenceMode() == FbxGeometryElement::eDirect;
		binormalsMappingByControl = pBinormals->GetMappingMode() == FbxGeometryElement::eByControlPoint;
	}

	//Get the correct amount of verticies
	int nrVerts = pMesh->GetPolygonVertexCount();
	//Reserve an adaquate amount of data
	positions.reserve(nrVerts);
	if (pUVs) 
	{
		uvs.reserve(nrVerts);
	}
	if (pColor)
	{
		colors.reserve(nrVerts);
	}
	if (pNormals)
	{
		normals.reserve(nrVerts);
	}
	if (pTangents)
	{
		tangents.reserve(nrVerts);
	}
	if (pBinormals)
	{
		binormals.reserve(nrVerts);
	}

	//Get all vertices
	for (int polygonNr = 0; polygonNr < pMesh->GetPolygonCount(); ++polygonNr)
	{
		//Only triangulated meshes allowed
		int numVertices = pMesh->GetPolygonSize(polygonNr);
		assert(numVertices == 3);

		//Go over all the verticies
		for (int vertexNr = 0; vertexNr < numVertices; ++vertexNr)
		{
			int vertexIndex = pMesh->GetPolygonVertex(polygonNr, vertexNr);

			positions.push_back(DirectX::XMFLOAT3(
				(float)pVertices[vertexIndex].mData[0],
				(float)pVertices[vertexIndex].mData[1],
				(float)pVertices[vertexIndex].mData[2]));
			if (pNormals)
			{
				normals.push_back(GetNormalFBX(pNormals, polygonNr, vertexNr, vertexIndex, normalUseIndex, normalsMappingByControl));
			}
			if (pUVs)
			{
				uvs.push_back(GetUVFBX(polygonNr, vertexNr, pMesh, list));
			}
			if (pColor)
			{
				colors.push_back(GetColorFBX(pColor, polygonNr, vertexNr, vertexIndex, colorUseIndex, colorsMappingByControl));
			}
			if (pTangents)
			{
				tangents.push_back(GetTangentsFBX(pTangents, polygonNr, vertexNr, vertexIndex, tangentUseIndex, tangentsMappingByControl));
			}
			if (pBinormals)
			{
				binormals.push_back(GetBinormalFBX(pBinormals, polygonNr, vertexNr, vertexIndex, binormalUseIndex, binormalsMappingByControl));
			}
		}
	}

	pMeshAsset->SetPositions(positions, false);
	pMeshAsset->SetColors(colors, false);
	pMeshAsset->SetUVs(uvs, false);
	pMeshAsset->SetNormals(normals, false);
	pMeshAsset->SetTangents(tangents, false);
	pMeshAsset->SetBinormals(binormals, false);
	pMeshAsset->RefillVertexVector();
}

DirectX::XMFLOAT2 FileManager::GetUVFBX(const int polygonNr, const int vertexNr, fbxsdk::FbxMesh* pMesh, fbxsdk::FbxStringList& list)
{
	bool unmapped{};
	fbxsdk::FbxVector2 finalUV{};

	//TO-DO: Pick correct uv's better than just 0 or split vertex up in a better way?
	pMesh->GetPolygonVertexUV(polygonNr, vertexNr, list[0], finalUV, unmapped);

	return DirectX::XMFLOAT2(
		(float)finalUV.mData[0],
		1.0f - (float)finalUV.mData[1]);
}
DirectX::XMFLOAT3 FileManager::GetColorFBX(const fbxsdk::FbxGeometryElementVertexColor* pColor, const int polygonNr, const int vertexNr, 
	const int vertexIdx, const bool colorUseIndex, const bool colorsMappingByControl)
{
	FbxColor finalColor{};
	if (colorsMappingByControl) //FbxGeometryElement::eByControlPoint
	{
		int colorIndex = colorUseIndex ? vertexIdx : pColor->GetIndexArray().GetAt(vertexIdx);
		finalColor = pColor->GetDirectArray().GetAt(colorIndex);
	}
	else //FbxGeometryElement::eByPolygonVertex
	{
		int colorIndex = colorUseIndex ? polygonNr * 3 + vertexNr : pColor->GetIndexArray().GetAt(vertexIdx);
		finalColor = pColor->GetDirectArray().GetAt(colorIndex);
	}
	return DirectX::XMFLOAT3(
		(float)finalColor[0],
		(float)finalColor[1],
		(float)finalColor[2]);
}
DirectX::XMFLOAT3 FileManager::GetNormalFBX(const fbxsdk::FbxGeometryElementNormal* pNormal, const int polygonNr, const int vertexNr, 
	const int vertexIdx, const bool normalUseIndex, const bool normalsMappingByControl)
{
	FbxVector4 finalNormal{};
	if (normalsMappingByControl) //FbxGeometryElement::eByControlPoint
	{
		int normalIndex = normalUseIndex ? vertexIdx : pNormal->GetIndexArray().GetAt(vertexIdx);
		finalNormal = pNormal->GetDirectArray().GetAt(normalIndex);
	}
	else //FbxGeometryElement::eByPolygonVertex
	{
		int normalIndex = normalUseIndex ? polygonNr * 3 + vertexNr : pNormal->GetIndexArray().GetAt(vertexIdx);
		finalNormal = pNormal->GetDirectArray().GetAt(normalIndex);
	}
	return DirectX::XMFLOAT3(
		(float)finalNormal.mData[0],
		(float)finalNormal.mData[1],
		(float)finalNormal.mData[2]);
}
DirectX::XMFLOAT3 FileManager::GetTangentsFBX(const fbxsdk::FbxGeometryElementTangent* pTangent, const int polygonNr, const int vertexNr, 
	const int vertexIdx, const bool tangentsUseIndex, const bool tangentsMappingByControl)
{
	FbxVector4 finalTangent{};
	if (tangentsMappingByControl) //FbxGeometryElement::eByControlPoint
	{
		int tangentIndex = tangentsUseIndex ? vertexIdx : pTangent->GetIndexArray().GetAt(vertexIdx);
		finalTangent = pTangent->GetDirectArray().GetAt(tangentIndex);
	}
	else //FbxGeometryElement::eByPolygonVertex
	{
		int tangentIndex = tangentsUseIndex ? polygonNr * 3 + vertexNr : pTangent->GetIndexArray().GetAt(vertexIdx);
		finalTangent = pTangent->GetDirectArray().GetAt(tangentIndex);
	}
	return DirectX::XMFLOAT3(
		(float)finalTangent.mData[0],
		(float)finalTangent.mData[1],
		(float)finalTangent.mData[2]);
}
DirectX::XMFLOAT3 FileManager::GetBinormalFBX(const fbxsdk::FbxGeometryElementBinormal* pBinormal, const int polygonNr, const int vertexNr, 
	const int vertexIdx, const bool binormalUseIndex, const bool binormalsMappingByControl)
{
	FbxVector4 finalBinormal{};
	if (binormalsMappingByControl) //FbxGeometryElement::eByControlPoint)
	{
		int normalIndex = binormalUseIndex ? vertexIdx : pBinormal->GetIndexArray().GetAt(vertexIdx);
		finalBinormal = pBinormal->GetDirectArray().GetAt(normalIndex);
	}
	else //FbxGeometryElement::eByPolygonVertex
	{
		int normalIndex = binormalUseIndex ? polygonNr * 3 + vertexNr : pBinormal->GetIndexArray().GetAt(vertexIdx);
		finalBinormal = pBinormal->GetDirectArray().GetAt(normalIndex);
	}
	return DirectX::XMFLOAT3(
		(float)finalBinormal.mData[0],
		(float)finalBinormal.mData[1],
		(float)finalBinormal.mData[2]);
}
#pragma endregion

#pragma region Textures
Texture* FileManager::LoadDDS(ID3D11Device* pDevice, std::wstring localFileDir)
{
	HRESULT hr;

	const std::wstring absoluteFilePath = GetAbsoluteExePath() + localFileDir;

	//Create DX11 variables
	ID3D11Resource* pResource;
	ID3D11ShaderResourceView* pResourceView;
	hr = DirectX::CreateDDSTextureFromFile(pDevice, absoluteFilePath.c_str(), &pResource, &pResourceView);
	if (hr != S_OK)
	{
		V_LOG(LogVerbosity::Warning, V_WTEXT("FileManager: Failed creating a texture from a DDS."));
		return nullptr;
	}

	return new Texture(pResource, pResourceView);
}
#pragma endregion
