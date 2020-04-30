#include <d3dcompiler.h>
#include <iostream>
#include <DirectXMath.h>
#include "Graphics.h"
#include "SimpleDebug.h"
#include "DXDrawable.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

Graphics::Graphics(HWND hWnd, int screenWidth, int screenHeight) 
	: width(screenWidth), height(screenHeight)
{
	// Create a struct for the settings of the swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;


	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Actually create the swap chain
	SimpleDebug::ProcessHresult(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));

	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
	SimpleDebug::ProcessHresult(pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		nullptr,
		&pTarget
	));

	// Create the depth stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;

	wrl::ComPtr<ID3D11DepthStencilState> pDSState;
	SimpleDebug::ProcessHresult(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));

	// Find the depth stencil state
	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	// Create the depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0u;
	descDepth.MiscFlags = 0u;

	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	SimpleDebug::ProcessHresult(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	// Create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	descDSV.Flags = 0;

	 SimpleDebug::ProcessHresult(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &pDSV));


	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDSV.Get());

	// Configure viewport
	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);

	DXDrawable::SetGraphics(this);
	DXBindable::SetGraphics(this);

	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
}

void Graphics::EndFrame()
{
	pSwap->Present(1u, 0u);
}

void Graphics::ClearBuffer(float red, float green, float blue)
{
	const float color[] = { red, green, blue };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj)
{
	camera.projection = proj;
}
