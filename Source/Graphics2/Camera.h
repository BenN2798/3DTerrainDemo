#pragma once
#include "core.h"
#include "DirectXCore.h"

class Camera
{
public:
    Camera();
    ~Camera();

    void Update();
    XMMATRIX GetViewMatrix();
    XMVECTOR GetCameraPosition();
    void SetCameraPosition(float x, float y, float z);
    void SetPitch(float pitch);
    void SetYaw(float yaw);
    void SetRoll(float roll);
    void SetLeftRight(float leftRight);
    void SetForwardBack(float forwardBack);

private:
    XMFLOAT4    _defaultForward;
    XMFLOAT4    _defaultRight;
    XMFLOAT4    _defaultUp;
    XMFLOAT4    _cameraForward;
    XMFLOAT4    _cameraUp;
    XMFLOAT4    _cameraRight;
    XMFLOAT4    _cameraPosition;

    XMFLOAT4X4  _viewMatrix;

    float       _moveLeftRight;
    float       _moveForwardBack;

    float       _cameraYaw;
    float       _cameraPitch;
    float       _cameraRoll;
};

