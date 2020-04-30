#pragma once
#include "Graphics.h"
#include "Transform.h"
#include "Vertex.h"

class MeshRenderer
{
public:
	MeshRenderer() = default;
	~MeshRenderer() = default;

	static void SetGraphicsDevice(Graphics* graphicsDevice) { graphics = graphicsDevice; }

	void LoadCubeMesh(float size);
	void LoadCuboidMesh(float width, float height, float depth);
	void LoadPlaneMesh(float width, float height);

	void AddTexture(const wchar_t* filePath);

	void Render();
	void Update();

	Transform transform;

private:
	static Graphics* graphics;

	struct ConstantBuffer
	{
		DirectX::XMMATRIX transform;
	};

	void CreateVertexBuffer(const Vertex vertices[], UINT sizeofVertices);
	void CreateIndexBuffer(const unsigned short indices[], UINT sizeofIndices);
	void CreateWorldConstantBuffer();
	void CreateColourConstantBuffer();
	void CreateVertexShader();
	void CreatePixelShader();

	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pWorldConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> pColourConstantBuffer;


	Microsoft::WRL::ComPtr<ID3D11Resource> pTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	Microsoft::WRL::ComPtr <ID3D11SamplerState> pSampler;

	ConstantBuffer constBufferData = {};
	int totalIndicies = 0;
};

