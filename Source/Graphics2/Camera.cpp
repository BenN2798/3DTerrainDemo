#include "Camera.h"

Camera::Camera()
{
    _defaultForward = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
    _defaultRight = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
    _defaultUp = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
    _cameraForward = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
    _cameraRight = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
    _cameraUp = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
    _cameraPosition = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    _moveLeftRight = 0.0f;
    _moveForwardBack = 0.0f;
    _cameraYaw = 0.0f;
    _cameraPitch = 0.0f;
    _cameraRoll = 0.0f;
}

Camera::~Camera()
{
}

void Camera::SetPitch(float pitch)
{
    _cameraPitch += XMConvertToRadians(pitch);
}

void Camera::SetYaw(float yaw)
{
    _cameraYaw += XMConvertToRadians(yaw);
}

void Camera::SetRoll(float roll)
{
    _cameraRoll += XMConvertToRadians(roll);
}

void Camera::SetLeftRight(float leftRight)
{
    _moveLeftRight = leftRight;
}

void Camera::SetForwardBack(float forwardBack)
{
    _moveForwardBack = forwardBack;
}

XMMATRIX Camera::GetViewMatrix(void)
{
    return XMLoadFloat4x4(&_viewMatrix);
}

XMVECTOR Camera::GetCameraPosition(void)
{
    return XMLoadFloat4(&_cameraPosition);
}

void Camera::SetCameraPosition(float x, float y, float z)
{
    _cameraPosition = XMFLOAT4(x, y, z, 0.0f);
}

void Camera::Update(void)
{
    XMMATRIX cameraRotationYaw = XMMatrixRotationAxis(XMLoadFloat4(&_defaultUp), _cameraYaw);
    XMStoreFloat4(&_cameraRight, XMVector3TransformCoord(XMLoadFloat4(&_defaultRight), cameraRotationYaw));
    XMStoreFloat4(&_cameraForward, XMVector3TransformCoord(XMLoadFloat4(&_defaultForward), cameraRotationYaw));

    XMMATRIX cameraRotationPitch = XMMatrixRotationAxis(XMLoadFloat4(&_cameraRight), _cameraPitch);
    XMStoreFloat4(&_cameraUp, XMVector3TransformCoord(XMLoadFloat4(&_defaultUp), cameraRotationPitch));
    XMStoreFloat4(&_cameraForward, XMVector3TransformCoord(XMLoadFloat4(&_cameraForward), cameraRotationPitch));

    XMMATRIX cameraRotationRoll = XMMatrixRotationAxis(XMLoadFloat4(&_cameraForward), _cameraRoll);
    XMStoreFloat4(&_cameraUp, XMVector3TransformCoord(XMLoadFloat4(&_cameraUp), cameraRotationRoll));
    XMStoreFloat4(&_cameraRight, XMVector3TransformCoord(XMLoadFloat4(&_cameraRight), cameraRotationRoll));

	_cameraPosition = XMFLOAT4(_cameraPosition.x + _moveLeftRight * _cameraRight.x + _moveForwardBack * _cameraForward.x,
							   _cameraPosition.y + _moveLeftRight * _cameraRight.y + _moveForwardBack * _cameraForward.y,
							   _cameraPosition.z + _moveLeftRight * _cameraRight.z + _moveForwardBack * _cameraForward.z,
							   0);

	_moveLeftRight = 0.0f;
	_moveForwardBack = 0.0f;

	XMVECTOR cameraPosition = XMLoadFloat4(&_cameraPosition);
	XMVECTOR cameraTarget = cameraPosition + XMVector3Normalize(XMLoadFloat4(&_cameraForward));

	XMStoreFloat4x4(&_viewMatrix, XMMatrixLookAtLH(cameraPosition, cameraTarget, XMLoadFloat4(&_cameraUp)));
}
