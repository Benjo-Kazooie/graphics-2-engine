#include "MoveableMeshNode.h"


MoveableMeshNode::~MoveableMeshNode()
{
}

void MoveableMeshNode::Update(FXMMATRIX& currentWorldTransformation)
{
	XMMATRIX meshRotationYaw = XMMatrixRotationAxis(XMLoadFloat4(&_defaultUp), _meshYaw);
	XMStoreFloat4(&_meshRight, XMVector3TransformCoord(XMLoadFloat4(&_defaultRight), meshRotationYaw));
	XMStoreFloat4(&_meshForward, XMVector3TransformCoord(XMLoadFloat4(&_defaultForward), meshRotationYaw));

	XMMATRIX meshRotationPitch = XMMatrixRotationAxis(XMLoadFloat4(&_meshRight), _meshPitch);
	XMStoreFloat4(&_meshUp, XMVector3TransformCoord(XMLoadFloat4(&_defaultUp), meshRotationPitch));
	XMStoreFloat4(&_meshForward, XMVector3TransformCoord(XMLoadFloat4(&_meshForward), meshRotationPitch));

	XMMATRIX meshRotationRoll = XMMatrixRotationAxis(XMLoadFloat4(&_meshForward), _meshRoll);
	XMStoreFloat4(&_meshUp, XMVector3TransformCoord(XMLoadFloat4(&_meshUp), meshRotationRoll));
	XMStoreFloat4(&_meshRight, XMVector3TransformCoord(XMLoadFloat4(&_meshRight), meshRotationRoll));

	_meshPosition = XMFLOAT4(_meshPosition.x + _moveLeftRight * _meshRight.x + _moveForwardBack * _meshForward.x,
		_meshPosition.y + _moveLeftRight * _meshRight.y + _moveForwardBack * _meshForward.y,
		_meshPosition.z + _moveLeftRight * _meshRight.z + _moveForwardBack * _meshForward.z,
		0);

	_moveLeftRight = 0.0f;
	_moveForwardBack = 0.0f;

	XMMATRIX worldTransform = meshRotationYaw * meshRotationPitch * meshRotationRoll * XMMatrixTranslation(_meshPosition.x, _meshPosition.y, _meshPosition.z);
	XMStoreFloat4x4(&_worldTransformation, worldTransform);
	SceneNode::Update(currentWorldTransformation);
}

XMVECTOR MoveableMeshNode::GetPosition(void)
{
	return XMLoadFloat4(&_meshPosition);
}

void MoveableMeshNode::SetPosition(float x, float y, float z)
{
	_meshPosition = XMFLOAT4(x, y, z, 0.0f);
}

void MoveableMeshNode::SetPitch(float pitch)
{
	_meshPitch += XMConvertToRadians(pitch);
}

void MoveableMeshNode::SetTotalPitch(float pitch)
{
	_meshPitch = XMConvertToRadians(pitch);
}

float MoveableMeshNode::GetPitch() const
{
	return XMConvertToDegrees(_meshPitch);
}

void MoveableMeshNode::SetYaw(float yaw)
{
	_meshYaw += XMConvertToRadians(yaw);
}

void MoveableMeshNode::SetTotalYaw(float yaw)
{
	_meshYaw = XMConvertToRadians(yaw);
}

float MoveableMeshNode::GetYaw() const
{
	return XMConvertToDegrees(_meshYaw);
}

void MoveableMeshNode::SetRoll(float roll)
{
	_meshRoll += XMConvertToRadians(roll);
}

void MoveableMeshNode::SetTotalRoll(float roll)
{
	_meshRoll = XMConvertToRadians(roll);
}

float MoveableMeshNode::GetRoll() const
{
	return XMConvertToDegrees(_meshRoll);
}

void MoveableMeshNode::SetLeftRight(float leftRight)
{
	_moveLeftRight = leftRight;
}

void MoveableMeshNode::SetForwardBack(float forwardBack)
{
	_moveForwardBack = forwardBack;
}

//XMMATRIX MoveableMeshNode::GetWorldTransformation() const
//{
//	return _worldTransformation;
//}
