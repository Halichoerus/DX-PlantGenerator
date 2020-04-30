#pragma once
#include "DXBindable.h"
class PixelBuffer :
	public DXBindable
{
public:
	PixelBuffer();

	BindableType GetType() override { return BindableType::PIXEL_BUFFER; };
	void Update(const DirectX::XMFLOAT3& newPos);
	void Bind() override;
private:

	struct ConstantBuffer
	{
		DirectX::XMFLOAT3 pos;
		float padding;
	};
	ConstantBuffer constBufferData = {};

	Microsoft::WRL::ComPtr<ID3D11Buffer> pixelConstantBuffer;
};

