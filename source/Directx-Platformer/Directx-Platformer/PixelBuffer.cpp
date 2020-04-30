#include "PixelBuffer.h"

PixelBuffer::PixelBuffer()
{
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(constBufferData);
	cbd.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA csd;
	csd.pSysMem = &constBufferData;
	graphics->GetDevice()->CreateBuffer(&cbd, &csd, &pixelConstantBuffer);
}

void PixelBuffer::Update(const DirectX::XMFLOAT3& newPos)
{
	constBufferData.pos.x = newPos.x;
	constBufferData.pos.y = newPos.y;
	constBufferData.pos.z = newPos.z;

	D3D11_MAPPED_SUBRESOURCE msr;
	graphics->GetContext()->Map(pixelConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
	memcpy(msr.pData, &constBufferData, sizeof(constBufferData));
	graphics->GetContext()->Unmap(pixelConstantBuffer.Get(), 0u);
}

void PixelBuffer::Bind()
{
	graphics->GetContext()->PSSetConstantBuffers(0u, 1u, pixelConstantBuffer.GetAddressOf());
}
