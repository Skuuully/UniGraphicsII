#include "MoveableNode.h"

XMVECTOR _defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR _defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR _defaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

MoveableNode::~MoveableNode()
{
}

void MoveableNode::Update(FXMMATRIX& currentWorldTransformation)
{
	XMVECTOR position;
	XMVECTOR right;
	XMVECTOR forward;
	XMVECTOR up;

	// Yaw (rotation around the Y axis) will have an impact on the forward and right vectors
	XMMATRIX cameraRotationYaw = XMMatrixRotationAxis(_defaultUp, _yaw);
	right = XMVector3TransformCoord(_defaultRight, cameraRotationYaw);
	forward = XMVector3TransformCoord(_defaultForward, cameraRotationYaw);

	// Pitch (rotation around the X axis) impact the up and forward vectors
	XMMATRIX cameraRotationPitch = XMMatrixRotationAxis(right, _pitch);
	up = XMVector3TransformCoord(_defaultUp, cameraRotationPitch);
	forward = XMVector3TransformCoord(forward, cameraRotationPitch);

	// Roll (rotation around the Z axis) will impact the Up and Right vectors
	XMMATRIX cameraRotationRoll = XMMatrixRotationAxis(forward, _roll);
	up = XMVector3TransformCoord(up, cameraRotationRoll);
	right = XMVector3TransformCoord(right, cameraRotationRoll);

	position = XMLoadFloat4(&_position) + _leftRight * right + _forwardBack * forward;
	XMStoreFloat4(&_position, position);

	// Reset the amount we are moving
	_leftRight = 0.0f;
	_forwardBack = 0.0f;

	XMMATRIX newTransform = XMMatrixRotationX(_pitch) *  XMMatrixRotationY(_yaw) * XMMatrixRotationZ(_roll) *  XMMatrixTranslation(_position.x, _position.y, _position.z);
	XMStoreFloat4x4(&_combinedWorldTransformation, XMLoadFloat4x4(&_combinedWorldTransformation) * newTransform);
	SceneNode::Update(currentWorldTransformation * newTransform);
}

void MoveableNode::SetPitch(float pitch)
{
	_pitch += XMConvertToRadians(pitch);
}

float MoveableNode::GetPitch()
{
	return XMConvertToRadians(_pitch);
}

void MoveableNode::SetYaw(float yaw)
{
	_yaw += XMConvertToRadians(yaw);
}

float MoveableNode::GetYaw()
{
	return  XMConvertToRadians(_yaw);
}

void MoveableNode::SetRoll(float roll)
{
	_roll += XMConvertToRadians(roll);
}

void MoveableNode::SetTotalRoll(float roll)
{
	_roll = XMConvertToRadians(roll);
}

float MoveableNode::GetRoll()
{
	return  XMConvertToRadians(_roll);
}

void MoveableNode::SetForwardBack(float forwardBack)
{
	_forwardBack = forwardBack;
}

void MoveableNode::SetLeftRight(float leftRight)
{
	_leftRight = leftRight;
}

void MoveableNode::SetPosition(float x, float y, float z)
{
	_position = XMFLOAT4(x, y, z, 0.0f);
}

XMFLOAT4 MoveableNode::GetPosition()
{
	return _position;
}

XMFLOAT4X4 MoveableNode::GetWorldTransform()
{
	return _combinedWorldTransformation;
}
