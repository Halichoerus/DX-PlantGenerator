#include <d3dcompiler.h>
#include <iostream>
#include "MeshRenderer.h"
#include "SimpleDebug.h"
#include "WICTextureLoader.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

Graphics* MeshRenderer::graphics = nullptr;

void MeshRenderer::LoadCubeMesh(float size)
{
	const Vertex vertices[] =
	{
		/*
		Vertex(-1, -1,-1,0,0) * size,
		Vertex(1, -1,-1,1,0) * size,
		Vertex(-1, 1,-1,0,1)* size,
		Vertex(1, 1,-1,1,1)* size,
		Vertex(-1, -1,1,0,1)* size,
		Vertex(1, -1,1,1,1 )* size,
		Vertex(-1, 1,1,1,0 )* size,
		Vertex(1, 1,1,0,0 )* size,
		*/

		Vertex(-1, -1,-1,2.0f / 3.0f,0.0f / 4.0f)* size,
		Vertex(1, -1,-1, 1.0f / 3.0f,0.0f / 4.0f)* size,
		Vertex(-1, 1,-1, 2.0f / 3.0f,1.0f / 4.0f)* size,
		Vertex(1, 1,-1,  1.0f / 3.0f,1.0f / 4.0f)* size,
		Vertex(-1, -1,1, 2.0f / 3.0f,3.0f / 4.0f)* size,
		Vertex(1, -1,1,  1.0f / 3.0f,3.0f / 4.0f)* size,
		Vertex(-1, 1,1,  2.0f / 3.0f,2.0f / 4.0f)* size,
		Vertex(1, 1,1,   1.0f / 3.0f,2.0f / 4.0f)* size,
		Vertex(-1, -1,-1,2.0f / 3.0f,4.0f / 4.0f)* size,
		Vertex(1, -1,-1, 1.0f / 3.0f,4.0f / 4.0f)* size,
		Vertex(-1, -1,-1,3.0f / 3.0f,1.0f / 4.0f)* size,
		Vertex(-1, -1,1, 3.0f / 3.0f,2.0f / 4.0f)* size,
		Vertex(1, -1,-1, 0.0f / 3.0f,1.0f / 4.0f)* size,
		Vertex(1, -1,1,  0.0f / 3.0f,2.0f / 4.0f)* size,
	};

	CreateVertexBuffer(vertices,sizeof(vertices));

	const unsigned short indices[] =
	{
			0,2,1,   2,3,1,
				4,8,5,   5,8,9,
				2,6,3,   3,6,7,
				4,5,7,   4,7,6,
				2,10,11, 2,11,6,
				12,3,7,  12,7,13
	};

	CreateIndexBuffer(indices, sizeof(indices));
	CreateWorldConstantBuffer();
	//CreateColourConstantBuffer();
	CreatePixelShader();
	CreateVertexShader();

	totalIndicies = std::size(indices);
}

void MeshRenderer::LoadCuboidMesh(float width, float height, float depth)
{
	const Vertex vertices[] =
	{
		Vertex(-1*width,-1*height,-1*depth,2.0f / 3.0f,0.0f / 4.0f),
		Vertex(1*width, -1*height,-1*depth, 1.0f / 3.0f,0.0f / 4.0f),
		Vertex(-1*width, 1*height,-1*depth, 2.0f / 3.0f,1.0f / 4.0f),
		Vertex(1*width, 1*height,-1*depth,  1.0f / 3.0f,1.0f / 4.0f),
		Vertex(-1*width, -1*height,1*depth, 2.0f / 3.0f,3.0f / 4.0f),
		Vertex(1*width, -1*height,1*depth,  1.0f / 3.0f,3.0f / 4.0f),
		Vertex(-1*width, 1*height,1*depth,  2.0f / 3.0f,2.0f / 4.0f),
		Vertex(1*width, 1*height, 1*depth,   1.0f / 3.0f,2.0f / 4.0f),
		Vertex(-1*width, -1*height,-1*depth,2.0f / 3.0f,4.0f / 4.0f),
		Vertex(1*width, -1*height,-1*depth, 1.0f / 3.0f,4.0f / 4.0f),
		Vertex(-1*width, -1*height,-1*depth,3.0f / 3.0f,1.0f / 4.0f),
		Vertex(-1*width, -1*height,1*depth, 3.0f / 3.0f,2.0f / 4.0f),
		Vertex(1*width, -1*height,-1*depth, 0.0f / 3.0f,1.0f / 4.0f),
		Vertex(1*width, -1*height,1*depth,  0.0f / 3.0f,2.0f / 4.0f),
	};

	CreateVertexBuffer(vertices, sizeof(vertices));

	const unsigned short indices[] =
	{
				0,2,1,   2,3,1,
				4,8,5,   5,8,9,
				2,6,3,   3,6,7,
				4,5,7,   4,7,6,
				2,10,11, 2,11,6,
				12,3,7,  12,7,13
	};

	CreateIndexBuffer(indices, sizeof(indices));
	CreateWorldConstantBuffer();
	CreateColourConstantBuffer();
	CreatePixelShader();
	CreateVertexShader();

	totalIndicies = std::size(indices);
}

