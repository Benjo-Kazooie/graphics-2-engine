#pragma once
#include "SceneNode.h"
#include "MeshNode.h"

class MoveableMeshNode : public MeshNode
{
public:
public:
	MoveableMeshNode(wstring name, wstring modelName) : MeshNode(name, modelName)  {}
	~MoveableMeshNode();

	void Update(FXMMATRIX& currentWorldTransformation);

	XMVECTOR GetPosition();
	void SetPosition(float x, float y, float z);
	void SetPitch(float pitch);
	void SetTotalPitch(float pitch);
	float GetPitch() const;
	void SetYaw(float yaw);
	void SetTotalYaw(float yaw);
	float GetYaw() const;
	void SetRoll(float roll);
	void SetTotalRoll(float roll);
	float GetRoll() const;
	void SetLeftRight(float leftRight);
	void SetForwardBack(float forwardBack);
	/*XMMATRIX GetWorldTransformation() const;*/

private:
	XMFLOAT4    _defaultForward;
	XMFLOAT4    _defaultRight;
	XMFLOAT4    _defaultUp;
	XMFLOAT4    _meshForward;
	XMFLOAT4    _meshUp;
	XMFLOAT4    _meshRight;
	XMFLOAT4    _meshPosition;

	float       _moveLeftRight;
	float       _moveForwardBack;

	float       _meshYaw;
	float       _meshPitch;
	float       _meshRoll;
};

