#include "pch.h"
#include "Material.h"
#include "EngineManager.h"
#include "EngineDevice.h"
#include "FileManager.h"
#include "Texture.h"
#include "Light.h"
#include "MaterialWidget.h"

Material::Material(const std::wstring& effectFile, RenderType type)
	:m_pWidget{new MaterialWidget(this)}
	, m_EffectFile{effectFile}
	, m_pEffect{}
	, m_TechniqueName{}
	, m_pTechnique{}
	, m_pWorld{}
	, m_pView{}
	, m_pInverseView{}
	, m_pProjection{}
	, m_pWorldViewProj{}
	, m_LightsBuffer{}
	, m_BufferVariable{}
	, m_pLayouts{}
	, m_pInputLayoutSize{}
	, m_pNrBasisRenderTargets{3}
	, m_RenderType{ type }
	, m_Initialized{ false }
{
	// TO-DO: Make it a bit more obvious the params need to be setup before calling init
}
Material::~Material()
{
	SafeDelete(m_pWidget);

	// Lights
	SafeRelease(m_LightsBuffer);
	//Parameters
	m_MaterialTextureParamsMapping.Empty();
	m_MaterialScalarParamsMapping.Empty();
	// Matrix
	SafeRelease(m_pWorldViewProj);
	SafeRelease(m_pProjection);
	SafeRelease(m_pView);
	SafeRelease(m_pWorld);
	// Effect
	SafeRelease(m_pLayouts);
	SafeRelease(m_pTechnique);
	SafeRelease(m_pEffect);
}

