#include "pch.h"
#include "GreyScale.h"
#include "PostProcesses.h"
#include "EngineManager.h"
#include "EngineDevice.h"
#include "RenderUtils.h"
#include "PostProcessWidget.h"
#include "RenderAnnotation.h"

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
	MaterialTextureParam MaterialTextureParam{ nullptr, true, "gFinalRender", L"", nullptr };
	m_pMaterial->GetTextureParams().AddMapping(MaterialTextureParam);
	HRESULT hr = m_pMaterial->InitShader(pDevice->GetDevice(), pDevice->GetDeviceContext());

	CreateNDCQuad(&m_pScreenQuadVertexBuffer, &m_pScreenQuadIndexBuffer, m_ScreenQuadNrVerticies, m_ScreenQuadNrIndicies);

	return hr == S_OK;
}
void GreyScalePostProcess::ReleaseShader()
{
	SafeDelete(m_pMaterial);
}

void GreyScalePostProcess::RunEffect(EngineDevice* pDevice)
{
	V_DX11_ANNOTATE(V_WTEXT("GreyScale"));

	ID3D11DeviceContext* pContext = pDevice->GetDeviceContext();

	PostProcessSwapChain* pPostProcessSwapChain = PostProcessSwapChain::Instance();

	//Set the material buffers
	{
		const std::string name = "gFinalRender";
		m_pMaterial->GetTextureParams().GetMappingbasedOnName(name)->SetTexture(pPostProcessSwapChain->GetSource(), true);
		m_pMaterial->UpdateParameterValues(pDevice);
	}

	//Select which vertex buffer to display
	const UINT stride = sizeof(QuadVertex);
	const UINT offset = 0;
	pContext->IASetVertexBuffers(0, 1, &m_pScreenQuadVertexBuffer, &stride, &offset);

	//Set the index buffer to active in the input assembler so it can be rendered.
	pContext->IASetIndexBuffer(m_pScreenQuadIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set the render target
	ID3D11RenderTargetView* renderTargs[] =
	{
		pPostProcessSwapChain->GetTarget()->GetRenderTargetView()
	};
	pContext->OMSetRenderTargets(1, renderTargs, nullptr);

	m_pMaterial->Render(pDevice->GetDeviceContext(), QUAD_INDICES, 0);

	//Unhook render targets from material
	EngineManager::Instance()->GetActiveRenderer()->ExplicitlyUnbindingRenderTargets(m_pMaterial);

	pPostProcessSwapChain->Swap();
}
