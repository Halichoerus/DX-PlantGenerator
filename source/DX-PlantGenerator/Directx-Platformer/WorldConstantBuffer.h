#pragma once
#include "DXBindable.h"
#include "Transform.h"

class WorldConstantBuffer
	: public DXBindable
{
public:
	WorldConstantBuffer(Transform& t);
	~WorldConstantBuffer();

	void Update(Transform& t);

	BindableType GetType() override { return BindableType::WORLD_CONSTANT_BUFFER; };
	void Bind() override;
private:

	struct ConstantBuffer
	{
		DirectX::XMMATRIX pos;
		DirectX::XMMATRIX transform;
	};
	ConstantBuffer constBufferData = {};

	DirectX::XMMATRIX localWorld;
	DirectX::XMMATRIX localRot;

	Microsoft::WRL::ComPtr<ID3D11Buffer> worldConstantBuffer;

};

