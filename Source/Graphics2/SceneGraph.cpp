#include "SceneGraph.h"

/*SceneGraph::SceneGraph() : SceneNode(L"Root")
{

}

SceneGraph::SceneGraph(wstring name) : SceneNode(name)
{

}

SceneGraph::~SceneGraph(void)
{

}*/

bool SceneGraph::Initialise(void)
{
	bool initialised;
	_int64 j;

	for (std::vector<SceneNodePointer>::iterator i = _children.begin(); i != _children.end(); i++)
	{
		j = i - _children.begin();
		initialised = _children.at(j)->Initialise();
		if (!initialised)
		{
			return false;
		}
	}
	return true;
}

void SceneGraph::Update(FXMMATRIX& currentWorldTransformation)
{
	_int64 j;
	for (std::vector<SceneNodePointer>::iterator i = _children.begin(); i != _children.end(); i++)
	{
		j = i - _children.begin();
		XMMATRIX _transformation = XMLoadFloat4x4(&_worldTransformation) * currentWorldTransformation;
		_children.at(j)->Update(_transformation);
	}
}

void SceneGraph::Render(void)
{
	_int64 j;
	
	for (std::vector<SceneNodePointer>::iterator i = _children.begin(); i != _children.end(); i++)
	{
		j = i - _children.begin();
		_children.at(j)->Render();
	}
}

void SceneGraph::Shutdown(void)
{
	_int64 j;

	for (std::vector<SceneNodePointer>::iterator i = _children.begin(); i != _children.end(); i++)
	{
		j = i - _children.begin();
		_children.at(j)->Shutdown();
	}
}

void SceneGraph::Add(SceneNodePointer node)
{
	_children.push_back(node);
}

void SceneGraph::Remove(SceneNodePointer node)
{
	_int64 j;

	for (std::vector<SceneNodePointer>::iterator i = _children.begin(); i != _children.end(); i++)
	{
		j = i - _children.begin();

		if (_children[j] == node)
		{
			_children.erase(i);
		}
	}
}

SceneNodePointer SceneGraph::Find(wstring name)
{
	_int64 j;

	for (int i = 0; i < _children.size(); i++)
	{
		if (_children[i]->GetName() == name)
		{
			return _children[i];
		}
	}
}