HRESULT Material::InitShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{	
	HRESULT hr;

	hr = InitEffect(pDevice);
	if (hr != S_OK)
	{
		V_LOG(LogVerbosity::Warning, V_WTEXT("Material: Failed to InitEffect for %s.", FileManager::GetFileName(m_EffectFile)));
		return hr;
	}

	hr = InitInputLayout(pDevice);
	if (hr != S_OK)
	{
		V_LOG(LogVerbosity::Warning, V_WTEXT("Material: Failed to InitInputLayout for %s.", FileManager::GetFileName(m_EffectFile)));
		return hr;
	}

	hr = InitShaderVariables(pDevice);
	if (hr != S_OK)
	{
		V_LOG(LogVerbosity::Warning, V_WTEXT("Material: Failed to InitShaderVariables for %s.", FileManager::GetFileName(m_EffectFile)));
		return hr;
	}

	m_Initialized = true;

	return S_OK;
}
HRESULT Material::InitEffect(ID3D11Device* pDevice)
{
	ID3D10Blob* pErrorBlob = nullptr;

	DWORD shaderFlags = 0;
#if defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	std::wstring fullPath = FileManager::GetAbsoluteExePath();
	fullPath += m_EffectFile;
	LPCWSTR shaderPath = fullPath.c_str();

	HRESULT hr = D3DX11CompileEffectFromFile(shaderPath,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		shaderFlags,
		0,
		pDevice,
		&m_pEffect,
		&pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}

		if (m_pEffect)
			m_pEffect->Release();
	}
	return hr;
}
HRESULT Material::InitInputLayout(ID3D11Device* pDevice)
{
	HRESULT hr;

	if (!m_TechniqueName.empty())
	{
		m_pTechnique = m_pEffect->GetTechniqueByName(UTF8Encode(m_TechniqueName).c_str());
	}
	else
	{
		m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
	}

	//Assemble the input layout
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);

	m_pLayouts.resize(techDesc.Passes);
	for (uint32_t passNr = 0; passNr < techDesc.Passes; ++passNr)
	{
		D3DX11_PASS_SHADER_DESC passShaderDesc;
		m_pTechnique->GetPassByIndex(passNr)->GetVertexShaderDesc(&passShaderDesc);

		D3DX11_EFFECT_SHADER_DESC effectShaderDesc;
		passShaderDesc.pShaderVariable->GetShaderDesc(passShaderDesc.ShaderIndex, &effectShaderDesc);

		D3D11_SIGNATURE_PARAMETER_DESC signParDesc;
		std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc;

		for (UINT i = 0; i < effectShaderDesc.NumInputSignatureEntries; ++i)
		{
			passShaderDesc.pShaderVariable->GetInputSignatureElementDesc(passShaderDesc.ShaderIndex, i, &signParDesc);
			 
			//Semantic Format
			DXGI_FORMAT format;
			switch (signParDesc.ComponentType)
			{
			case D3D10_REGISTER_COMPONENT_FLOAT32:
				if (signParDesc.Mask == 1)		format = DXGI_FORMAT_R32_FLOAT;
				else if (signParDesc.Mask == 3)	format = DXGI_FORMAT_R32G32_FLOAT;
				else if (signParDesc.Mask == 7)	format = DXGI_FORMAT_R32G32B32_FLOAT;
				else							format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			case D3D10_REGISTER_COMPONENT_UINT32:
				if (signParDesc.Mask == 1)		format = DXGI_FORMAT_R32_UINT;
				else if (signParDesc.Mask == 3)	format = DXGI_FORMAT_R32G32_UINT;
				else if (signParDesc.Mask == 7)	format = DXGI_FORMAT_R32G32B32_UINT;
				else							format = DXGI_FORMAT_R32G32B32A32_UINT;
				break;
			case D3D10_REGISTER_COMPONENT_SINT32:
				if (signParDesc.Mask == 1)		format = DXGI_FORMAT_R32_SINT;
				else if (signParDesc.Mask == 3)	format = DXGI_FORMAT_R32G32_SINT;
				else if (signParDesc.Mask == 7)	format = DXGI_FORMAT_R32G32B32_SINT;
				else							format = DXGI_FORMAT_R32G32B32A32_SINT;
				break;
			default:
				//Unsupported Component Type!
				break;
			}

			//Create Input Element Desc
			D3D11_INPUT_ELEMENT_DESC inputLayout = { signParDesc.SemanticName, signParDesc.SemanticIndex, format, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			layoutDesc.push_back(inputLayout);
		}
		
		//Create the input layout
		D3DX11_PASS_DESC PassDesc;
		//Get the pass decriptor from the effect technique
		m_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
		hr = pDevice->CreateInputLayout(static_cast<D3D11_INPUT_ELEMENT_DESC*>(&layoutDesc[0]), (UINT)layoutDesc.size(), PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pLayouts[0]);
		if (!SUCCEEDED(hr))
		{
			V_LOG(LogVerbosity::Warning, V_WTEXT("Material: Failed to CreateInputLayout for %s.", FileManager::GetFileName(m_EffectFile)));
			return hr;
		}
	}

	return S_OK;
}
HRESULT Material::InitShaderVariables(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;

	//Creating matrix parameters
	// TO-DO: Make these into pairs or a dictionary or something... So we can assign which ones should be made during init
	if (m_RenderType != RenderType::postprocessPass) // TO-DO: temporary but dirty, see above todo
	{
		ID3DX11EffectVariable* pEffectVar = m_pEffect->GetVariableBySemantic("WORLD");
		m_pWorld = (pEffectVar->IsValid()) ? pEffectVar->AsMatrix() : nullptr;
		pEffectVar = m_pEffect->GetVariableBySemantic("VIEW");
		m_pView = (pEffectVar->IsValid()) ? pEffectVar->AsMatrix() : nullptr;
		pEffectVar = m_pEffect->GetVariableBySemantic("INVERSEVIEW");
		m_pInverseView = (pEffectVar->IsValid()) ? pEffectVar->AsMatrix() : nullptr;
		pEffectVar = m_pEffect->GetVariableBySemantic("PROJECTION");
		m_pProjection = (pEffectVar->IsValid()) ? pEffectVar->AsMatrix() : nullptr;
		pEffectVar = m_pEffect->GetVariableBySemantic("WORLDVIEWPROJECTION");
		m_pWorldViewProj = (pEffectVar->IsValid()) ? pEffectVar->AsMatrix() : nullptr;
	}

	// Textures
	auto& textureParams = m_MaterialTextureParamsMapping.m_MaterialTextureParams;
	for (int idx = 0; idx < textureParams.size(); ++idx)
	{
		// Pull the variable from the shader
		ID3DX11EffectVariable* pEffectVariable = m_pEffect->GetVariableByName(textureParams[idx].m_Name.c_str());
		if (!pEffectVariable->IsValid()) 
		{
			return E_FAIL;
		}
		textureParams[idx].m_Resource = pEffectVariable->AsShaderResource();

		// Don't overwrite rendertarget pointers
		if (textureParams[idx].m_HoldsRendertarget)
		{
			continue;
		}

		// Load the texture
		if (textureParams[idx].m_Resource && textureParams[idx].m_LocalFilePath.length())
		{
			
			textureParams[idx].SetTexture(FileManager::LoadDDS(pDevice, textureParams[idx].m_LocalFilePath), false);
			// If successfully loaded, set the texture
			if (textureParams[idx].GetTexture())
			{
				textureParams[idx].m_Resource->SetResource(textureParams[idx].GetTexture()->GetResourceView());
			}
			else
			{
				V_LOG(LogVerbosity::Warning, V_WTEXT("Material: Failed to SetResource for %s, since the texture was not created.", FileManager::GetFileName(m_EffectFile)));
				return E_FAIL;
			}
		}
	}
	// Scalars
	auto& scalarParams = m_MaterialScalarParamsMapping.m_MaterialScalarParams;
	for (int idx = 0; idx < scalarParams.size(); ++idx)
	{
		scalarParams[idx].m_Resource = m_pEffect->GetVariableByName(scalarParams[idx].m_Name.c_str())->AsScalar();
	}

	if (m_RenderType == RenderType::forwards || m_RenderType == RenderType::lightingPass)
	{
		m_BufferVariable = m_pEffect->GetConstantBufferByName("Lights");

		//Fill in a buffer description.
		const int maxNrLights = Light::GetMaxNrLights();
		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = sizeof(ShaderLight) * maxNrLights;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;

		//Create the buffer.
		hr = pDevice->CreateBuffer(&cbDesc, NULL, &m_LightsBuffer);
		if (FAILED(hr))
		{
			V_LOG(LogVerbosity::Warning, V_WTEXT("Material: Failed to create a constant lightbuffer for %s.", FileManager::GetFileName(m_EffectFile)));
			return hr;
		}
	}

	return hr;
}

