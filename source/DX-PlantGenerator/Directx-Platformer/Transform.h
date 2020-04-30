#pragma once
#include <DirectXMath.h>

struct Transform
{
public:
	friend class WorldConstantBuffer;

	DirectX::XMFLOAT3& Position() 
	{
		dirty = true;
		return position;
	}

	DirectX::XMFLOAT3& Rotation()
	{
		dirty = true;
		return rotation;
	}

	DirectX::XMFLOAT3& Scale()
	{
		dirty = true;
		return scale;
	}

private:
	bool dirty = false;
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 rotation = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
};