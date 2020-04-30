#include <string>
#include "Camera.h"

Camera::Camera()
{
	SetPosition({ 0,0,0 });
}

void Camera::SetPosition(const DirectX::XMFLOAT3& newPos)
{
	using namespace DirectX;

	addPosition = newPos;

	Update();
}

void Camera::Translate(const DirectX::XMFLOAT3& amount)
{
	using namespace DirectX;

	addPosition.x += amount.x;
	addPosition.y += amount.y;
	addPosition.z += amount.z;

	Update();
}

void Camera::Rotate(const DirectX::XMFLOAT3& amount)
{
	rotation.x += amount.x;
	rotation.y += amount.y;
	rotation.z += amount.z;

	if (rotation.x > 1.5f)
		rotation.x = 1.5f;
	if (rotation.x < -1.5f)
		rotation.x = -1.5f;

	Update();
}

void Camera::Update()
{
	using namespace DirectX;
	hasMoved = true;

	cameraRot = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, 0);
	XMVECTOR target = XMVector3TransformCoord(forward, cameraRot);
	target = XMVector3Normalize(target);

	XMMATRIX rotateYTempMatrix = XMMatrixRotationY(rotation.y);

	XMVECTOR camRight = XMVector3TransformCoord(right, rotateYTempMatrix);
	XMVECTOR camForward = XMVector3TransformCoord(forward, rotateYTempMatrix);
	//XMVECTOR camUp = XMVector3TransformCoord(up, rotateYTempMatrix);
	//localCamUp = XMVector3TransformCoord(camUp, rotateYTempMatrix);


	positionVec += addPosition.x * camRight;
	positionVec += addPosition.y * up;
	positionVec += addPosition.z * camForward;

	XMStoreFloat3(&position, positionVec);


	addPosition = { 0,0,0 };

	//target += cameraPos;
	target += positionVec;

	//XMVECTOR upDir = XMVector3TransformCoord(up, cameraRot);

	cameraView = XMMatrixLookAtLH(positionVec, target, up);
	//cameraView = XMMatrixLookAtLH(pos, target, camUp);
	//cameraView = XMMatrixTranslation(position.x, position.y, position.z);

	matWorldViewProj = cameraView * projection;

}

DirectX::XMFLOAT3 Camera::GetPositionForwardOfCamera(float amountForward)
{
	using namespace DirectX;

	cameraRot = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, 0);
	XMVECTOR target = XMVector3TransformCoord(forward, cameraRot);
	target = XMVector3Normalize(target);

	XMMATRIX rotateYTempMatrix = XMMatrixRotationY(rotation.y);

	XMVECTOR camRight = XMVector3TransformCoord(right, rotateYTempMatrix);
	XMVECTOR camForward = XMVector3TransformCoord(forward, rotateYTempMatrix);

	target *= amountForward;
	target += positionVec;

	XMFLOAT3 returnPos;
	DirectX::XMStoreFloat3(&returnPos, target);
	return returnPos;
}

DirectX::XMFLOAT3 Camera::GetPosition() 
{
	return position;
}