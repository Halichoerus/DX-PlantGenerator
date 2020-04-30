#include "VertexBuffer.h"

void VertexBuffer::Bind()
{
	const UINT offset = 0u;
	graphics->GetContext()->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);
}
