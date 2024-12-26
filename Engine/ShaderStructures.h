#pragma once
#include "pch.h"
// TO-DO: I don't like this inlude
#include "Texture.h"

// TO-DO: Mixing these types is asking for troubles
enum class RenderType : UINT
{
	deferred = 0,
	lightingPass = 1,
	shadowPass = 2,
	postprocessPass = 3
};
enum class LightType : UINT
{
	directional = 0,
	point = 1,
};
//Should be used as a bitmask
enum class VertexFeatures : UINT
{
	None = 0,
	P = 1,		//Position
	U = 4,		//UV
	N = 8,		//Normal
	T = 16,		//Tangent
};

struct Vertex
{
	DirectX::XMFLOAT4 tangent;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;

	bool Compare(const Vertex& other)
	{
		return VMath::Compare(this->position, other.position) &&
			VMath::Compare(this->normal, other.normal) &&
			VMath::Compare(this->tangent, other.tangent) &&
			VMath::Compare(this->uv, other.uv);
	}
};
struct QuadVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 uv;
};

// TO-DO: Make this not hardcoded, have an ID determine which one to update
struct MatrixTransformationContainer final
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 inverseView;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4X4 worldViewProj;

	void MarkAllForUpdate()
	{
		updateWorld = true;
		updateView = true;
		updateInverseView = true;
		updateProjection = true;
		updateWorldViewProjection = true;
	}

	bool updateWorld = false, updateView = false, updateInverseView = false, updateProjection = false, updateWorldViewProjection = false;
};

class RenderTarget final : public Texture
{
public:
	RenderTarget()
		:Texture{ nullptr, nullptr }
		, m_pRenderTargetView{}
		, m_pTexture{}
	{
	}
	~RenderTarget()
	{
		SafeRelease(m_pTexture);
		SafeRelease(m_pRenderTargetView);
	}

	inline ID3D11RenderTargetView** GetRenderTargetViewLocation()
	{
		return (ID3D11RenderTargetView**)&m_pRenderTargetView;
	}
	inline ID3D11RenderTargetView* GetRenderTargetView()
	{
		// TO-DO: This is a neat stopgap to keep targets alive, needs something neater?
		MarkAsUsed();
		return m_pRenderTargetView;
	}	
	inline ID3D11Texture2D** GetTextureLocation()
	{
		return (ID3D11Texture2D**)&m_pTexture;
	}
	inline ID3D11Texture2D* GetTexture()
	{
		// TO-DO: This is a neat stopgap to keep targets alive, needs something neater?
		MarkAsUsed();
		return m_pTexture; 
	}

	// TO-DO: This should have a non-member function as well
	inline bool CompareDesc(const D3D11_TEXTURE2D_DESC& otherDesc) const
	{
		D3D11_TEXTURE2D_DESC targetDesc{};
		m_pTexture->GetDesc(&targetDesc);

		return targetDesc.Width == otherDesc.Width &&
			targetDesc.Height == otherDesc.Height &&
			targetDesc.Format == otherDesc.Format &&
			targetDesc.CPUAccessFlags == otherDesc.CPUAccessFlags &&
			targetDesc.MiscFlags == otherDesc.MiscFlags;
	}

	inline void MarkAsUsed() 
	{
		m_Used = true;
		m_UnusedTimer = 0.0f;
	}
	// TO-DO: Perhaps this would benifit from being automated somehow? (For shaders and such)
	inline void MarkAsUnused() 
	{
		m_Used = false;
	}
	inline bool IsUsed() 
	{
		return m_Used;
	}
	inline float UpdateTimeSpentUnused(float deltaTime) 
	{
		return m_UnusedTimer += deltaTime;
	}

private:
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11Texture2D* m_pTexture;

	float m_UnusedTimer = 0.0f;
	bool m_Used = false;
};

class DepthStencil final : public Texture
{
public:
	DepthStencil()
		:Texture{ nullptr, nullptr }
		, m_pDepthStencilView{}
		, m_pDepthStencilBuffer{}
	{
	}
	~DepthStencil()
	{
		SafeRelease(m_pDepthStencilBuffer);
		SafeRelease(m_pDepthStencilView);
	}

	inline ID3D11DepthStencilView** GetStencilViewLocation()
	{
		return (ID3D11DepthStencilView**)&m_pDepthStencilView;
	}
	inline ID3D11DepthStencilView* GetStencilView()
	{
		// TO-DO: This is a neat stopgap to keep targets alive, needs something neater?
		MarkAsUsed();
		return m_pDepthStencilView;
	}
	inline ID3D11Texture2D** GetStencilBufferLocation()
	{
		return (ID3D11Texture2D**)&m_pDepthStencilBuffer;
	}
	inline ID3D11Texture2D* GetStencilBuffer()
	{
		// TO-DO: This is a neat stopgap to keep targets alive, needs something neater?
		MarkAsUsed();
		return m_pDepthStencilBuffer;
	}

	// TO-DO: This should have a non-member function as well
	inline bool CompareDesc(const D3D11_TEXTURE2D_DESC& otherDesc) const
	{
		D3D11_TEXTURE2D_DESC targetDesc{};
		m_pDepthStencilBuffer->GetDesc(&targetDesc);

		// TO-DO: Check if this description check is good enough
		return targetDesc.Width == otherDesc.Width &&
			targetDesc.Height == otherDesc.Height &&
			targetDesc.Format == otherDesc.Format &&
			targetDesc.CPUAccessFlags == otherDesc.CPUAccessFlags &&
			targetDesc.MiscFlags == otherDesc.MiscFlags;
	}

	inline void MarkAsUsed()
	{
		m_Used = true;
		m_UnusedTimer = 0.0f;
	}
	// TO-DO: Perhaps this would benifit from being automated somehow? (For shaders and such)
	inline void MarkAsUnused()
	{
		m_Used = false;
	}
	inline bool IsUsed()
	{
		return m_Used;
	}
	inline float UpdateTimeSpentUnused(float deltaTime)
	{
		return m_UnusedTimer += deltaTime;
	}

private:
	ID3D11DepthStencilView* m_pDepthStencilView;	// The pointer to our depthbuffer
	ID3D11Texture2D* m_pDepthStencilBuffer;			// Buffer that holds the depth stencil data

	float m_UnusedTimer = 0.0f;
	bool m_Used = false;
};

struct ShaderLight final
{
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 direction;
	DirectX::XMFLOAT3 color;
	float intensity;
	LightType type;
	// We need an index in case not all lights shadowcast and we are offset,
	// That way we can easily identify what target we need to use (only needed for batched lights)
	int shadowIndex; 
	DirectX::XMFLOAT2 padding;
};
