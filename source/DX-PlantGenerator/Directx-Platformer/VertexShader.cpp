#include <d3dcompiler.h>

#include "VertexShader.h"
#include "SimpleDebug.h"

VertexShader::VertexShader(const std::wstring& path)
{
	SimpleDebug::ProcessHresult(D3DReadFileToBlob(path.c_str(), &bytecodeBlob));

	SimpleDebug::ProcessHresult(graphics->GetDevice()->CreateVertexShader(bytecodeBlob->GetBufferPointer(), bytecodeBlob->GetBufferSize(), nullptr, &vertexShader));
}

void VertexShader::Bind()
{
	graphics->GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0u);
}
