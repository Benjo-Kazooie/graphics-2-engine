#include "Graphics2.h"

Graphics2 app;

void Graphics2::CreateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();
	
	GetCamera()->SetCameraPosition(0.0f, 50.0f, -500.0f);

	// This is where you add nodes to the scene graph

	shared_ptr<MeshNode> plane = make_shared<MeshNode>(L"Plane1", L"airplane.x");
	sceneGraph->Add(plane);
	shared_ptr<MeshNode> rayman = make_shared<MeshNode>(L"Rayman1", L"rayman_2_mdl.obj");
	sceneGraph->Add(rayman);
	shared_ptr<MeshNode> sonic = make_shared<MeshNode>(L"Sonic1", L"ssonic.obj");
	sceneGraph->Add(sonic);
	shared_ptr<TerrainNode> terrain = make_shared<TerrainNode>(L"Terrain", L"rollinghillsold.raw");
	sceneGraph->Add(terrain);

	sceneGraph->Find(L"Plane1")->SetWorldTransform(XMMatrixTranslation(0, 100, 0));
	sceneGraph->Find(L"Rayman1")->SetWorldTransform(XMMatrixTranslation(0, 100, 0));
	sceneGraph->Find(L"Sonic1")->SetWorldTransform(XMMatrixTranslation(20, 100, 20));
}

void Graphics2::UpdateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();
	_rotAngle += 0.5f;
	if (_sonicTranslate < -1000)
	{
		_sonicTranslate = 1000;
	}
	_sonicTranslate -= 10.0f;
	// This is where you make any changes to the local world transformations to nodes
	// in the scene graph

	sceneGraph->Find(L"Plane1")->SetWorldTransform(XMMatrixScaling(5.0f, 5.0f, 5.0f) * XMMatrixTranslation(100.0f, 200, -100.5f) * XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), _rotAngle * XM_PI / 140.0f));
	//sceneGraph->Find(L"Rayman1")->SetWorldTransform(XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), _rotAngle * 0.5f * XM_PI / 180.0f));*/
	sceneGraph->Find(L"Sonic1")->SetWorldTransform(XMMatrixTranslation(_sonicTranslate, 200, 20.0f));

	/*Translation and scaling matrices, for reference
	SetWorldTransform(XMMatrixScaling(10.0f, 10.0f, 10.0f) * XMMatrixTranslation(5.0f, 0, -5.5f) * XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), _rotAngle * XM_PI / 140.0f));*/

	GetKeyBoardInput();
	GetXInput();
}

void Graphics2::GetKeyBoardInput()
{
	if (GetAsyncKeyState(0x57) < 0)
	{
		//W Key
		GetCamera()->SetForwardBack(2);
	}
	if (GetAsyncKeyState(0x53) < 0)
	{
		//S Key
		GetCamera()->SetForwardBack(-2);
	}
	if (GetAsyncKeyState(0x41) < 0)
	{
		//A Key
		GetCamera()->SetLeftRight(-2);
	}
	if (GetAsyncKeyState(0x44) < 0)
	{
		//D Key
		GetCamera()->SetLeftRight(2);
	}

	if (GetAsyncKeyState(0x49) < 0)
	{
		//I Key
		GetCamera()->SetForwardBack(5);
	}
	if (GetAsyncKeyState(0x4B) < 0)
	{
		//K Key
		GetCamera()->SetForwardBack(-5);
	}
	if (GetAsyncKeyState(0x4A) < 0)
	{
		//J Key
		GetCamera()->SetLeftRight(-5);
	}
	if (GetAsyncKeyState(0x4C) < 0)
	{
		//L Key
		GetCamera()->SetLeftRight(5);
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
}

//Currently does nothing
void Graphics2::GetXInput()
{
	gamePad.ProcessGameController();
}