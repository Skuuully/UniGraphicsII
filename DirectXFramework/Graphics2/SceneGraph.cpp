#include "SceneGraph.h"
#include <algorithm>

bool SceneGraph::Initialise(void)
{
	size_t noOfChildren = _children.size();
	// if initialise fails at any point return false, else return true
	for (int i = 0; i < noOfChildren; i++)
	{
		if (_children[i]->Initialise() == false)
		{
			return false;
		}
	}
	return true;
}

void SceneGraph::Update(FXMMATRIX & currentWorldTransformation)
{
	SceneNode::Update(currentWorldTransformation);
	size_t noOfChildren = _children.size();

	for (int i = 0; i < noOfChildren; i++)
	{
		_children[i]->Update(XMLoadFloat4x4(&_combinedWorldTransformation));
	} 
}

void SceneGraph::Render(void)
{
	size_t noOfChildren = _children.size();

	for (int i = 0; i < noOfChildren; i++)
	{
		_children[i]->Render();
	}
}

void SceneGraph::Shutdown(void)
{
	size_t noOfChildren = _children.size();

	for (int i = 0; i < noOfChildren; i++)
	{
		_children[i]->Shutdown();
	}
}

void SceneGraph::Add(SceneNodePointer node)
{
	_children.push_back(node);
}

void SceneGraph::Remove(SceneNodePointer node)
{
	size_t noOfChildren = _children.size();

	for (int i = 0; i < noOfChildren; i++)
	{
		_children[i]->Remove(node);

		if (_children[i] == node)
		{
			_children.erase(_children.begin() + i);
		}
	}
}

SceneNodePointer SceneGraph::Find(wstring name)
{
	size_t noOfChildren = _children.size();

	if (_name == name)
	{
		return SceneNodePointer(this);
	}
	else
	{
		for (int i = 0; i < noOfChildren; i++)
		{
			if (_children[i]->Find(name) != nullptr)
			{
				return _children[i];
			}
		}

		return nullptr;
	}
}
