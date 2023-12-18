#pragma once
#include "Asset.h"
#include "ShaderStructures.h"

struct MeshSettings
{
	// Geometry related
	bool m_ForceGenerateNormals = false;
	bool m_CombineNormals = false;
	bool m_ForceGenerateTangents = false;
	// Generation
	bool m_GeneratedUVs = false;
	bool m_GeneratedNormals = false;
	bool m_GeneratedTangents = false;

	// UV
	int m_ChosenUVMap = 0;

	// Material related
	bool m_AlbedoScalar = false;
	bool m_MetalnessScalar = false;
	bool m_RoughnessScalar = false;
};

class MeshAsset : public Asset 
{
public:
	MeshAsset();
	~MeshAsset();

	MeshAsset(MeshAsset& other) = delete;
	MeshAsset(MeshAsset&& other) = delete;
	MeshAsset operator=(MeshAsset& other) = delete;
	MeshAsset& operator=(MeshAsset&& other) = delete;

	inline std::vector<Vertex> GetVerticies() const							{ return m_Vertices; }
	inline std::vector<DirectX::XMINT3> GetUnpackedIndicies() const			{ return m_UnpackedIndices; }
	inline std::vector<int> GetIndicies() const								{ return m_Indices; }
	inline std::vector<DirectX::XMFLOAT3> GetPositions() const				{ return m_Positions; }
	inline std::vector<std::vector<DirectX::XMFLOAT2>> GetUVs() const		{ return m_UVs; }
	inline std::vector<DirectX::XMFLOAT3> GetNormals() const				{ return m_Normals; }
	inline std::vector<DirectX::XMFLOAT4> GetTangents() const				{ return m_Tangents; }

	inline std::vector<Vertex>& GetVerticies()								{ return m_Vertices; }
	inline std::vector<DirectX::XMINT3>& GetUnpackedIndicies()				{ return m_UnpackedIndices; }
	inline std::vector<int>& GetIndicies()									{ return m_Indices; }
	inline std::vector<DirectX::XMFLOAT3>& GetPositions()					{ return m_Positions; }
	inline std::vector<std::vector<DirectX::XMFLOAT2>>& GetUVs()			{ return m_UVs; }
	inline std::vector<DirectX::XMFLOAT3>& GetNormals()						{ return m_Normals; }
	inline std::vector<DirectX::XMFLOAT4>& GetTangents()					{ return m_Tangents; }

	void SetUnpackedIndicies(const std::vector<DirectX::XMINT3>& indices);
	void SetIndicies(const std::vector<int>& indices);
	void SetPositions(const std::vector<DirectX::XMFLOAT3>& positions);
	void SetUVs(const std::vector<std::vector<DirectX::XMFLOAT2>>& uvs);
	void SetNormals(const std::vector<DirectX::XMFLOAT3>& normals);
	void SetTangents(const std::vector<DirectX::XMFLOAT4>& tangents);

	/*
	We need to run this function in order to pack the mesh data into a usable vertex array.
	That can be uploaded to the GPU.
	*/
	void RefillVertexVector();

	void AutoUnwrapUVs();
	void GenerateNormals();
	void GenerateTangents();

	inline MeshSettings& GetMeshSettings() 
	{
		return m_MeshSettings;
	}

private:
	virtual void WriteToVAsset() const override;

	MeshSettings m_MeshSettings;

	std::vector<Vertex> m_Vertices;
	// TO-DO: Make this temporal, since this is a lot of memory to just keep around
	std::vector<DirectX::XMFLOAT3> m_Positions;
	std::vector<std::vector<DirectX::XMFLOAT2>> m_UVs;
	std::vector<DirectX::XMFLOAT3> m_Normals;
	std::vector<DirectX::XMFLOAT4> m_Tangents;
	std::vector<DirectX::XMINT3> m_UnpackedIndices;
	std::vector<int> m_Indices;
};
