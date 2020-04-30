#include "InputLayout.h"
#include "SimpleDebug.h"

InputLayout::InputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& desc, ID3DBlob* pVertexShaderBytecode)
{
	SimpleDebug::ProcessHresult(graphics->GetDevice()->CreateInputLayout(
		desc.data(),
		(UINT)desc.size(),
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&inputLayout));
}

void InputLayout::Bind()
{
	graphics->GetContext()->IASetInputLayout(inputLayout.Get());
}
