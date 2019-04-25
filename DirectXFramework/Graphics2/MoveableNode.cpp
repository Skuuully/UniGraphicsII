#include "MoveableNode.h"

MoveableNode::~MoveableNode()
{
}

void MoveableNode::Update()
{
	XMMATRIX newTransform = _translate * XMMatrixRotationX(XMConvertToRadians(_pitch)) *  XMMatrixRotationY(XMConvertToRadians(_yaw)) * XMMatrixRotationZ(XMConvertToRadians(_roll));
	XMStoreFloat4x4(&_worldTransformation, XMLoadFloat4x4(&_worldTransformation) * newTransform);
}

void MoveableNode::SetPitch(float pitch)
{
	_pitch = pitch;
}

void MoveableNode::SetYaw(float yaw)
{
	_yaw = yaw;
}

void MoveableNode::SetRoll(float roll)
{
	_roll = roll;
}

void MoveableNode::Translate(XMMATRIX translate)
{
	_translate = translate;
}

void MoveableNode::Translate(float x, float y, float z)
{
	XMMATRIX translate = XMMatrixTranslation(x, y, z);
	_translate = translate;
}
