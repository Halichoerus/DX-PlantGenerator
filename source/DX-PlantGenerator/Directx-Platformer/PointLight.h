#pragma once
#include "Graphics.h"
#include "PixelBuffer.h"

class PointLight
{
public:
	PointLight();

	void Bind();

	DirectX::XMFLOAT3& GetPos() {return pos;};

private:

	DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };

	PixelBuffer buffer;
};

