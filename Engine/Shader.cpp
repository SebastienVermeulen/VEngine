#include "pch.h"
#include "Shader.h"
#include "EngineManager.h"
#include "EngineDevice.h"

Shader::Shader()
	:m_pVSBlob{}
	, m_pPSBlob{}
	, m_pVS{}
	, m_pPS{}
{
}
Shader::~Shader() 
{
	SafeRelease(m_pPS);
	SafeRelease(m_pVS);
	SafeRelease(m_pPSBlob);
	SafeRelease(m_pVSBlob);
}

void Shader::Init(ID3D11Device& device, ID3D11DeviceContext& context)
{
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &m_pVSBlob, 0, 0);
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &m_pPSBlob, 0, 0);

	// encapsulate both shaders into shader objects
	//ID3D11Device* pDevice = &EngineManager::Instance().GetDevice().GetDevice();
	device.CreateVertexShader(m_pVSBlob->GetBufferPointer(), m_pVSBlob->GetBufferSize(), NULL, &m_pVS);
	device.CreatePixelShader(m_pPSBlob->GetBufferPointer(), m_pPSBlob->GetBufferSize(), NULL, &m_pPS);

	// set the shader objects
	//ID3D11DeviceContext* context = &EngineManager::Instance().GetDevice().GetDeviceContext();
	context.VSSetShader(m_pVS, 0, 0);
	context.PSSetShader(m_pPS, 0, 0);
}
