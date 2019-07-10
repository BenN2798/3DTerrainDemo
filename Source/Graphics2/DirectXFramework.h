#pragma once
#include <vector>
#include "Framework.h"
#include "DirectXCore.h"
#include "SceneGraph.h"
#include "Cube.h"
#include "ResourceManager.h"
#include "Camera.h"

class DirectXFramework : public Framework
{
public:
	DirectXFramework();
	DirectXFramework(unsigned int width, unsigned int height);

	virtual void CreateSceneGraph();
	virtual void UpdateSceneGraph();

	bool Initialise();
	void Update();
	void Render();
	void OnResize(WPARAM wParam);
	void Shutdown();

	static DirectXFramework *			GetDXFramework();

	inline SceneGraphPointer			GetSceneGraph() { return _sceneGraph; }
	inline ComPtr<ID3D11Device>			GetDevice() { return _device; }
	inline ComPtr<ID3D11DeviceContext>	GetDeviceContext() { return _deviceContext; }
	inline shared_ptr<ResourceManager>  GetResourceManager() { return _resourceManager; }
	inline shared_ptr<Camera>           GetCamera() { return _camera; }

	XMMATRIX							GetProjectionTransformation();

private:
	ComPtr<ID3D11Device>			_device;
	ComPtr<ID3D11DeviceContext>		_deviceContext;
	ComPtr<IDXGISwapChain>			_swapChain;
	ComPtr<ID3D11Texture2D>			_depthStencilBuffer;
	ComPtr<ID3D11RenderTargetView>	_renderTargetView;
	ComPtr<ID3D11DepthStencilView>	_depthStencilView;

	shared_ptr<ResourceManager>     _resourceManager;

	D3D11_VIEWPORT					_screenViewport;

	// Our vectors and matrices.  Note that we cannot use
	// XMVECTOR and XMMATRIX for class variables since they need
	// to be aligned on 16-byte boundaries and the compiler cannot
	// guarantee this for class variables

	XMFLOAT4X4						_projectionTransformation;

	SceneGraphPointer				_sceneGraph;

	shared_ptr<Camera>				_camera;

	bool GetDeviceAndSwapChain();
};