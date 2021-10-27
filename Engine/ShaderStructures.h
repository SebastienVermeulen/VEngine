#pragma once
#include "pch.h"

enum RenderType : UINT
{
	forwards = 0,
	deferred = 1,
	lightingPass = 2
};
enum class LightType : UINT
{
	directional = 0,
	point = 1,
};

struct VertexPNTU 
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT2 uv;
};
struct VertexPU
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 uv;
};

struct MatrixRenderBuffer final
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 inverseView;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4X4 worldViewProj;
};

struct RenderTarget final 
{
	~RenderTarget() 
	{
		SafeRelease(pTexture);
		SafeRelease(pRenderTargetView);
		SafeRelease(pShaderResourceView);
	}

	ID3D11Texture2D* pTexture;
	ID3D11RenderTargetView* pRenderTargetView;
	ID3D11ShaderResourceView* pShaderResourceView;
};

struct ShaderLight final
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 direction;
	DirectX::XMFLOAT3 color;
	float intensity;
	LightType type;
	bool enabled;
	DirectX::XMFLOAT2 padding;
};