void MeshRenderer::LoadPlaneMesh(float width, float height)
{
	const Vertex vertices[] =
	{
		Vertex(-1 * width, -1 * height,0, 0, 1),
		Vertex(width, -1 * height,0, 1, 1),
		Vertex(-1 * width, height,0, 0, 0),
		Vertex(width, height,0, 1, 0),
	};


	CreateVertexBuffer(vertices, sizeof(vertices));

	const unsigned short indices[] =
	{
				0,2,1,   2,3,1,

	};

	CreateIndexBuffer(indices, sizeof(indices));
	CreateWorldConstantBuffer();
	CreateColourConstantBuffer();
	CreatePixelShader();
	CreateVertexShader();

	graphics->GetContext()->IASetInputLayout(pInputLayout.Get());
	totalIndicies = std::size(indices);
}

void MeshRenderer::AddTexture(const wchar_t* filePath)
{
	SimpleDebug::ProcessHresult(CreateWICTextureFromFile(graphics->GetDevice(), filePath, &pTexture, &pTextureView));


	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	SimpleDebug::ProcessHresult(graphics->GetDevice()->CreateSamplerState(&samplerDesc, &pSampler));
}


void MeshRenderer::Render()
{	
	// Bind the vertex buffer to the pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	graphics->GetContext()->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
	// Bind index buffer
	graphics->GetContext()->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
	// Bind constant buffer to vertex shader
	graphics->GetContext()->VSSetConstantBuffers(0u, 1u, pWorldConstantBuffer.GetAddressOf());
	// bind constant buffer to pixel shader
	if(pColourConstantBuffer)
		graphics->GetContext()->PSSetConstantBuffers(0u, 1u, pColourConstantBuffer.GetAddressOf());
	// Bind Vertex shader to the pipline
	graphics->GetContext()->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	// Bind Pixel shader to the pipline
	graphics->GetContext()->PSSetShader(pPixelShader.Get(), nullptr, 0u);


	if(pTextureView)
		graphics->GetContext()->PSSetShaderResources(0u, 1u, pTextureView.GetAddressOf());
	if (pSampler)
		graphics->GetContext()->PSSetSamplers(0, 1, pSampler.GetAddressOf());

	graphics->GetContext()->DrawIndexed(totalIndicies, 0u, 0u);
}

void MeshRenderer::Update()
{
	constBufferData.transform = dx::XMMatrixTranspose(
		dx::XMMatrixRotationX(transform.Rotation().x) *
		dx::XMMatrixRotationY(transform.Rotation().y) *
		dx::XMMatrixRotationZ(transform.Rotation().z) *
		dx::XMMatrixTranslation(transform.Position().x, transform.Position().y, transform.Position().z) *
		dx::XMMatrixPerspectiveLH(1.0f, (float)graphics->GetWindowHeight() / (float)graphics->GetWindowWidth(), 0.5f, 50.0f));
	graphics->GetContext()->UpdateSubresource(pWorldConstantBuffer.Get(), 0, 0, &constBufferData, 0, 0);
}

