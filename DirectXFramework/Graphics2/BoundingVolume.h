#pragma once
#include "DirectXFramework.h"

class BoundingVolume
{
public:
	BoundingVolume();
	~BoundingVolume();

	virtual bool IsIntersecting(shared_ptr<BoundingVolume> otherVolume) { return false; }
	virtual void Update(FXMMATRIX& worldTransformation) {};
	#if defined(RENDER_BOUNDING_VOLUMES)
	virtual void Initialise() {}; 
	virtual void Render() {};
	#endif
};

