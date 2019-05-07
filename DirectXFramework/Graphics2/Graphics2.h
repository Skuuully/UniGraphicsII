#pragma once

#ifndef GRAPHICS2
#define GRAPHICS2

#include "DirectXFramework.h"
#include "Cube.h"
#include "MeshNode.h"
#include "TerrainNode.h"
#include "SkyNode.h"
#include "MoveableNode.h"

#endif

class Graphics2 : public DirectXFramework
{
public:
	void CreateSceneGraph();
	void UpdateSceneGraph();

	void CameraHover(XMFLOAT3 offset);
	void MovePlayer();

private:
	//global stuffs
	shared_ptr<TerrainNode> _terrain;
	shared_ptr<MoveableNode> _player;

	// the offset for how far behind the camera should follow
	XMFLOAT3 _offset;

	float _planeMoveInZ;

};

