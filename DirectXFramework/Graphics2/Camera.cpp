#include "Camera.h"
#include "MoveableNode.h"

XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR defaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

Camera::Camera()
{
	_cameraPosition = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	_offset = { 0.0f, 0.0f, 0.0f };
}

Camera::~Camera()
{
}

void Camera::SetNodeToFollow(shared_ptr<MoveableNode> nodeToFollow, XMFLOAT3 offset)
{
	_nodeToFollow = nodeToFollow;
	_offset = offset;
}

XMMATRIX Camera::GetViewMatrix(void)
{
	return XMLoadFloat4x4(&_viewMatrix);
}

XMVECTOR Camera::GetCameraPosition(void)
{
	return XMLoadFloat4(&_cameraPosition);
}

void Camera::SetCameraPosition(float x, float y, float z)
{
	_cameraPosition = XMFLOAT4(x, y, z, 0.0f);
}

void Camera::Update(void)
{
	
	// get the position
	XMFLOAT4 position = _nodeToFollow->GetPosition();
	XMFLOAT4 offset = { _offset.x, _offset.y, _offset.z, 0 };
	XMVECTOR newLocation = { position.x + offset.x, position.y + offset.y, position.z + offset.z, position.w + offset.w };
	
	XMVECTOR cameraTarget;
	XMVECTOR cameraRight;
	XMVECTOR cameraForward;
	XMVECTOR cameraUp;

	// Yaw (rotation around the Y axis) will have an impact on the forward and right vectors
	float yaw = _nodeToFollow->GetYaw();
	XMMATRIX cameraRotationYaw = XMMatrixRotationAxis(defaultUp, yaw);
	cameraRight = XMVector3TransformCoord(defaultRight, cameraRotationYaw);
	cameraForward = XMVector3TransformCoord(defaultForward, cameraRotationYaw);

	float pitch = _nodeToFollow->GetPitch();
	// Pitch (rotation around the X axis) impact the up and forward vectors
	XMMATRIX cameraRotationPitch = XMMatrixRotationAxis(cameraRight, pitch);
	cameraUp = XMVector3TransformCoord(defaultUp, cameraRotationPitch);
	cameraForward = XMVector3TransformCoord(cameraForward, cameraRotationPitch);

	float roll = _nodeToFollow->GetRoll();
	// Roll (rotation around the Z axis) will impact the Up and Right vectors
	XMMATRIX cameraRotationRoll = XMMatrixRotationAxis(cameraForward, roll);
	cameraUp = XMVector3TransformCoord(cameraUp, cameraRotationRoll);
	cameraRight = XMVector3TransformCoord(cameraRight, cameraRotationRoll);

	// Calculate a vector that tells us the direction the camera is looking in
	XMVECTOR pos = { position.x , position.y , position.z };
	cameraTarget = pos + XMVector3Normalize(cameraForward);

	// and calculate our view matrix
	XMStoreFloat4x4(&_viewMatrix, XMMatrixLookAtLH(newLocation, cameraTarget, cameraUp));
	
}
