#pragma once

struct WindowSettings;

class EngineDevice final
{
public:
	EngineDevice();
	~EngineDevice();

	// function prototypes
	void InitD3D(HWND hWnd, const WindowSettings settings);     //Sets up and initializes Direct3D
	void CleanD3D(void);         //Closes Direct3D and releases memory

	inline IDXGISwapChain& GetSwapChain() const { return *m_pSwapChain; }
	inline ID3D11Device& GetDevice() const { return *m_pDevice; }
	inline ID3D11DeviceContext& GetDeviceContext() const { return *m_pDeviceContext; }
	inline ID3D11RenderTargetView& GetBackBuffer() const { return *m_pBackbuffer; }

private:
	IDXGISwapChain* m_pSwapChain; //The pointer to the swap chain interface
	ID3D11Device* m_pDevice; //The pointer to our Direct3D device interface
	ID3D11DeviceContext* m_pDeviceContext; //The pointer to our Direct3D device context
	ID3D11RenderTargetView* m_pBackbuffer; //The pointer to our backbuffer, now as the render target
};
