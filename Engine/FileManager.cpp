#include "pch.h"
#include "FileManager.h"
#include "ShaderStructures.h"
#include "Texture.h"
#include "Transform.h"

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

HRESULT FileManager::LoadFBX(std::vector<VertexPNTU>& vertices, std::vector<unsigned int>& indices, std::wstring localFileDir)
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
	for (int i = 0; i < nodeCount; i++)
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
		ReadMeshFBX(pMesh, vertices, indices);

		ReadAllChildNodesFBX(pNode, vertices, indices);
	}

	return S_OK;
}

void FileManager::ReadAllChildNodesFBX(FbxNode* pNode, std::vector<VertexPNTU>& vertices, std::vector<unsigned int>& indices)
{
	int childCount = pNode->GetChildCount();
	if (childCount <= 0)
	{
		return;
	}

	for (int i = 0; i < childCount; i++)
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
		ReadMeshFBX(pMesh, vertices, indices);

		ReadAllChildNodesFBX(pChildNode, vertices, indices);
	}
}
void FileManager::ReadMeshFBX(fbxsdk::FbxMesh* pMesh, std::vector<VertexPNTU>& vertices, std::vector<unsigned int>& indices)
{
	//Try to access the mesh's parameters
	FbxVector4* pVertices = pMesh->GetControlPoints();
	const fbxsdk::FbxGeometryElementNormal* pNormals = nullptr;
	pNormals = pMesh->GetElementNormal(0);
	const fbxsdk::FbxGeometryElementUV* pUVs = nullptr;
	pUVs = pMesh->GetElementUV(0);
	const fbxsdk::FbxGeometryElementTangent* pTangents = nullptr;
	pTangents = pMesh->GetElementTangent(0);

	//Mesh settings
	const bool normalUseIndex = pNormals->GetReferenceMode() == FbxGeometryElement::eDirect;
	const bool uvUseIndex = pUVs->GetReferenceMode() != FbxGeometryElement::eDirect;
	const bool tangentUseIndex = pTangents->GetReferenceMode() == FbxGeometryElement::eDirect;

	//Reserve an adaquate amount of data
	int nrVerts = pMesh->GetControlPointsCount();
	vertices.resize(nrVerts);
	indices.reserve(pMesh->GetPolygonCount() * 3);
	//Go over all the polygons in the mesh
	for (int polygonNr = 0; polygonNr < pMesh->GetPolygonCount(); polygonNr++)
	{
		//Only triangulated meshes allowed
		int numVertices = pMesh->GetPolygonSize(polygonNr);
		assert(numVertices == 3);

		//Go over all the verticies
		for (int vertexNr = 0; vertexNr < numVertices; vertexNr++)
		{
			int vertexIndex = pMesh->GetPolygonVertex(polygonNr, vertexNr);

			std::vector<unsigned int>::iterator possibleDuplicate = std::find(indices.begin(), indices.end(), vertexIndex);
			if (possibleDuplicate != indices.end())
			{
				indices.push_back(vertexIndex);
				continue;
			}
			indices.push_back(vertexIndex);

			VertexPNTU vertex;
			vertex.position = DirectX::XMFLOAT3(
				(float)pVertices[vertexIndex].mData[0],
				(float)pVertices[vertexIndex].mData[1],
				(float)pVertices[vertexIndex].mData[2]);
			vertex.normal =		GetNormalFBX(pNormals, polygonNr, vertexNr, vertexIndex, normalUseIndex);
			vertex.uv =			GetUVFBX(pUVs, polygonNr, vertexNr, vertexIndex, uvUseIndex);
			vertex.tangent =	GetTangentsFBX(pTangents, polygonNr, vertexNr, vertexIndex, tangentUseIndex);

			vertices[vertexIndex] = vertex;
		}
	}
}

DirectX::XMFLOAT2 FileManager::GetUVFBX(const fbxsdk::FbxGeometryElementUV* pUVs, const int polygonNr, 
	const int vertexNr, const int vertexIdx, const bool uvUseIndex)
{
	FbxVector2 finalUV{};
	if (pUVs->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		int uvIndex = uvUseIndex ? pUVs->GetIndexArray().GetAt(vertexIdx) : vertexIdx;
		finalUV = pUVs->GetDirectArray().GetAt(uvIndex);
	}
	else if (pUVs->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		int uvIndex = uvUseIndex ? pUVs->GetIndexArray().GetAt(polygonNr * 3 + vertexNr) : polygonNr * 3 + vertexNr;
		finalUV = pUVs->GetDirectArray().GetAt(uvIndex);
	}
	return DirectX::XMFLOAT2(
		(float)finalUV.mData[0],
		-(float)finalUV.mData[1]);
}
DirectX::XMFLOAT3 FileManager::GetNormalFBX(const fbxsdk::FbxGeometryElementNormal* pNormal, const int polygonNr,
	const int vertexNr, const int vertexIdx, const bool normalUseIndex)
{
	FbxVector4 finalNormal{};
	if (pNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		int normalIndex = normalUseIndex ? vertexIdx : pNormal->GetIndexArray().GetAt(vertexIdx);
		finalNormal = pNormal->GetDirectArray().GetAt(normalIndex);
	}
	else if (pNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		int normalIndex = normalUseIndex ? polygonNr * 3 + vertexNr : pNormal->GetIndexArray().GetAt(vertexIdx);
		finalNormal = pNormal->GetDirectArray().GetAt(normalIndex);
	}
	return DirectX::XMFLOAT3(
		(float)finalNormal.mData[0],
		(float)finalNormal.mData[1],
		(float)finalNormal.mData[2]);
}
DirectX::XMFLOAT3 FileManager::GetTangentsFBX(const fbxsdk::FbxGeometryElementTangent* pTangent, const int polygonNr,
	const int vertexNr, const int vertexIdx, const bool tangentsUseIndex)
{
	FbxVector4 finalTangent{};
	if (pTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		int tangentIndex = tangentsUseIndex ? vertexIdx : pTangent->GetIndexArray().GetAt(vertexIdx);
		finalTangent = pTangent->GetDirectArray().GetAt(tangentIndex);
	}
	else if (pTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		int tangentIndex = tangentsUseIndex ? polygonNr * 3 + vertexNr : pTangent->GetIndexArray().GetAt(vertexIdx);
		finalTangent = pTangent->GetDirectArray().GetAt(tangentIndex);
	}
	return DirectX::XMFLOAT3(
		(float)finalTangent.mData[0],
		(float)finalTangent.mData[1],
		(float)finalTangent.mData[2]);
}

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
		//TO-DO: Add logger
		return nullptr;
	}

	return new Texture(pResource, pResourceView);
}
