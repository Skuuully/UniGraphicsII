#pragma once

#ifndef SCENE_GRAPH
#define SCENE_GRAPH

#include "SceneNode.h"

#include <vector>

#endif

class SceneGraph : public SceneNode
{
public:
	SceneGraph() : SceneNode(L"Root") {};
	SceneGraph(wstring name) : SceneNode(name) {};
	~SceneGraph(void) {};

	virtual bool Initialise(void);
	virtual void Update(FXMMATRIX& currentWorldTransformation);
	virtual void Render(void);
	virtual void Shutdown(void);

	void Add(SceneNodePointer node);
	void Remove(SceneNodePointer node);
	SceneNodePointer Find(wstring name);

private:

	// Here you need to add whatever collection you are going to
	// use to store the children of this scene graph


	vector<SceneNodePointer> _children;
};

typedef shared_ptr<SceneGraph>			 SceneGraphPointer;