void MeshRenderer::CreateVertexBuffer(const Vertex vertices[], UINT sizeofVertices)
{
	// Fill out the desciptor for a new buffer object (vertex buffer)
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeofVertices;
	bd.StructureByteStride = sizeof(Vertex);

	// Create a subresource struct that hold a pointer to the vertex data
	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = vertices;

	// Create the buffer in pVertexBuffer using the buffer description and subresource data

	SimpleDebug::ProcessHresult(graphics->GetDevice()->CreateBuffer(&bd, &sd, &pVertexBuffer));
	ID3D11Buffer* testBuffer = pVertexBuffer.Get();
}

void MeshRenderer::CreateIndexBuffer(const unsigned short indices[], UINT sizeofIndices)
{

	D3D11_BUFFER_DESC ibd;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeofIndices;
	ibd.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA isd;
	isd.pSysMem = indices;

	SimpleDebug::ProcessHresult(graphics->GetDevice()->CreateBuffer(&ibd, &isd, pIndexBuffer.GetAddressOf()));
}

void MeshRenderer::CreateWorldConstantBuffer()
{
	// Create a constant buffer for the transformation matrix


	constBufferData.transform =
		dx::XMMatrixTranspose(
			dx::XMMatrixRotationX(transform.Rotation().x) *
			dx::XMMatrixRotationY(transform.Rotation().y) *
			dx::XMMatrixRotationZ(transform.Rotation().z) *
			dx::XMMatrixTranslation(transform.Position().x, transform.Position().y, transform.Position().z) *
			dx::XMMatrixPerspectiveLH(1.0f, (float)graphics->GetWindowHeight() / (float)graphics->GetWindowWidth(), 0.5f, 10.0f));


	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.CPUAccessFlags = 0;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(constBufferData);
	cbd.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA csd;
	csd.pSysMem = &constBufferData;
	graphics->GetDevice()->CreateBuffer(&cbd, &csd, &pWorldConstantBuffer);
}

void MeshRenderer::CreateColourConstantBuffer()
{
	// lookup table for cube face colors
	struct ConstantBuffer
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[6];
	};
	const ConstantBuffer cb2 =
	{
		{
			{ 1.0f,0.0f,1.0f },
			{ 1.0f,0.0f,0.0f },
			{ 0.0f,1.0f,0.0f },
			{ 0.0f,0.0f,1.0f },
			{ 1.0f,1.0f,0.0f },
			{ 0.0f,1.0f,1.0f },
		}
	};

	D3D11_BUFFER_DESC cbd2;
	cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd2.Usage = D3D11_USAGE_DEFAULT;
	cbd2.CPUAccessFlags = 0u;
	cbd2.MiscFlags = 0u;
	cbd2.ByteWidth = sizeof(cb2);
	cbd2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA csd2 = {};
	csd2.pSysMem = &cb2;
	SimpleDebug::ProcessHresult(graphics->GetDevice()->CreateBuffer(&cbd2, &csd2, &pColourConstantBuffer));
}

void MeshRenderer::CreateVertexShader()
{
	// Create vertex shader
	wrl::ComPtr<ID3DBlob> pBlob;
	if(pTexture)
		SimpleDebug::ProcessHresult(D3DReadFileToBlob(L"TextureVS.cso", &pBlob));
	else
		SimpleDebug::ProcessHresult(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	SimpleDebug::ProcessHresult(graphics->GetDevice()->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	// Input layout
	if (pTexture)
	{
		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		SimpleDebug::ProcessHresult(graphics->GetDevice()->CreateInputLayout(
			ied,
			(UINT)std::size(ied),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout));
	}
	else
	{
		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		SimpleDebug::ProcessHresult(graphics->GetDevice()->CreateInputLayout(
			ied,
			(UINT)std::size(ied),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout));
	}


	// Set primitive topology
	graphics->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphics->GetContext()->IASetInputLayout(pInputLayout.Get());
}

void MeshRenderer::CreatePixelShader()
{
	// Create pixel shader
	wrl::ComPtr<ID3DBlob> pBlob;
	if(pTexture)
		D3DReadFileToBlob(L"TexturePS.cso", &pBlob);
	else
		D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
	SimpleDebug::ProcessHresult(graphics->GetDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
}