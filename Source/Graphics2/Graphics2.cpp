#include "Graphics2.h"
#include "cube.h"
#include "MeshNode.h"
#include "TerrainNode.h"

Graphics2 app;

void Graphics2::CreateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();
	shared_ptr<TerrainNode> terrain = make_shared<TerrainNode>(L"Terrain", L"volcano.raw");
	sceneGraph->Add(terrain);
	shared_ptr<MeshNode> node = make_shared<MeshNode>(L"Plane1", L"airplane.x");
	sceneGraph->Add(node);
	shared_ptr<MeshNode> node2 = make_shared<MeshNode>(L"Plane2", L"airplane.x");
	sceneGraph->Add(node2);
	GetCamera()->SetCameraPosition(0.0f, 50.0f, -500.0f);
	
	// This is where you add nodes to the scene graph
}

void Graphics2::UpdateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();
	_rotationAngle++;
	sceneGraph->Find(L"Plane1")->SetWorldTransform(XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), -90) * XMMatrixTranslation(2.0f, 30.0f, 50.0f) *XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), _rotationAngle * 0.5f * XM_PI / 180.0f)  * XMMatrixScaling(15.0f, 15.0f, 15.0f));
	sceneGraph->Find(L"Plane2")->SetWorldTransform(XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), -90) * XMMatrixTranslation(100.0f, 30.0f, 150.0f) *XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), _rotationAngle * 0.5f * XM_PI / 180.0f)  * XMMatrixScaling(15.0f, 15.0f, 15.0f));

	if (GetAsyncKeyState(0x57) < 0)
	{
		GetCamera()->SetForwardBack(1);
	}
	if (GetAsyncKeyState(0x53) < 0)
	{
		GetCamera()->SetForwardBack(-1);
	}
	if (GetAsyncKeyState(0x41) < 0)
	{
		GetCamera()->SetLeftRight(-1);
	}
	if (GetAsyncKeyState(0x44) < 0)
	{
		GetCamera()->SetLeftRight(1);
	}
	if (GetAsyncKeyState(VK_UP) < 0)
	{
		GetCamera()->SetPitch(-1);
	}
	if (GetAsyncKeyState(VK_DOWN) < 0)
	{
		GetCamera()->SetPitch(1);
	}
	if (GetAsyncKeyState(VK_LEFT) < 0)
	{
		GetCamera()->SetYaw(-1);
	}
	if (GetAsyncKeyState(VK_RIGHT) < 0)
	{
		GetCamera()->SetYaw(1);
	}
	// This is where you make any changes to the local world transformations to nodes
	// in the scene graph
}
