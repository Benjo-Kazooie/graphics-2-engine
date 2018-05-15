#pragma once
#include "DirectXFramework.h"
#include "Cube.h"
#include "MeshNode.h"
#include "TerrainNode.h"
#include "GamePadController.h"
#include "MoveableMeshNode.h"

class Graphics2 : public DirectXFramework
{
private:
	float _rotAngle = 0;
	float _sonicTranslate = 1000;
	void GetKeyBoardInput();
	void GetXInput();
	GamePadController gamePad;
public:
	void CreateSceneGraph();
	void UpdateSceneGraph();

};

