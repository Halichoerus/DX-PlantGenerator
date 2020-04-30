#pragma once
#include "DXBindable.h"
#include <string>
class PixelShader :
	public DXBindable
{
public:
	PixelShader(const std::wstring& path);
	~PixelShader() = default;

	void Bind() override;
	BindableType GetType() override { return BindableType::PIXEL_SHADER; };
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
};

