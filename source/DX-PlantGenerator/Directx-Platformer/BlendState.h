#pragma once
#include "DXBindable.h"
class BlendState :
	public DXBindable
{
public:
	BlendState();
	~BlendState() = default;

	void Bind() override;
	BindableType GetType() override { return BindableType::BLEND_STATE; };

private:
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
};

