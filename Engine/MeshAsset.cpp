#include "pch.h"
#include "MeshAsset.h"
#include "FileManager.h"
#include "VAssetWriterReader.h"

MeshAsset::MeshAsset()
	:Asset()
	, m_Vertices{}
	, m_UVs{}
	, m_Normals{}
	, m_Tangents{}
	, m_Indices{}
{
}
MeshAsset::~MeshAsset()
{
}

void MeshAsset::SetUnpackedIndicies(const std::vector<DirectX::XMINT3>& indices)
{
	m_UnpackedIndices = indices;
}
void MeshAsset::SetIndicies(const std::vector<int>& indices)
{ 
	m_Indices = indices; 
}
void MeshAsset::SetPositions(const std::vector<DirectX::XMFLOAT3>& positions) 
{ 
	m_Positions = positions;
}
void MeshAsset::SetUVs(const std::vector<std::vector<DirectX::XMFLOAT2>>& UVs)
{ 
	m_UVs = UVs;
}
void MeshAsset::SetNormals(const std::vector<DirectX::XMFLOAT3>& normals) 
{ 
	m_Normals = normals;
}
void MeshAsset::SetTangents(const std::vector<DirectX::XMFLOAT4>& tangents) 
{ 
	m_Tangents = tangents;
}

void MeshAsset::RefillVertexVector()
{
	m_Vertices.clear();

	// TO-DO: hook this int up to a setting
	int chosenUVMap = 0;

	// Remap our indices to make sure we only map the minimum needed verts
	std::vector<DirectX::XMINT3> individualIndices;

	// Continue to iterate
	for (int i = 0; i < m_UnpackedIndices.size(); ++i)
	{
		const int nrIndicies = (int)m_Indices.size();
		for (int j = 0; j < individualIndices.size(); ++j)
		{
			if (VMath::Compare(m_UnpackedIndices[i], individualIndices[j]))
			{
				m_Indices.push_back(j);
				break;
			}
		}

		// None have been added
		if (nrIndicies == m_Indices.size())
		{
			m_Indices.push_back((int)individualIndices.size());
			individualIndices.push_back(m_UnpackedIndices[i]);
		}
	}

	// Pack our verts
	m_Vertices.reserve(individualIndices.size());
	for (DirectX::XMINT3& index : individualIndices)
	{
		// TO-DO: Clean this up based on user settings etc.
		// TO-DO: Make tangent generation possible
		DirectX::XMFLOAT3 normal = index.z < 0 ? DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f} : m_Normals[index.z];
		DirectX::XMFLOAT2 uv = index.y < 0 ? DirectX::XMFLOAT2{ 0.0f, 0.0f } : m_UVs[chosenUVMap][index.y];
		m_Vertices.emplace_back(DirectX::XMFLOAT4{}, m_Positions[index.x], normal, uv);
	}
}

void MeshAsset::WriteToVAsset() const 
{
	std::wstring dataToWrite{};

	//Document
	dataToWrite += m_FilePath + L'\n';

	//Vertices
	for (int i = 0; i < m_Vertices.size(); ++i)
	{
		m_Vertices;
	}

	VAssetWriterReader::Instance()->WriteAsset(m_FilePath, dataToWrite);
}
