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

	// ------------------------------------------
	// Generate missing data
	// ------------------------------------------
	// TO-DO: Clean this up based on user settings etc.
	if (m_UVs[0].size() == 0)
	{
		m_MeshSettings.m_AlbedoScalar = true;
		m_MeshSettings.m_MetalnessScalar = true;
		m_MeshSettings.m_RoughnessScalar = true;

		AutoUnwrapUVs();
	}
	if (m_Normals.size() == 0 || m_MeshSettings.m_ForceGenerateNormals)
	{
		GenerateNormals();
	}

	// ------------------------------------------
	// Reduce duplicate vertex information
	// ------------------------------------------
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

	// ------------------------------------------
	// Pack our verts
	// ------------------------------------------
	m_Vertices.reserve(individualIndices.size());
	for (DirectX::XMINT3& index : individualIndices)
	{
		// Needs some expanding
		// -------
		// Normals
		DirectX::XMFLOAT3 normal{};
		if (m_MeshSettings.m_GeneratedNormals && !m_MeshSettings.m_CombineNormals)
		{
			normal = m_Normals[index.z];
		}
		else 
		{
			normal = m_Normals[index.x];
		}
		// -------
		//   UVs
		DirectX::XMFLOAT2 uv = m_MeshSettings.m_GeneratedUVs ? m_UVs[m_MeshSettings.m_ChosenUVMap][index.x] : m_UVs[m_MeshSettings.m_ChosenUVMap][index.y];
		// ---------
		// Positions
		m_Vertices.emplace_back(DirectX::XMFLOAT4{}, m_Positions[index.x], normal, uv);
	}
}

void MeshAsset::AutoUnwrapUVs()
{
	// TO-DO: Implement something here, perhaps just using a lib.?
	m_UVs[0] = std::vector<DirectX::XMFLOAT2>{ m_Positions.size() };

	m_MeshSettings.m_GeneratedUVs = true;
}
void MeshAsset::GenerateNormals()
{
	std::vector<DirectX::XMVECTOR> faceNormals{ m_UnpackedIndices.size() / 3 };
	std::vector<DirectX::XMVECTOR>::iterator faceNormalIterator{ faceNormals.begin() };

	// Loop over all faces to determine their normals
 	std::vector<DirectX::XMINT3>::iterator face{ m_UnpackedIndices.begin()};
	while (face != m_UnpackedIndices.end())
	{
		DirectX::XMVECTOR VectorA{ VMath::Sub(m_Positions[(*(face + 1)).x], m_Positions[(*face).x]) };
		DirectX::XMVECTOR VectorB{ VMath::Sub(m_Positions[(*(face + 2)).x], m_Positions[(*face).x]) };

		*faceNormalIterator = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(VectorA, VectorB));

		// Go to the next face's first index
		faceNormalIterator += 1;
		face += 3;
	}

	if (m_MeshSettings.m_CombineNormals)
	{
		// Loop over all vertices to find them in each face and averge out their normals 
		m_Normals.reserve(m_Positions.size());
		for (int i = 0; i < m_Positions.size(); ++i)
		{
			std::vector<DirectX::XMVECTOR> normalAvarage{};
			for (int j = 0; j < m_UnpackedIndices.size(); ++j)
			{
				// Is this vertex referenced in the face
				if (i == m_UnpackedIndices[j].x)
				{
					normalAvarage.push_back(faceNormals[j / 3]);
				}
			}

			// Get the average
			DirectX::XMFLOAT3 finalNormal{};
			for (int k = 1; k < normalAvarage.size(); ++k)
			{
				normalAvarage[0] = DirectX::XMVectorAdd(normalAvarage[0], normalAvarage[k]);
			}
			DirectX::XMStoreFloat3(&finalNormal, DirectX::XMVector3Normalize(normalAvarage[0]));

			// Store our completed average
			m_Normals.push_back(finalNormal);
		}
	}
	else 
	{
		// Simply store the normal of each face
		m_Normals.reserve(m_Indices.size() / 3);
		DirectX::XMFLOAT3 normal{};
		for (int i = 0; i < faceNormals.size(); ++i)
		{
			DirectX::XMStoreFloat3(&normal, faceNormals[i]);
			m_Normals.push_back(normal);

			// Adjust the unpacked indices as well
			m_UnpackedIndices[i * 3].z = i;
			m_UnpackedIndices[i * 3 + 1].z = i;
			m_UnpackedIndices[i * 3 + 2].z = i;
		}
	}

	m_MeshSettings.m_GeneratedNormals = true;
}
void MeshAsset::GenerateTangents()
{
	// TO-DO: Implement when we actually need tangents...
	m_MeshSettings.m_GeneratedTangents = true;
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
