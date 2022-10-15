#include "pch.h"
#include "MeshComponent.h"
#include "Material.h"
#include "ShaderStructures.h"
#include "EngineManager.h"
#include "FileManager.h"
#include "MeshWidget.h"
#include "MeshAsset.h"
#include "MeshFactory.h"

MeshComponent::MeshComponent(const std::wstring& fileName)
	:Component()
	, m_pAsset{}
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
	m_pAsset = MeshFactory::Instance()->CreateAsset(m_FileName);

	std::vector<Vertex> verticies = m_pAsset->GetVerticies();
	std::vector<unsigned int> indicies = m_pAsset->GetIndicies();

	EngineDevice* pEngineDiv = EngineManager::Instance()->GetDevice();
	if (FAILED(pEngineDiv->CreateVertexBuffer(verticies, &m_pVertexBuffer)))
	{
		//TO-DO: make logger
		return false;
	}
	if (FAILED(pEngineDiv->CreateIndexBuffer(indicies, &m_pIndexBuffer)))
	{
		//TO-DO: make logger
		return false;
	}
	m_NrVerticies = (UINT)verticies.size();
	m_NrIndicies = (UINT)indicies.size();

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
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered.
	pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pMaterial->Render(pContext, m_NrIndicies, passNr);
}
