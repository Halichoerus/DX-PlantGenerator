#include "WorldConstantBuffer.h"
namespace dx = DirectX;
#include <string>

WorldConstantBuffer::~WorldConstantBuffer()
{
}

WorldConstantBuffer::WorldConstantBuffer(Transform& t)
{
	// Create a constant buffer for the transformation matrix
	constBufferData.transform =
		dx::XMMatrixTranspose(
			dx::XMMatrixRotationX(t.rotation.x) *
			dx::XMMatrixRotationY(t.rotation.y) *
			dx::XMMatrixRotationZ(t.rotation.z) *
			dx::XMMatrixTranslation(t.position.x, t.position.y, t.position.z) *
			graphics->GetCamera().matWorldViewProj);

	localRot = dx::XMMatrixRotationX(t.rotation.x) *
		dx::XMMatrixRotationY(t.rotation.y) *
		dx::XMMatrixRotationZ(t.rotation.z);

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = sizeof(constBufferData);
	cbd.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA csd;
	csd.pSysMem = &constBufferData;
	graphics->GetDevice()->CreateBuffer(&cbd, &csd, &worldConstantBuffer);
}

void WorldConstantBuffer::Update(Transform& t)
{

		localRot = dx::XMMatrixRotationX(t.rotation.x) *
			dx::XMMatrixRotationY(t.rotation.y) *
			dx::XMMatrixRotationZ(t.rotation.z);

		localWorld =
			localRot *
			dx::XMMatrixTranslation(t.position.x, t.position.y, t.position.z) *
			dx::XMMatrixScaling(t.scale.x, t.scale.y, t.scale.z);
		t.dirty = false;
	

	constBufferData.pos = dx::XMMatrixTranspose(
		localWorld *
		graphics->GetCamera().matWorldViewProj);

	constBufferData.transform = dx::XMMatrixTranspose(
		localWorld *
		graphics->GetCamera().matWorldViewProj);

	//constBufferData.pos = dx::XMMatrixTranspose(localRot);

	D3D11_MAPPED_SUBRESOURCE msr;
	graphics->GetContext()->Map(worldConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
	memcpy(msr.pData, &constBufferData, sizeof(constBufferData));
	graphics->GetContext()->Unmap(worldConstantBuffer.Get(), 0u);


}

void WorldConstantBuffer::Bind()
{
	graphics->GetContext()->VSSetConstantBuffers(0u, 1u, worldConstantBuffer.GetAddressOf());
}
