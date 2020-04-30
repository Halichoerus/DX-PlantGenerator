#pragma once
#include "DXBindable.h"
#include "SimpleDebug.h"

class VertexBuffer
	: public DXBindable
{
public:
	template<class t>
	VertexBuffer(std::vector<t> vertices, D3D11_USAGE usage = D3D11_USAGE_DEFAULT) : stride(sizeof(t))
	{
		// Fill out the desciptor for a new buffer object (vertex buffer)
		D3D11_BUFFER_DESC bd;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = usage;
		if(usage == D3D11_USAGE_DYNAMIC)
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		else
			bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT(stride * vertices.size());
		bd.StructureByteStride = stride;

		// Create a subresource struct that hold a pointer to the vertex data
		D3D11_SUBRESOURCE_DATA sd;
		sd.pSysMem = vertices.data();

		// Create the buffer in pVertexBuffer using the buffer description and subresource data

		SimpleDebug::ProcessHresult(graphics->GetDevice()->CreateBuffer(&bd, &sd, &vertexBuffer));
	}

	template<class t>
	void Update(std::vector<t> vertices)
	{
		D3D11_BUFFER_DESC bd;
		vertexBuffer->GetDesc(&bd);
		// If the new byte width is larger, then re-make the buffer
		if ((sizeof(t) * vertices.size()) > bd.ByteWidth)
		{
			bd.ByteWidth = UINT(sizeof(t) * vertices.size());

			// Create a subresource struct that hold a pointer to the vertex data
			D3D11_SUBRESOURCE_DATA sd;
			sd.pSysMem = vertices.data();

			// Create the buffer in pVertexBuffer using the buffer description and subresource data
			vertexBuffer.ReleaseAndGetAddressOf();
			SimpleDebug::ProcessHresult(graphics->GetDevice()->CreateBuffer(&bd, &sd, &vertexBuffer));
		}
		else
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			ZeroMemory(&msr, sizeof(D3D11_MAPPED_SUBRESOURCE));
			graphics->GetContext()->Map(vertexBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
			memcpy(msr.pData, vertices.data(), sizeof(t) * vertices.size());
			graphics->GetContext()->Unmap(vertexBuffer.Get(), 0u);
		}
	}

	~VertexBuffer() = default;

	BindableType GetType() override { return BindableType::VERTEX_BUFFER; };
	void Bind();
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	const UINT stride = 0;
};

