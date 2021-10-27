#include "pch.h"
#include "MeshComponent.h"
#include "Material.h"
#include "ShaderStructures.h"
#include "EngineManager.h"
#include "FileManager.h"
#include "MeshWidget.h"

MeshComponent::MeshComponent(const std::wstring& fileName)
	:Component()
	, m_FileName{ fileName }
	, m_pVertexBuffer{}
	, m_pIndexBuffer{}
	, m_NrVerticies{}
	, m_NrIndicies{}
{
	m_pWidget = new MeshWidget(this);
}
MeshComponent::~MeshComponent()
{
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
}

bool MeshComponent::Init()
{
	m_ShouldRender = true;

	//***********************************************************************************
	// 	   Mesh
	//***********************************************************************************
	std::vector<VertexPNTU> vertices;
	std::vector<unsigned int> indices;
	FileManager::LoadFBX(vertices, indices, m_FileName);
	VertexPNTU* verticesArray = &vertices[0];
	m_NrVerticies = (UINT)vertices.size();
	unsigned int* indicesArray = &indices[0];
	m_NrIndicies = (UINT)indices.size();

	D3D11_SUBRESOURCE_DATA vertexMS, indexMS;
	D3D11_BUFFER_DESC vertexBD, indexBD;
	HRESULT result;

	//Vertex
	vertexBD.Usage = D3D11_USAGE_DEFAULT;							
	vertexBD.ByteWidth = sizeof(VertexPNTU) * m_NrVerticies;			
	vertexBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;					
	vertexBD.CPUAccessFlags = 0;
	vertexBD.MiscFlags = 0;
	vertexBD.StructureByteStride = 0;

	//Give the subresource structure a pointer to the vertex data.
	vertexMS.pSysMem = verticesArray;
	vertexMS.SysMemPitch = 0;
	vertexMS.SysMemSlicePitch = 0;

	//Now create the vertex buffer.
	EngineDevice* pEngineDiv = EngineManager::Instance()->GetDevice();
	result = pEngineDiv->GetDevice()->CreateBuffer(&vertexBD, &vertexMS, &m_pVertexBuffer);
	if (FAILED(result))
	{
		//TO-DO: make logger
		return false;
	}

	//Index
	indexBD.Usage = D3D11_USAGE_DEFAULT;							
	indexBD.ByteWidth = sizeof(unsigned int) * m_NrIndicies;		
	indexBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBD.CPUAccessFlags = 0;
	indexBD.MiscFlags = 0;
	indexBD.StructureByteStride = 0;
	
	//Give the subresource structure a pointer to the index data.
	indexMS.pSysMem = indicesArray;
	indexMS.SysMemPitch = 0;
	indexMS.SysMemSlicePitch = 0;
	
	//Now create the index buffer.
	result = pEngineDiv->GetDevice()->CreateBuffer(&indexBD, &indexMS, &m_pIndexBuffer);
	if (FAILED(result))
	{
		//TO-DO: make logger
		return false;
	}

	//Success
	return true;
}

void MeshComponent::Render(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int passNr) const
{
	if (!m_pMaterial)
	{
		return;
	}

	//Select which vertex buffer to display
	const UINT stride = sizeof(VertexPNTU);
	const UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered.
	pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pMaterial->Render(pContext, m_NrIndicies, passNr);
}
