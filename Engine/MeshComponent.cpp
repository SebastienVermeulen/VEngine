#include "pch.h"
#include "MeshComponent.h"
#include "Material.h"
#include "ShaderStructures.h"
#include "EngineManager.h"

MeshComponent::MeshComponent()
	:Component()
	, m_pMaterial{}
	, m_pVertexBuffer{}
	, m_NrVerticies{}
{
	m_ShouldRender = true;
}
MeshComponent::~MeshComponent()
{
	SafeDelete(m_pMaterial);
	SafeRelease(m_pVertexBuffer);
}

void MeshComponent::Init()
{
	Vertex ourVertices[] =
	{
		{0.0f, 0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)},
		{0.45f, -0.5, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)},
		{-0.45f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)}
	};
	m_NrVerticies = 3;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;							// write access access by CPU and GPU
	bd.ByteWidth = sizeof(Vertex) * 3;						// size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;				// use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;				// allow CPU to write in buffer

	EngineDevice& engineDiv = EngineManager::Instance().GetDevice();
	engineDiv.GetDevice().CreateBuffer(&bd, NULL, &m_pVertexBuffer);       // create the buffer

	D3D11_MAPPED_SUBRESOURCE ms;
	ID3D11DeviceContext& context = engineDiv.GetDeviceContext();
	context.Map(m_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);		// map the buffer
	memcpy(ms.pData, ourVertices, sizeof(ourVertices));							// copy the data
	context.Unmap(m_pVertexBuffer, NULL);										// unmap the buffer
}

void MeshComponent::Render(ID3D11Device& device, ID3D11DeviceContext& context) const
{
	if (!m_pMaterial) 
	{
		return;
	}

	// select which vertex buffer to display
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context.IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// select which primtive type we are using
	context.IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw the vertex buffer to the back buffer
	context.Draw(m_NrVerticies, 0);
}

void MeshComponent::SetMaterial(Material* pMat) 
{
	m_pMaterial = pMat;

	if (m_pMaterial)
	{
		EngineDevice& eDevice = EngineManager::Instance().GetDevice();
		m_pMaterial->InitShader(eDevice.GetDevice(), eDevice.GetDeviceContext());
	}
}
