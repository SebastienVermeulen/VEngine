#include "pch.h"
#include "Material.h"
#include "EngineManager.h"
#include "EngineDevice.h"
#include "FileManager.h"
#include "Texture.h"
#include "Light.h"
#include "MaterialWidget.h"

Material::Material(const std::wstring& effectFile)
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
	, m_RenderType{RenderType::deferred}
{
	//Setup the texture params
	m_MaterialTextureParams.reserve(6);
	m_MaterialTextureParams.push_back(new MaterialTextureParam{nullptr, nullptr, "gPositionMap", L""});
	m_MaterialTextureParams.push_back(new MaterialTextureParam{nullptr, nullptr, "gNormalMap", L""});
	m_MaterialTextureParams.push_back(new MaterialTextureParam{nullptr, nullptr, "gTangentMap", L""});
	m_MaterialTextureParams.push_back(new MaterialTextureParam{nullptr, nullptr, "gAlbedoMap", L"..\\Resources\\Textures\\ShaderBallAlbedo.dds" });
	m_MaterialTextureParams.push_back(new MaterialTextureParam{nullptr, nullptr, "gMetalnessMap", L"..\\Resources\\Textures\\ShaderBallMetalness.dds" });
	m_MaterialTextureParams.push_back(new MaterialTextureParam{nullptr, nullptr, "gRoughnessMap", L"..\\Resources\\Textures\\ShaderBallRoughness.dds" });

	m_MaterialScalarParams.reserve(4);
	m_MaterialScalarParams.push_back(new MaterialScalarParam{nullptr, 1.0f, 0.0f, 2.0f, "gAlbedoMultiply"});
	m_MaterialScalarParams.push_back(new MaterialScalarParam{nullptr, 1.0f, 0.0f, 2.0f, "gMetalnessMultiply"});
	m_MaterialScalarParams.push_back(new MaterialScalarParam{nullptr, 1.0f, 0.0f, 1.0f, "gRoughnessMultiply"});
}
Material::~Material()
{
	SafeDelete(m_pWidget);

	//Lights
	SafeRelease(m_LightsBuffer);
	//Parameters
	SafeDelete(m_MaterialTextureParams);
	SafeDelete(m_MaterialScalarParams);
	//Matrix
	SafeRelease(m_pWorldViewProj);
	SafeRelease(m_pProjection);
	SafeRelease(m_pView);
	SafeRelease(m_pWorld);
	//Effect
	SafeRelease(m_pLayouts);
	SafeRelease(m_pTechnique);
	SafeRelease(m_pEffect);
}

void Material::InitShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{	
	HRESULT hr;

	hr = InitEffect(pDevice);
	if (hr != S_OK)
	{
		//TO-DO: Add logger
		return;
	}

	hr = InitInputLayout(pDevice);
	if (hr != S_OK)
	{
		//TO-DO: Add logger
		return;
	}

	hr = InitShaderVariables(pDevice);
	if (hr != S_OK)
	{
		//TO-DO: Add logger
		return;
	}
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

	return D3DX11CompileEffectFromFile(shaderPath,
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&m_pEffect,
		&pErrorBlob);
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
	for (uint32_t passNr = 0; passNr < techDesc.Passes; passNr++)
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
			//TO-DO: make logger
			return hr;
		}
	}

	return S_OK;
}
HRESULT Material::InitShaderVariables(ID3D11Device* pDevice)
{
	//Creating matrix parameters
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

	//Textures
	for (int idx = 0; idx < m_MaterialTextureParams.size(); idx++) 
	{
		m_MaterialTextureParams[idx]->m_Resource = m_pEffect->GetVariableByName(m_MaterialTextureParams[idx]->m_Name.c_str())->AsShaderResource();
		if (m_MaterialTextureParams[idx]->m_Resource && m_MaterialTextureParams[idx]->m_LocalFilePath.length())
		{
			//Load the texture
			m_MaterialTextureParams[idx]->m_pTexture = FileManager::LoadDDS(pDevice, m_MaterialTextureParams[idx]->m_LocalFilePath);
			//If successfully loaded, set the texture
			if (m_MaterialTextureParams[idx]->m_pTexture)
			{
				m_MaterialTextureParams[idx]->m_Resource->SetResource(m_MaterialTextureParams[idx]->m_pTexture->GetTextureView());
			}
			else
			{
				//TO-DO: Add logger
				return E_FAIL;
			}
		}
	}
	//Scalars
	for (int idx = 0; idx < m_MaterialScalarParams.size(); idx++)
	{
		m_MaterialScalarParams[idx]->m_Resource = m_pEffect->GetVariableByName(m_MaterialScalarParams[idx]->m_Name.c_str())->AsScalar();
	}

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
	HRESULT hr = pDevice->CreateBuffer(&cbDesc, NULL, &m_LightsBuffer);
	if (FAILED(hr))
	{
		//TO-DO: Add logger
		return hr;
	}

	return hr;
}

void Material::Render(ID3D11DeviceContext* pContext, UINT nrIndices, int passNr)
{
	//Safety check
	if (m_pLayouts.size() <= passNr)
	{
		//TO-DO: make logger
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
	//Skip setting the textures if it is not a lighting pass, since those will be used by rendertargs
	if (m_RenderType == RenderType::lightingPass)
	{
		for (int idx = 0; idx < m_MaterialTextureParams.size(); idx++)
		{
			if (m_MaterialTextureParams[idx]->m_Resource && m_MaterialTextureParams[idx]->m_Resource->IsValid())
			{
				m_MaterialTextureParams[idx]->m_Resource->SetResource(pEngineDevice->TryGetRenderTarget(idx + 1, false)->pShaderResourceView);
			}
			else
			{
				//TO-DO: Add logger
				return E_FAIL;
			}
		}
	}
	//Scalars
	for (int idx = 0; idx < m_MaterialScalarParams.size(); idx++)
	{
		if (m_MaterialScalarParams[idx]->m_Resource && m_MaterialScalarParams[idx]->m_Resource->IsValid())
		{
			m_MaterialScalarParams[idx]->m_Resource->SetFloat(m_MaterialScalarParams[idx]->m_Value);
		}
	}

	return S_OK;
}

MaterialWidget* Material::GetWidget() const
{
	return m_pWidget; 
}
