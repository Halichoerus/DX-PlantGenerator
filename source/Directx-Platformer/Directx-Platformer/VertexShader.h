#pragma once
#include "DXBindable.h"
#include <string>

class VertexShader :
	public DXBindable
{
public:
	VertexShader(const std::wstring& path);
	~VertexShader() = default;

	void Bind() override;
	ID3DBlob* GetByteBlob() { return bytecodeBlob.Get(); }

	BindableType GetType() override { return BindableType::VERTEX_SHADER; };

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> bytecodeBlob;
};

