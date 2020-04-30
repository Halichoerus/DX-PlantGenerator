#pragma once
#include "Camera.h"

class Graphics
{
public:
	Graphics(HWND hWnd, int screenWidth, int screenHeight);
	~Graphics() = default;

	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void EndFrame();
	void ClearBuffer(float red, float green, float blue);

	int GetWindowWidth() { return width; }
	int GetWindowHeight() { return height; }
	ID3D11Device* GetDevice() { return pDevice.Get(); };
	ID3D11DeviceContext* GetContext() { return pContext.Get(); }

	Camera& GetCamera() { return camera; };

	void SetProjection(DirectX::FXMMATRIX proj);

private:
	Camera camera;

	int width;
	int height;

	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
};