void Material::Render(ID3D11DeviceContext* pContext, UINT nrIndices, int passNr)
{
	//Safety check
	if (m_pLayouts.size() <= passNr)
	{
		V_LOG(LogVerbosity::Warning, V_WTEXT("Material: The requested pass index is out of bounds for %s.", FileManager::GetFileName(m_EffectFile)));
		return;
	}

	//Set the vertex input layout.
	pContext->IASetInputLayout(m_pLayouts[passNr]);

	m_pTechnique->GetPassByIndex(passNr)->Apply(0, pContext);
	pContext->DrawIndexed(nrIndices, 0, 0);
}

void Material::UpdateMatrix(MatrixRenderBuffer buffer)
{
	if (m_pWorld)
	{
		m_pWorld->SetMatrix(reinterpret_cast<float*>(&buffer.world));
	}
	if (m_pView)
	{
		m_pView->SetMatrix(reinterpret_cast<float*>(&buffer.view));
	}	
	if (m_pInverseView)
	{
		m_pInverseView->SetMatrix(reinterpret_cast<float*>(&buffer.inverseView));
	}
	if (m_pProjection)
	{
		m_pProjection->SetMatrix(reinterpret_cast<float*>(&buffer.projection));
	}
	if (m_pWorldViewProj)
	{
		m_pWorldViewProj->SetMatrix(reinterpret_cast<float*>(&buffer.worldViewProj));
	}
}
void Material::UpdateMaterialLighting(ID3D11DeviceContext* pContext, std::vector<ShaderLight> lights)
{
	if (m_BufferVariable && m_LightsBuffer)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		//Disable GPU access to the vertex buffer data.
		pContext->Map(m_LightsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		//Update the vertex buffer here.
		memcpy(mappedResource.pData, &lights[0], sizeof(ShaderLight) * Light::GetMaxNrLights());
		//Reenable GPU access to the vertex buffer data.
		pContext->Unmap(m_LightsBuffer, 0);

		m_BufferVariable->SetConstantBuffer(m_LightsBuffer);
	}
}
HRESULT Material::UpdateParameterValues(EngineDevice* pEngineDevice)
{
	if (!m_Initialized)
	{
		return E_FAIL;
	}

	// Textures
	auto& textureParams = m_MaterialTextureParamsMapping.m_MaterialTextureParams;
	for (int idx = 0; idx < textureParams.size(); ++idx)
	{
		if (!textureParams[idx].m_Resource || !textureParams[idx].m_Resource->IsValid())
		{
			V_LOG(LogVerbosity::Warning, V_WTEXT("Material:", m_EffectFile, ", we couldn't update textureparam: ", textureParams[idx].m_Name, ", since its resource was invalid."));
			continue;
		}

		if (!textureParams[idx].GetTexture())
		{
			V_LOG(LogVerbosity::Warning, V_WTEXT("Material: We couldn't update textureparam: ", textureParams[idx].m_Name, ", since its there was no texture assigned."));
			continue;
		}

		textureParams[idx].m_Resource->SetResource(textureParams[idx].GetTexture()->GetResourceView());
	}

	// Scalars
	auto& scalarParams = m_MaterialScalarParamsMapping.m_MaterialScalarParams;
	for (int idx = 0; idx < scalarParams.size(); ++idx)
	{
		if (!scalarParams[idx].m_Resource || !scalarParams[idx].m_Resource->IsValid())
		{
			V_LOG(LogVerbosity::Warning, V_WTEXT("Material:", m_EffectFile, ", we couldn't update scalarparam: ", scalarParams[idx].m_Name, ", since its resource was invalid."));
			continue;
		}

		scalarParams[idx].m_Resource->SetFloat(scalarParams[idx].m_Value);
	}

	return S_OK;
}
HRESULT Material::ExplicitlyUnbindingResources(EngineDevice* pEngineDevice)
{
	if (!m_Initialized)
	{
		return E_FAIL;
	}

	auto& textureParams = m_MaterialTextureParamsMapping.m_MaterialTextureParams;
	for (int idx = 0; idx < textureParams.size(); ++idx)
	{
		if (FAILED(textureParams[idx].m_Resource->SetResource(nullptr)))
		{
			return E_FAIL;
		}
	}
	return m_pTechnique->GetPassByIndex(0)->Apply(0, pEngineDevice->GetDeviceContext());
}

MaterialWidget* Material::GetWidget() const
{
	return m_pWidget; 
}
