#pragma once
#include "DXBindable.h"
class InputLayout :
	public DXBindable
{

public:
	InputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& desc, ID3DBlob* pVertexShaderBytecode);
	~InputLayout() = default;

	void Bind() override;
	BindableType GetType() override { return BindableType::INPUT_LAYOUT; };
private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

