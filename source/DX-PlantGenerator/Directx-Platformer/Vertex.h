#pragma once
#include <DirectXMath.h>
struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 tex;

	Vertex() = default;

	Vertex(float _x, float _y, float _z, float _u = 0, float _v = 0)
	{
		position.x = _x;
		position.y = _y;
		position.z = _z;

		tex.x = _u;
		tex.y = _v;
	}

	Vertex operator*(const float& rhs)
	{
		this->position.x *= rhs;
		this->position.y *= rhs;
		this->position.z *= rhs;
		return *this;
	}
};