#include "pch.h"
#include "Material.h"
#include "EngineManager.h"
#include "EngineDevice.h"
#include "ShaderStructures.h"
#include "FileManager.h"

Material::Material()
	:m_pVSBlob{}
	, m_pPSBlob{}
	, m_pVS{}
	, m_pPS{}
	, m_pLayout{}
{
}
Material::~Material()
{
	SafeRelease(m_pLayout);
	SafeRelease(m_pPS);
	SafeRelease(m_pVS);
	SafeRelease(m_pPSBlob);
	SafeRelease(m_pVSBlob);
}

void Material::InitShader(ID3D11Device& device, ID3D11DeviceContext& context)
{
	std::wstring fullPath = FileManager::GetAbsoluteExePath();
	fullPath += L"..\\Resources\\Shaders\\Shader.fx";
	LPCWSTR shaderPath = fullPath.c_str();

	D3DX11CompileFromFile(shaderPath, NULL, NULL, "VShader", "vs_4_0", NULL, NULL, NULL, &m_pVSBlob, 0, 0);
	D3DX11CompileFromFile(shaderPath, NULL, NULL, "PShader", "ps_4_0", NULL, NULL, NULL, &m_pPSBlob, 0, 0);

	// encapsulate both shaders into shader objects
	device.CreateVertexShader(m_pVSBlob->GetBufferPointer(), m_pVSBlob->GetBufferSize(), NULL, &m_pVS);
	device.CreatePixelShader(m_pPSBlob->GetBufferPointer(), m_pPSBlob->GetBufferSize(), NULL, &m_pPS);

	// set the shader objects
	context.VSSetShader(m_pVS, 0, 0);
	context.PSSetShader(m_pPS, 0, 0);

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	device.CreateInputLayout(ied, 2, m_pVSBlob->GetBufferPointer(), m_pVSBlob->GetBufferSize(), &m_pLayout);
	context.IASetInputLayout(m_pLayout);
}
