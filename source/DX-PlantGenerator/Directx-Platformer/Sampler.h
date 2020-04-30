#pragma once
#include "DXBindable.h"
class Sampler :
	public DXBindable
{
public:
	Sampler();
	~Sampler() = default;

	void Bind() override;
	BindableType GetType() override { return BindableType::SAMPLER; };
private:
	Microsoft::WRL::ComPtr <ID3D11SamplerState> sampler;
};

