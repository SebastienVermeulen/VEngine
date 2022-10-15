#pragma once
#include "Asset.h"
#include "ShaderStructures.h"

class MeshAsset : public Asset 
{
public:
	MeshAsset();
	~MeshAsset();

	MeshAsset(MeshAsset& other) = delete;
	MeshAsset(MeshAsset&& other) = delete;
	MeshAsset operator=(MeshAsset& other) = delete;
	MeshAsset& operator=(MeshAsset&& other) = delete;

	void FillVertexBuffer() const;
	void FillIndexBuffer() const;

	inline std::vector<Vertex> GetVerticies() const				{ return m_Vertices; }
	inline std::vector<unsigned int> GetIndicies() const		{ return m_Indices; }
	inline std::vector<DirectX::XMFLOAT3> GetPositions() const	{ return m_Positions; }
	inline std::vector<DirectX::XMFLOAT3> GetColors() const		{ return m_Colors; }
	inline std::vector<DirectX::XMFLOAT2> GetUVs() const		{ return m_UVs; }
	inline std::vector<DirectX::XMFLOAT3> GetNormals() const	{ return m_Normals; }
	inline std::vector<DirectX::XMFLOAT3> GetTangents() const	{ return m_Tangents; }
	inline std::vector<DirectX::XMFLOAT3> GetBinormals() const	{ return m_Binormals; }

	inline std::vector<Vertex>& GetVerticies()					{ return m_Vertices; }
	inline std::vector<unsigned int>& GetIndicies()				{ return m_Indices; }
	inline std::vector<DirectX::XMFLOAT3>& GetPositions()		{ return m_Positions; }
	inline std::vector<DirectX::XMFLOAT3>& GetColors()			{ return m_Colors; }
	inline std::vector<DirectX::XMFLOAT2>& GetUVs()				{ return m_UVs; }
	inline std::vector<DirectX::XMFLOAT3>& GetNormals()			{ return m_Normals; }
	inline std::vector<DirectX::XMFLOAT3>& GetTangents()		{ return m_Tangents; }
	inline std::vector<DirectX::XMFLOAT3>& GetBinormals()		{ return m_Binormals; }

	void SetIndicies(const std::vector<unsigned int>& indices, const bool refillVertexVector = true);
	void SetPositions(const std::vector<DirectX::XMFLOAT3>& positions, const bool refillVertexVector = true);
	void SetColors(const std::vector<DirectX::XMFLOAT3>& vertices, const bool refillVertexVector = true);
	void SetUVs(const std::vector<DirectX::XMFLOAT2>& vertices, const bool refillVertexVector = true);
	void SetNormals(const std::vector<DirectX::XMFLOAT3>& vertices, const bool refillVertexVector = true);
	void SetTangents(const std::vector<DirectX::XMFLOAT3>& vertices, const bool refillVertexVector = true);
	void SetBinormals(const std::vector<DirectX::XMFLOAT3>& vertices, const bool refillVertexVector = true);

	void RefillVertexVector();

private:
	virtual void WriteToVAsset() const override;

	std::vector<Vertex> m_Vertices;
	std::vector<DirectX::XMFLOAT3> m_Positions;
	std::vector<DirectX::XMFLOAT3> m_Colors;
	std::vector<DirectX::XMFLOAT2> m_UVs;
	std::vector<DirectX::XMFLOAT3> m_Normals;
	std::vector<DirectX::XMFLOAT3> m_Tangents;
	std::vector<DirectX::XMFLOAT3> m_Binormals;
	std::vector<unsigned int> m_Indices;
};
