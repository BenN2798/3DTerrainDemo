#pragma once
#include "SceneNode.h"
#include <vector>

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
	vector<SceneNodePointer> _children;
	// Here you need to add whatever collection you are going to
	// use to store the children of this scene graph
};

typedef shared_ptr<SceneGraph>			 SceneGraphPointer;
