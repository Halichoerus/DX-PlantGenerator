#include "BlendState.h"
#include "SimpleDebug.h"

BlendState::BlendState()
{
	D3D11_BLEND_DESC desc;
	for (int i = 0; i < 8; i++)
	{
		desc.RenderTarget[i].BlendEnable = TRUE;
		desc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[i].RenderTargetWriteMask = 0x0f;
	}

	SimpleDebug::ProcessHresult(graphics->GetDevice()->CreateBlendState(&desc, &blendState));
}

void BlendState::Bind()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	graphics->GetContext()->OMSetBlendState(blendState.Get(), blendFactor, 0x0f);
}
