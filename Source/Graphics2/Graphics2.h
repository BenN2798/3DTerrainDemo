#pragma once
#include "DirectXFramework.h"

class Graphics2 : public DirectXFramework
{
public:
	void CreateSceneGraph();
	void UpdateSceneGraph();
private:
	XMFLOAT4X4 _worldTransformation;
	float _rotationAngle = 50;
};

