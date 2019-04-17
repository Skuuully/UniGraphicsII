#pragma once

#ifndef GRAPHICS2
#define GRAPHICS2

#include "DirectXFramework.h"
#include "Cube.h"
#include "MeshNode.h"
#include "TerrainNode.h"
#include "SkyNode.h"

#endif

class Graphics2 : public DirectXFramework
{
public:
	void CreateSceneGraph();
	void UpdateSceneGraph();

	void MoveCamera();
	void CameraHover(XMFLOAT3 offset);
	float DegreesToRadians(float degrees);

private:
	float _rotationAngle;
	int _cameraSensitivity;
	shared_ptr<TerrainNode> _terrain;
	// the offset for how far behind the camera should follow
	XMFLOAT3 _offset;

	float _planeMoveInZ;

};

