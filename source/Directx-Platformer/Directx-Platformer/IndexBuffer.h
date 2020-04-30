#pragma once
#include "DXBindable.h"
class IndexBuffer :
	public DXBindable
{
public:
	IndexBuffer(std::vector<unsigned short> indices, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);
	~IndexBuffer();

	void Bind() override;
	UINT GetIndexCount() { return indexCount; };

	BindableType GetType() override { return BindableType::INDEX_BUFFER; };
	void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) { primTopology = topology; };

	void Update(std::vector<unsigned short> indices);

private:
	D3D11_PRIMITIVE_TOPOLOGY primTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT indexCount = 0;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
};

