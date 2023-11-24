#include "pch.h"
#include "ScreenSpaceCopy.h"
#include "EngineManager.h"
#include "EngineDevice.h"
#include "RenderUtils.h"
#include "Texture.h"
#include "PostProcessWidget.h"
#include "RenderAnnotation.h"

ScreenSpaceCopy::ScreenSpaceCopy()
	: m_pScreenQuadVertexBuffer{ nullptr }
	, m_pScreenQuadIndexBuffer{ nullptr }
	, m_ScreenQuadNrVerticies{}
	, m_ScreenQuadNrIndicies{}
	, m_pMaterial{ nullptr }
{
}
ScreenSpaceCopy::~ScreenSpaceCopy()
{
}

bool ScreenSpaceCopy::InitShader(const EngineDevice* pDevice)
{
	m_pMaterial = new Material(L"..\\Resources\\Shaders\\PostProcessing\\ScreenSpaceCopy.fx", RenderType::postprocessPass);
	MaterialTextureParam MaterialTextureParam{ nullptr, true, "gInputTexture", L"", nullptr };
	m_pMaterial->GetTextureParams().AddMapping(MaterialTextureParam);
	HRESULT hr = m_pMaterial->InitShader(pDevice->GetDevice(), pDevice->GetDeviceContext());

	CreateNDCQuad(&m_pScreenQuadVertexBuffer, &m_pScreenQuadIndexBuffer, m_ScreenQuadNrVerticies, m_ScreenQuadNrIndicies);

	return hr == S_OK;
}
void ScreenSpaceCopy::ReleaseShader()
{
	SafeDelete(m_pMaterial);
}

void ScreenSpaceCopy::RunEffect(EngineDevice* pDevice, Texture* pSource, Texture* pTarget)
{
	V_DX11_ANNOTATE(V_WTEXT("ScreenSpace Copy"));

	ID3D11DeviceContext* pContext = pDevice->GetDeviceContext();

	//Set the material buffers
	{
		const std::string name = "gInputTexture";
		m_pMaterial->GetTextureParams().GetMappingbasedOnName(name)->SetTexture(pSource, true);
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
		((RenderTarget*)pTarget)->GetRenderTargetView()
	};
	pContext->OMSetRenderTargets(1, renderTargs, nullptr);

	m_pMaterial->Render(pDevice->GetDeviceContext(), QUAD_INDICES, 0);

	//Unhook render targets from material
	EngineManager::Instance()->GetActiveRenderer()->ExplicitlyUnbindingRenderTargets(m_pMaterial);
}
