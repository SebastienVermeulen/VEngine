#include "pch.h"
#include "GreyScale.h"
#include "EngineDevice.h"
#include "RenderUtils.h"
#include "PostProcessWidget.h"

GreyScalePostProcess::GreyScalePostProcess()
	: m_pScreenQuadVertexBuffer{ nullptr }
	, m_pScreenQuadIndexBuffer{ nullptr }
	, m_ScreenQuadNrVerticies{}
	, m_ScreenQuadNrIndicies{}
	, m_pMaterial{nullptr}
	, m_bActive{false}
	, m_pGreyScalePostProcessWidget{new GreyScalePostProcessWidget(this)}
{
}
GreyScalePostProcess::~GreyScalePostProcess()
{
	SafeDelete(m_pGreyScalePostProcessWidget);
}

bool GreyScalePostProcess::InitShader(const EngineDevice* pDevice)
{
	m_pMaterial = new Material(L"..\\Resources\\Shaders\\PostProcessing\\GreyScale.fx", RenderType::postprocessPass);
	HRESULT hr = m_pMaterial->InitShader(pDevice->GetDevice(), pDevice->GetDeviceContext());

	CreateNDCQuad(&m_pScreenQuadVertexBuffer, &m_pScreenQuadIndexBuffer, m_ScreenQuadNrVerticies, m_ScreenQuadNrIndicies);

	return hr == S_OK;
}
void GreyScalePostProcess::ReleaseShader()
{
	SafeDelete(m_pMaterial);
}

void GreyScalePostProcess::SetupEffectEnvironment(EngineDevice* pDevice)
{
	//Set the material buffers
	m_pMaterial->UpdateParameterValues(pDevice);
	//Set the render target
	ID3D11DeviceContext* pContext = pDevice->GetDeviceContext();
	ID3D11RenderTargetView* renderTarg = EngineManager::Instance()->GetActiveRenderer()->GetFinalTarget()->pRenderTargetView;
	pContext->OMSetRenderTargets(1, &renderTarg, pDevice->GetDepthBuffer());
	//Second/Deferred pass
	//Select which vertex buffer to display
	const UINT stride = sizeof(QuadVertex);
	const UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_pScreenQuadVertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered.
	pContext->IASetIndexBuffer(m_pScreenQuadIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void GreyScalePostProcess::RunEffect(EngineDevice* pDevice)
{
	m_pMaterial->Render(pDevice->GetDeviceContext(), QUAD_INDICES, 0);
}

void GreyScalePostProcess::TeardownEffectEnvironment(EngineDevice* pDevice)
{
}
