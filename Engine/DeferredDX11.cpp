#include "pch.h"
#include "DeferredDX11.h"
#include "Material.h"
#include "EngineManager.h"
#include "EngineDevice.h"
#include "ShaderStructures.h"
#include "Component.h"
#include "UIRenderer.h"
#include "EngineSettings.h"
#include "RenderUtils.h"
#include "PostProcesses.h"
#include "RenderAnnotation.h"

DeferredDX11::DeferredDX11(EngineDevice* device) 
	:Renderer(device)
	, m_DeferredRenderTargets{}
	, m_pDeferredLightingMaterial{ nullptr }
	, m_pScreenQuadVertexBuffer{ nullptr }
	, m_pScreenQuadIndexBuffer{ nullptr }
	, m_ScreenQuadNrVerticies{ 0 }
	, m_ScreenQuadNrIndicies{ 0 }
{
	m_RenderType = RenderType::deferred;
}
DeferredDX11::~DeferredDX11() 
{
	SafeRelease(m_pScreenQuadVertexBuffer);
	SafeRelease(m_pScreenQuadIndexBuffer);

	// TO-DO: We have to take special care not to clean this erroniously, terrible
	CleanPointersToTargets();
	SafeDelete(m_pDeferredLightingMaterial);
}

void DeferredDX11::Init() 
{
	Renderer::Init();

	// We can point here already since this is points to a container
	m_DeferredRenderTargets.m_pRenderTargets = &GetRenderTargets();

	// Setup deferred lighting material
	m_pDeferredLightingMaterial = new Material(L"..\\Resources\\Shaders\\DeferredLighting.fx", RenderType::lightingPass);
	std::vector<MaterialTextureParam> TextureParamsMap{
		MaterialTextureParam{ nullptr, true, "gPositionMap", L"", nullptr},
		MaterialTextureParam{ nullptr, true, "gNormalMap", L"", nullptr},
		MaterialTextureParam{ nullptr, true, "gTangentMap", L"", nullptr},
		MaterialTextureParam{ nullptr, true, "gAlbedoMap", L"", nullptr},
		MaterialTextureParam{ nullptr, true, "gMetalRoughnessMap", L"", nullptr} };
	m_pDeferredLightingMaterial->GetTextureParams().AddMap(TextureParamsMap);
	std::vector<MaterialScalarParam> ScalarParamsMap{
		MaterialScalarParam{ nullptr, 1.0f, 0.0f, 2.0f, "gAlbedoMultiply" },
		MaterialScalarParam{ nullptr, 1.0f, 0.0f, 2.0f, "gMetalnessMultiply" },
		MaterialScalarParam{ nullptr, 1.0f, 0.0f, 1.0f, "gRoughnessMultiply" } };
	m_pDeferredLightingMaterial->GetScalarParams().AddMap(ScalarParamsMap);
	m_pDeferredLightingMaterial->InitShader(m_pDevice->GetDevice(), m_pDevice->GetDeviceContext());

	// Create NDC for the deferred passes
	CreateNDCQuad(&m_pScreenQuadVertexBuffer, &m_pScreenQuadIndexBuffer, m_ScreenQuadNrVerticies, m_ScreenQuadNrIndicies);
}

