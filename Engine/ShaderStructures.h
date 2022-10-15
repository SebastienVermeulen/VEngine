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
//Should be used as a bitmask
enum VertexFeatures : UINT 
{
	None = 0,
	P = 1,		//Position
	C = 2,		//Color
	U = 4,		//UV
	N = 8,		//Normal
	T = 16,		//Tangent
	B = 32,		//Binormal
};

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 color;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tangent;
	DirectX::XMFLOAT3 binormal;
	DirectX::XMFLOAT2 uv;

	bool operator==(const Vertex& other)
	{
		return	this->position	== other.position	&&
				this->color		== other.color		&&
				this->normal	== other.normal		&&
				this->tangent	== other.tangent	&&
				this->binormal	== other.binormal	&&
				this->uv		== other.uv;
	}
};
struct QuadVertex
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
