#include "SceneGraph.h"

bool SceneGraph::Initialise(void)
{
	list<SceneNodePointer>::iterator iterator = _children.begin();
	for (iterator; iterator != _children.end(); iterator++)
	{
		if (!(*iterator)->Initialise())
		{
			return false;
		}
	}
	return true;
}

void SceneGraph::Update(FXMMATRIX & currentWorldTransformation)
{
	SceneNode::Update(currentWorldTransformation);
	XMMATRIX combinedWorldTransformation = XMLoadFloat4x4(&_combinedWorldTransformation);
	list<SceneNodePointer>::iterator iterator = _children.begin();
	for (iterator; iterator != _children.end(); iterator++)
	{
		(*iterator)->Update(combinedWorldTransformation);
	}
}

void SceneGraph::Render(void)
{
	list<SceneNodePointer>::iterator iterator = _children.begin();
	for (iterator; iterator != _children.end(); iterator++)
	{
		(*iterator)->Render();
	}
}

void SceneGraph::Shutdown(void)
{
	list<SceneNodePointer>::iterator iterator = _children.begin();
	for (iterator; iterator != _children.end(); iterator++)
	{
		(*iterator)->Shutdown();
	}
}

void SceneGraph::Add(SceneNodePointer node)
{
	_children.push_back(node);
}

void SceneGraph::Remove(SceneNodePointer node)
{
	list<SceneNodePointer>::iterator iterator = _children.begin();
	for (iterator; iterator != _children.end(); iterator++)
	{
		if (node == *iterator)
		{
			_children.erase(iterator);
		}
	}
}

SceneNodePointer SceneGraph::Find(wstring name)
{
	if (_name == name)
	{
		return shared_from_this();
	}

	list<SceneNodePointer>::iterator iterator = _children.begin();
	SceneNodePointer found;
	for (iterator; iterator != _children.end(); iterator++)
	{
		if ((*iterator)->Find(name))
		{
			found = (*iterator)->shared_from_this();
		}
	}
	return found;
}
