#pragma once

struct WindowSettings;
struct RenderTarget;
struct Vertex;

class EngineDevice final
{
public:
	EngineDevice();
	~EngineDevice();

	EngineDevice(EngineDevice& other) = delete;
	EngineDevice(EngineDevice&& other) = delete;
	EngineDevice operator=(EngineDevice& other) = delete;
	EngineDevice& operator=(EngineDevice&& other) = delete;

	void InitD3D(HWND hWnd, const WindowSettings settings);     //Sets up and initializes Direct3D
	void CleanD3D(void);         //Closes Direct3D and releases memory

#pragma region DirectX
	inline IDXGISwapChain* GetSwapChain() const { return m_pSwapChain; }
	inline ID3D11Device* GetDevice() const { return m_pDevice; }
	inline ID3D11DeviceContext* GetDeviceContext() const { return m_pDeviceContext; }
#pragma endregion

#pragma region RenderTargets/DepthBuffers
	/// <summary>
	/// Function if you are certain the target exists. If not nullptr is returned.
	/// Index 0 is guaranted to exists by design, but is the final render target (swapchain buffer).
	/// </summary>
	RenderTarget* GetRenderTarget(int index) const;
	/// <summary>
	/// Function if you don't know if the target exists if not make a new one.
	/// WARNING: the given index will not necessarily correspond with the given index.
	/// </summary>
	RenderTarget* TryGetRenderTarget(int index, bool customDesc, D3D11_TEXTURE2D_DESC desc = {});
	/// <summary>
	/// Make a new render target, the index of which is passedback via the uint reference.
	/// If no parameters are given defaults will be used.
	/// </summary>
	RenderTarget* GetNewRenderTarget(int& index, bool customDesc, D3D11_TEXTURE2D_DESC desc = {});
	inline ID3D11DepthStencilView* GetDepthBuffer() const { return m_pDepthStencilView; }

	// TO-DO: Release targets when not used
	HRESULT ReleaseTarget(int index);
#pragma endregion

#pragma region VertexBuffers/IndexBuffers
	HRESULT CreateVertexBuffer(std::vector<Vertex>& vertices, ID3D11Buffer** ppVertexBuffer) const;
	HRESULT CreateIndexBuffer(std::vector<unsigned int>& indices, ID3D11Buffer** ppIndexBuffer) const;
#pragma endregion

	inline UINT GetDefaultWidth() const { return m_DefaultWidth; }
	inline UINT GetDefaultHeight() const { return m_DefaultHeight; }

private:
#pragma region DirectX
	IDXGISwapChain* m_pSwapChain; //The pointer to the swap chain interface
	ID3D11Device* m_pDevice; //The pointer to our Direct3D device interface
	ID3D11DeviceContext* m_pDeviceContext; //The pointer to our Direct3D device context
#pragma endregion

#pragma region RenderTargets/DepthBuffers
	std::vector<RenderTarget*> m_RenderTargets; //The pointer to our backbuffer, now as the render target
	//TO-DO: make it possible to have more than one stencil and buffer
	ID3D11DepthStencilView* m_pDepthStencilView; //The pointer to our depthbuffer
	ID3D11Texture2D* m_pDepthStencilBuffer; //Buffer that holds the depth stencil data
#pragma endregion

	UINT m_DefaultWidth, m_DefaultHeight;
};
