#include "pch.h"
#include "MeshAsset.h"
#include "FileManager.h"
#include "VAssetWriterReader.h"

MeshAsset::MeshAsset()
	:Asset()
	, m_Vertices{}
	, m_Colors{}
	, m_UVs{}
	, m_Normals{}
	, m_Tangents{}
	, m_Binormals{}
	, m_Indices{}
{
}
MeshAsset::~MeshAsset()
{
}

void MeshAsset::FillVertexBuffer() const
{
}
void MeshAsset::FillIndexBuffer() const
{
}

void MeshAsset::SetIndicies(const std::vector<unsigned int>& indices, const bool refillVertexVector) 
{ 
	m_Indices = indices; 
}
void MeshAsset::SetPositions(const std::vector<DirectX::XMFLOAT3>& positions, const bool refillVertexVector) 
{ 
	m_Positions = positions;

	if (refillVertexVector)
	{
		RefillVertexVector();
	}
}
void MeshAsset::SetColors(const std::vector<DirectX::XMFLOAT3>& colors, const bool refillVertexVector) 
{ 
	m_Colors = colors;

	if (refillVertexVector)
	{
		RefillVertexVector();
	}
}
void MeshAsset::SetUVs(const std::vector<DirectX::XMFLOAT2>& UVs, const bool refillVertexVector) 
{ 
	m_UVs = UVs;

	if (refillVertexVector)
	{
		RefillVertexVector();
	}
}
void MeshAsset::SetNormals(const std::vector<DirectX::XMFLOAT3>& normals, const bool refillVertexVector) 
{ 
	m_Normals = normals;

	if (refillVertexVector)
	{
		RefillVertexVector();
	}
}
void MeshAsset::SetTangents(const std::vector<DirectX::XMFLOAT3>& tangents, const bool refillVertexVector) 
{ 
	m_Tangents = tangents;

	if (refillVertexVector)
	{
		RefillVertexVector();
	}
}
void MeshAsset::SetBinormals(const std::vector<DirectX::XMFLOAT3>& binormals, const bool refillVertexVector) 
{ 
	m_Binormals = binormals;

	if (refillVertexVector)
	{
		RefillVertexVector();
	}
}

void MeshAsset::RefillVertexVector() 
{
	m_Vertices.clear();

	if (m_Colors.size() == 0) 
	{
		m_Colors.resize(m_Positions.size());
	}
	if (m_Normals.size() == 0)
	{
		m_Normals.resize(m_Positions.size());
	}
	if (m_Tangents.size() == 0)
	{
		m_Tangents.resize(m_Positions.size());
	}
	if (m_Binormals.size() == 0)
	{
		m_Binormals.resize(m_Positions.size());
	}

	std::vector<Vertex> tempVerts{};
	std::vector<unsigned int> tempInds{};

	unsigned int index = 0;
	int nrSkippedIdx = 0;
	Vertex tempVertex = Vertex{};
	for (int i = 0; i < m_Positions.size(); ++i)
	{
		tempVertex = Vertex(m_Positions[i], m_Colors[i], m_Normals[i], m_Tangents[i], m_Binormals[i], m_UVs[i]);

		bool duplicateFound = false;
		for (unsigned int j = 0; j < tempVerts.size(); ++j)
		{
			if (tempVertex == tempVerts[j])
			{
				tempInds.push_back(j);
				duplicateFound = true;
				++nrSkippedIdx;
				break;
			}
		}
		if (duplicateFound) 
		{
			continue;
		}

		index = unsigned int(tempVerts.size());
		tempInds.push_back(index);
		tempVerts.push_back(tempVertex);
	}
	m_Vertices = tempVerts;
	m_Indices = tempInds;
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
