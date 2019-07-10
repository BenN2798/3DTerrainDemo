#pragma once
#include "SceneNode.h"
#include "DirectXFramework.h"
#include "WICTextureLoader.h"
#include "Framework.h"
#include  <vector>
#include <fstream>


struct _VERTEX
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
	XMFLOAT4 Colour;
};

class TerrainNode : public SceneNode
{
public:
	TerrainNode() : SceneNode(L"Root") {};
	TerrainNode(wstring name, wstring fileName) : SceneNode(name) { _fileName = fileName; };
	~TerrainNode(void) {};

	virtual bool Initialise();
	virtual void Update(FXMMATRIX & currentWorldTransformation);
	virtual void Render();
	virtual void Shutdown();
	void BuildRendererStates();
	XMFLOAT3 NormalPolygon(XMFLOAT3 vertex0, XMFLOAT3 vertex1, XMFLOAT3 vertex2);
	XMFLOAT3 CrossProduct(XMFLOAT3 vectorA, XMFLOAT3 vectorB);
	XMFLOAT3 Normal(XMFLOAT3 vector);
	void BuildGeometryBuffers();
	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void BuildTexture();
	bool LoadHeightMap(wstring heightMapFilename);
private:
	wstring _fileName;
	float xValue;
	float zValue;
	float yValue;
	int yIncrement;
	float squareSize;

	ComPtr<ID3D11Device>			_device;
	ComPtr<ID3D11DeviceContext>		_deviceContext;
	ComPtr<ID3D11RasterizerState>   _defaultRasteriserState;
	ComPtr<ID3D11RasterizerState>   _wireframeRasteriserState;
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

	XMFLOAT4X4						_worldTransformation1;
	XMFLOAT4X4						_worldTransformation2;

	vector<_VERTEX> _grid;
	vector<UINT> _indices;
	vector<float> _heightValues;
};