void DeferredDX11::AllocatePromisedTargets()
{
	Renderer::AllocatePromisedTargets();

	// TO-DO: Pack this data better
	// Setup
	D3D11_TEXTURE2D_DESC desc1;
	ZeroMemory(&desc1, sizeof(desc1));
	desc1.Width = m_pDevice->GetDefaultWidth();
	desc1.Height = m_pDevice->GetDefaultHeight();
	desc1.MipLevels = 1;
	desc1.ArraySize = 1;
	desc1.Format = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc1.SampleDesc.Count = 1;
	desc1.SampleDesc.Quality = 0;
	desc1.Usage = D3D11_USAGE_DEFAULT;
	desc1.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc1.CPUAccessFlags = 0;
	desc1.MiscFlags = 0;
	D3D11_TEXTURE2D_DESC desc2;
	ZeroMemory(&desc2, sizeof(desc2));
	desc2.Width = m_pDevice->GetDefaultWidth();
	desc2.Height = m_pDevice->GetDefaultHeight();
	desc2.MipLevels = 1;
	desc2.ArraySize = 1;
	desc2.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8_UNORM;
	desc2.SampleDesc.Count = 1;
	desc2.SampleDesc.Quality = 0;
	desc2.Usage = D3D11_USAGE_DEFAULT;
	desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc2.CPUAccessFlags = 0;
	desc2.MiscFlags = 0;

	m_DeferredRenderTargets.m_PositionTarget = m_pDevice->TryGetRenderTarget(true, desc1);
	m_DeferredRenderTargets.m_NormalTarget = m_pDevice->TryGetRenderTarget(true, desc1);
	m_DeferredRenderTargets.m_TangentTarget = m_pDevice->TryGetRenderTarget(true, desc1);
	m_DeferredRenderTargets.m_AlbedoTarget = m_pDevice->TryGetRenderTarget(true, desc1);
	m_DeferredRenderTargets.m_MetalRoughnessTarget = m_pDevice->TryGetRenderTarget(true, desc2);

	MaterialTextureParamsMapping& materialTextureParamsMapping = m_pDeferredLightingMaterial->GetTextureParams();
	materialTextureParamsMapping.GetMappingbasedOnName("gPositionMap")->SetTexture(m_DeferredRenderTargets.m_PositionTarget, true);
	materialTextureParamsMapping.GetMappingbasedOnName("gNormalMap")->SetTexture(m_DeferredRenderTargets.m_NormalTarget, true);
	materialTextureParamsMapping.GetMappingbasedOnName("gTangentMap")->SetTexture(m_DeferredRenderTargets.m_TangentTarget, true);
	materialTextureParamsMapping.GetMappingbasedOnName("gAlbedoMap")->SetTexture(m_DeferredRenderTargets.m_AlbedoTarget, true);
	materialTextureParamsMapping.GetMappingbasedOnName("gMetalRoughnessMap")->SetTexture(m_DeferredRenderTargets.m_MetalRoughnessTarget, true);
}
void DeferredDX11::CleanPointersToTargets()
{
	Renderer::CleanPointersToTargets();

	MarkTargetsAsNotUsed();

	MaterialTextureParamsMapping& materialTextureParamsMapping = m_pDeferredLightingMaterial->GetTextureParams();
	materialTextureParamsMapping.GetMappingbasedOnName("gPositionMap")->SetTexture(nullptr, true);
	materialTextureParamsMapping.GetMappingbasedOnName("gNormalMap")->SetTexture(nullptr, true);
	materialTextureParamsMapping.GetMappingbasedOnName("gTangentMap")->SetTexture(nullptr, true);
	materialTextureParamsMapping.GetMappingbasedOnName("gAlbedoMap")->SetTexture(nullptr, true);
	materialTextureParamsMapping.GetMappingbasedOnName("gMetalRoughnessMap")->SetTexture(nullptr, true);
}
void DeferredDX11::MarkTargetsAsNotUsed() 
{
	// We make open up our targets once more for re-use
	// They won't be removed from the pool next frame, since they were used at least once
	m_DeferredRenderTargets.m_PositionTarget->MarkAsUnused();
	m_DeferredRenderTargets.m_NormalTarget->MarkAsUnused();
	m_DeferredRenderTargets.m_TangentTarget->MarkAsUnused();
	m_DeferredRenderTargets.m_AlbedoTarget->MarkAsUnused();
	m_DeferredRenderTargets.m_MetalRoughnessTarget->MarkAsUnused();
}

