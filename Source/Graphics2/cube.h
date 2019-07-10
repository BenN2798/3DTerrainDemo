#pragma once
#include "SceneNode.h"
#include "WICTextureLoader.h"
#include "DirectXFramework.h"
#include "Framework.h"

class Cube : public SceneNode
{
public:
	Cube() : SceneNode(L"Root") {};
	Cube(wstring name) : SceneNode(name) {};
	~Cube(void) {};

	virtual bool Initialise();
	virtual void Update(FXMMATRIX& currentWorldTransformation);
	virtual void Render();
	virtual void Shutdown();
	virtual void OnResize(WPARAM wParam);
private:
	Framework _framework;
	ComPtr<ID3D11Device>			_device;
	ComPtr<ID3D11DeviceContext>		_deviceContext;
	ComPtr<IDXGISwapChain>			_swapChain;
	ComPtr<ID3D11Texture2D>			_depthStencilBuffer;
	ComPtr<ID3D11RenderTargetView>	_renderTargetView;
	ComPtr<ID3D11DepthStencilView>	_depthStencilView;

	ComPtr<ID3D11Buffer>			_vertexBuffer;
	ComPtr<ID3D11Buffer>			_indexBuffer;

	ComPtr<ID3DBlob>				_vertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>				_pixelShaderByteCode = nullptr;
	ComPtr<ID3D11VertexShader>		_vertexShader;
	ComPtr<ID3D11PixelShader>		_pixelShader;
	ComPtr<ID3D11InputLayout>		_layout;
	ComPtr<ID3D11Buffer>			_constantBuffer;

	ComPtr<ID3D11ShaderResourceView> _texture;;

	D3D11_VIEWPORT					_screenViewport;

	// Our vectors and matrices.  Note that we cannot use
	// XMVECTOR and XMMATRIX for class variables since they need
	// to be aligned on 16-byte boundaries and the compiler cannot
	// guarantee this for class variables

	XMFLOAT4						_eyePosition;
	XMFLOAT4						_focalPointPosition;
	XMFLOAT4						_upVector;

	XMFLOAT4X4						_worldTransformation1;
	XMFLOAT4X4						_worldTransformation2;
	XMFLOAT4X4						_viewTransformation;
	XMFLOAT4X4						_projectionTransformation;

	float							_rotationAngle;

	bool GetDeviceAndSwapChain();
	void BuildGeometryBuffers();
	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void BuildTexture();

};