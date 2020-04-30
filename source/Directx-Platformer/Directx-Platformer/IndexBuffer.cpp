#include "IndexBuffer.h"
#include "SimpleDebug.h"

IndexBuffer::~IndexBuffer()
{
	int a;
}

IndexBuffer::IndexBuffer(std::vector<unsigned short> indices, D3D11_USAGE usage)
	: indexCount((UINT)indices.size())
{
	D3D11_BUFFER_DESC ibd;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = usage;
	if (usage == D3D11_USAGE_DYNAMIC)
		ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else
		ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = UINT(indexCount * sizeof(unsigned short));
	ibd.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA isd;
	isd.pSysMem = indices.data();

	SimpleDebug::ProcessHresult(graphics->GetDevice()->CreateBuffer(&ibd, &isd, indexBuffer.GetAddressOf()));

}

void IndexBuffer::Bind()
{
	graphics->GetContext()->IASetPrimitiveTopology(primTopology);
	graphics->GetContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

void IndexBuffer::Update(std::vector<unsigned short> indices)
{
	indexCount = (UINT)indices.size();

	D3D11_BUFFER_DESC bd;
	indexBuffer->GetDesc(&bd);
	UINT newWidth = UINT(indexCount * sizeof(unsigned short));
	if (newWidth > bd.ByteWidth)
	{
		bd.ByteWidth = newWidth;
		bd.StructureByteStride = sizeof(unsigned short);

		D3D11_SUBRESOURCE_DATA isd;
		isd.pSysMem = indices.data();

		indexBuffer.ReleaseAndGetAddressOf();
		SimpleDebug::ProcessHresult(graphics->GetDevice()->CreateBuffer(&bd, &isd, indexBuffer.GetAddressOf()));

		D3D11_BUFFER_DESC bd2;
		indexBuffer->GetDesc(&bd2);
		int a = 0;
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE msr;
		ZeroMemory(&msr, sizeof(D3D11_MAPPED_SUBRESOURCE));
		graphics->GetContext()->Map(indexBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
		memcpy(msr.pData, indices.data(), sizeof(indices[0]) * indices.size());
		graphics->GetContext()->Unmap(indexBuffer.Get(), 0u);
	}
}
