#pragma once
#include "MeshNode.h"
class MoveableNode :
	public MeshNode
{
public:
	MoveableNode(wstring name, wstring fileName) : MeshNode(name, fileName) { _yaw = 0; _pitch = 0; _roll = 0; _position = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f); _leftRight = 0; _forwardBack = 0; }
	~MoveableNode();

	void Update(FXMMATRIX& currentWorldTransformation) override;

	void SetPitch(float pitch);
	float GetPitch();
	void SetYaw(float yaw);
	float GetYaw();
	void SetRoll(float roll);
	void SetTotalRoll(float roll);
	float GetRoll();
	void SetForwardBack(float forwardBack);
	void SetLeftRight(float leftRight);
	void SetPosition(float x, float y, float z);
	XMFLOAT4 GetPosition();
	XMFLOAT4X4 GetWorldTransform();


private:
	//should be passed in as degrees
	float       _yaw;
	float       _pitch;
	float       _roll;
	float		_forwardBack;
	float		_leftRight;

	XMFLOAT4    _position;
};

