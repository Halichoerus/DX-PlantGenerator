#include <d3dcompiler.h>

#include "PixelShader.h"
#include "SimpleDebug.h"

PixelShader::PixelShader(const std::wstring& path)
{
	// Create pixel shader
	Microsoft::WRL::ComPtr<ID3DBlob> blob;
	D3DReadFileToBlob(path.c_str(), &blob);

	SimpleDebug::ProcessHresult(graphics->GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader));
}

void PixelShader::Bind()
{
	graphics->GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0u);
}
