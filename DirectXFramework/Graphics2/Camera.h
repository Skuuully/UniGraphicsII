#pragma once
#include "core.h"
#include "DirectXCore.h"

class MoveableNode;

class Camera
{
public:
	Camera();
	~Camera();

	void Update();
	XMMATRIX GetViewMatrix();
	XMVECTOR GetCameraPosition();
	void SetCameraPosition(float x, float y, float z);
	void SetNodeToFollow(shared_ptr<MoveableNode> nodeToFollow, XMFLOAT3 offset);

private:
	XMFLOAT4    _cameraPosition;

	XMFLOAT4X4  _viewMatrix;


	XMFLOAT3 _offset;
	shared_ptr<MoveableNode> _nodeToFollow;
};

