#pragma once
#include "MeshNode.h"
class MoveableNode :
	public MeshNode
{
public:
	MoveableNode(wstring name, wstring fileName) : MeshNode(name, fileName) { _name = name; };
	~MoveableNode();
	void Update();

	void SetPitch(float pitch);
	void SetYaw(float yaw);
	void SetRoll(float roll);
	void Translate(XMMATRIX translate);
	void Translate(float x, float y, float z);


private:
	//should be passed in as degrees
	float       _yaw;
	float       _pitch;
	float       _roll;

	XMMATRIX _translate;
};