void DeferredDX11::Render()
{
	// No camera == no rendering
	if (!m_pRenderingCamera)
	{
		return;
	}

	// TO-DO: Find a way to just request memory targets and then clear them in a tastfull manner
	ClearBuffers();

	{
		V_DX11_ANNOTATE(V_WTEXT("Scene"));

		//**********************************************************************
		// 	   Main render loop
		//**********************************************************************
		for (int i = 0; i < m_Renderables.size(); ++i)
		{
			if (m_Renderables[i]->IsVisible())
			{
				Material* pMaterial = m_Renderables[i]->GetMaterial();
				// Set the parameters
				pMaterial->UpdateParameterValues(m_pDevice);
				// Update matrices
				UpdateMatrices(m_Renderables[i]);
				// Set the render target
				SetupTargetsDeferredFirstPass();
				// First pass
				m_Renderables[i]->Render(m_pDevice->GetDevice(), m_pDevice->GetDeviceContext(), 0);
			}
		}

		// Unhook render targets from material
		ExplicitlyUnbindingRenderTargets();
	}

	{
		V_DX11_ANNOTATE(V_WTEXT("Deferred"));

		//**********************************************************************
		// 	   Final Render for deferred
		//**********************************************************************
		{
			//Set the material buffers
			m_pDeferredLightingMaterial->UpdateParameterValues(m_pDevice);
			if (m_UpdateLighting)
			{
				// Update the lights
				UpdateLights(m_pDeferredLightingMaterial);
			}
			// Update view matrix
			UpdateMatrices(m_pDeferredLightingMaterial);
			// Set the render target
			SetupTargetsDeferredSecondPass();
			// Second/Deferred pass
			// Select which vertex buffer to display
			const UINT stride = sizeof(QuadVertex);
			const UINT offset = 0;
			ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
			pContext->IASetVertexBuffers(0, 1, &m_pScreenQuadVertexBuffer, &stride, &offset);

			// Set the index buffer to active in the input assembler so it can be rendered.
			pContext->IASetIndexBuffer(m_pScreenQuadIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

			// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
			pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_pDeferredLightingMaterial->Render(pContext, m_ScreenQuadNrIndicies, 0);

			// Unhook render targets from material
			ExplicitlyUnbindingRenderTargets(m_pDeferredLightingMaterial);
		}

		MarkTargetsAsNotUsed();
	}

	//**********************************************************************
	// 	   Postprocess
	//**********************************************************************
	{
		V_DX11_ANNOTATE(V_WTEXT("Postprocesses"));

		m_pPostProcessingPipeline->RunPostProcesses(m_pDevice, this);
	}

	//**********************************************************************
	// 	   Final present + UI (ImGui)
	//**********************************************************************
	// Reset bool here as it can be flipped by UI interactions
	m_UpdateLighting = false;
	{
		V_DX11_ANNOTATE(V_WTEXT("UI"));

		ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
		ID3D11RenderTargetView* renderTargs[] =
		{
			GetFinalTarget()->GetRenderTargetView()
		};
		pContext->OMSetRenderTargets(1, renderTargs, nullptr);
		UIRenderer::Instance()->RenderUI();
	}

	// Switch the back buffer and the front buffer
	m_pDevice->GetSwapChain()->Present(EngineSettings::Instance()->GetIfVSync(), 0);
}
void DeferredDX11::ClearBuffers()
{
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	// Clear the main target buffer to a deep blue
	pContext->ClearRenderTargetView(GetFinalSceneTarget()->GetRenderTargetView(), &m_DefaultClearColor.x);
	// Clear the other buffers as well with black
	const DirectX::XMFLOAT4 black = { 0,0,0,0 };
	pContext->ClearRenderTargetView(GetFinalTarget()->GetRenderTargetView(), &black.x);
	pContext->ClearRenderTargetView(m_DeferredRenderTargets.m_PositionTarget->GetRenderTargetView(), &black.x);
	pContext->ClearRenderTargetView(m_DeferredRenderTargets.m_NormalTarget->GetRenderTargetView(), &black.x);
	pContext->ClearRenderTargetView(m_DeferredRenderTargets.m_TangentTarget->GetRenderTargetView(), &black.x);
	pContext->ClearRenderTargetView(m_DeferredRenderTargets.m_AlbedoTarget->GetRenderTargetView(), &black.x);
	pContext->ClearRenderTargetView(m_DeferredRenderTargets.m_MetalRoughnessTarget->GetRenderTargetView(), &black.x);
	// Clear the depth buffer to the max value
	pContext->ClearDepthStencilView(m_pDevice->GetDepthBuffer(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

#pragma region RenderHelpers
void DeferredDX11::SetupTargetsDeferredFirstPass()
{
	// Set the render target
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	ID3D11RenderTargetView* renderTargs[] =
	{
		// Used for storing position, normals etc.
		m_DeferredRenderTargets.m_PositionTarget->GetRenderTargetView(),
		m_DeferredRenderTargets.m_NormalTarget->GetRenderTargetView(),
		m_DeferredRenderTargets.m_TangentTarget->GetRenderTargetView(),
		m_DeferredRenderTargets.m_AlbedoTarget->GetRenderTargetView(),
		m_DeferredRenderTargets.m_MetalRoughnessTarget->GetRenderTargetView(),
	};
	pContext->OMSetRenderTargets(m_NrBasisRenderTargets, renderTargs, m_pDevice->GetDepthBuffer());
}
void DeferredDX11::SetupTargetsDeferredSecondPass()
{
	// Set the render target
	ID3D11DeviceContext* pContext = m_pDevice->GetDeviceContext();
	ID3D11RenderTargetView* renderTargs[] =
	{
		GetFinalSceneTarget()->GetRenderTargetView()
	};
	pContext->OMSetRenderTargets(1, renderTargs, nullptr);
}
#pragma endregion

Material* DeferredDX11::GetDeferredLightingPassMaterial() const
{
	return m_pDeferredLightingMaterial;
}
