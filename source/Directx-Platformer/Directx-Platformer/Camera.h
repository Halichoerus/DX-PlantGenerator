#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>

class Camera
{
public:
	friend class WorldConstantBuffer;
	friend class Graphics;
	friend class Application;
	friend class PixelBuffer;

	Camera();

	void SetPosition(const DirectX::XMFLOAT3& newPos);
	void Translate(const DirectX::XMFLOAT3& amount);
	void Rotate(const DirectX::XMFLOAT3& amount);
	DirectX::XMFLOAT3 GetPosition();

	DirectX::XMFLOAT3 GetPositionForwardOfCamera(float amountForward);

private:
	void Update();

	bool hasMoved = false;
	DirectX::XMMATRIX cameraRot;
	DirectX::XMMATRIX cameraView;
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX matWorldViewProj;

	DirectX::XMFLOAT3 addPosition = { 0,0,0 };
	DirectX::XMFLOAT3 position;
	DirectX::XMVECTOR positionVec;
	DirectX::XMFLOAT3 rotation;

	DirectX::XMVECTOR cameraPos;
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR right = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR forward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR localCamUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
};

