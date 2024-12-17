#pragma once

struct WindowSettings;
class RenderTarget;
class DepthStencil;
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

#pragma region RenderTargets
	/// <summary>
	/// The swapchain target is always at the same place in memory.
	/// </summary>
	inline RenderTarget* GetSwapChainTarget() { return m_RenderTargets[0]; }
	/// <summary>
	/// Function if you don't know if the target exists if not make a new one.
	/// </summary>
	RenderTarget* TryGetRenderTarget(bool customDesc, D3D11_TEXTURE2D_DESC desc = {});
	/// <summary>
	/// Make a new render target, the index of which is passed back via the int reference.
	/// If no parameters are given defaults will be used.
	/// </summary>
	RenderTarget* GetNewRenderTarget(int& index, bool customDesc, D3D11_TEXTURE2D_DESC desc = {});

	bool CompareTargetFormats(RenderTarget* pTarget, D3D11_TEXTURE2D_DESC desc);

	/// <summary>
	/// This also can delete targets if not used properly
	/// Don't call this multiple times per frame
	/// </summary>
	void UpdateTargetLifeTimes(float deltaTime);

	void ReleaseTarget(RenderTarget*);
#pragma endregion

#pragma region DepthBuffers
	/// <summary>
	/// Function if you don't know if the stencil exists if not make a new one.
	/// </summary>
	DepthStencil* TryGetDepthStencil(bool customDesc, D3D11_TEXTURE2D_DESC desc = {});
	/// <summary>
	/// Make a new Depth Stencil, the index of which is passed back via the int reference.
	/// If no parameters are given defaults will be used.
	/// </summary>
	DepthStencil* GetNewDepthStencil(int& index, bool customDesc, D3D11_TEXTURE2D_DESC desc = {});

	bool CompareStencilFormats(DepthStencil* pStencil, D3D11_TEXTURE2D_DESC desc);

	/// <summary>
	/// This also can delete stencils if not used properly
	/// Don't call this multiple times per frame
	/// </summary>
	void UpdateStencilLifeTimes(float deltaTime);

	void ReleaseStencil(DepthStencil*);
#pragma endregion

#pragma region VertexBuffers/IndexBuffers
	HRESULT CreateVertexBuffer(std::vector<Vertex>& vertices, ID3D11Buffer** ppVertexBuffer) const;
	HRESULT CreateIndexBuffer(std::vector<int>& indices, ID3D11Buffer** ppIndexBuffer) const;
#pragma endregion

	inline UINT GetDefaultWidth() const { return m_DefaultWidth; }
	inline UINT GetDefaultHeight() const { return m_DefaultHeight; }

private:
#pragma region RenderTargets/DepthBuffers
	RenderTarget* FindAvailableTarget(bool customDesc, D3D11_TEXTURE2D_DESC desc);

	DepthStencil* FindAvailableStencil(bool customDesc, D3D11_TEXTURE2D_DESC desc);
#pragma endregion

#pragma region DirectX
	IDXGISwapChain* m_pSwapChain;			// The pointer to the swap chain interface
	ID3D11Device* m_pDevice;				// The pointer to our Direct3D device interface
	ID3D11DeviceContext* m_pDeviceContext;	// The pointer to our Direct3D device context
#pragma endregion

#pragma region RenderTargets/DepthBuffers
	// TO-DO: Make resising possible
	std::vector<RenderTarget*> m_RenderTargets;		// The pointer to our backbuffer, now as the render target
	//TO-DO: Remove stensil from these names, since they are just buffers
	std::vector<DepthStencil*> m_DepthStencils;
#pragma endregion

	UINT m_DefaultWidth, m_DefaultHeight;

	// TO-DO: Put this in a better place
	float m_MaxUnusedTargetLifeTime = 0.1f;
};
