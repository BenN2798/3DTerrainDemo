#include "TerrainNode.h"
#define NUMBER_OF_ROWS			256
#define NUMBER_OF_COLUMNS	    256

struct CBUFFER
{
	XMMATRIX    CompleteTransformation;
	XMMATRIX	WorldTransformation;
	XMFLOAT4	CameraPosition;
	XMVECTOR    LightVector;
	XMFLOAT4    LightColour;
	XMFLOAT4    AmbientColour;
	XMFLOAT4    DiffuseColour;
	XMFLOAT4	SpecularColour;
	float		Shininess;
	float       Padding[3];
};

bool TerrainNode::Initialise()
{
	_device = DirectXFramework::GetDXFramework()->GetDevice();
	_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();

	BuildRendererStates();
	LoadHeightMap(_fileName);
	BuildGeometryBuffers();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	BuildTexture();

	return true;
}

void TerrainNode::Update(FXMMATRIX & currentWorldTransformation)
{
	XMStoreFloat4x4(&_combinedWorldTransformation, XMLoadFloat4x4(&_worldTransformation) * currentWorldTransformation);
}

void TerrainNode::Render()
{
	XMMATRIX viewTransformation = DirectXFramework::GetDXFramework()->GetCamera()->GetViewMatrix();

	// Calculate the world x view x projection transformation
	XMMATRIX completeTransformation = XMLoadFloat4x4(&_combinedWorldTransformation) * viewTransformation * DirectXFramework::GetDXFramework()->GetProjectionTransformation();

	// Draw the grid
	CBUFFER cBuffer;
	cBuffer.CompleteTransformation = completeTransformation;
	cBuffer.WorldTransformation = XMLoadFloat4x4(&_worldTransformation1);
	cBuffer.CameraPosition = XMFLOAT4(0.0f, 50.0f, -500.0f, 0.0f);
	cBuffer.AmbientColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cBuffer.LightVector = XMVector4Normalize(XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f));
	cBuffer.LightColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cBuffer.DiffuseColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cBuffer.SpecularColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cBuffer.Shininess = 10.0f;

	// Update the constant buffer 
	_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &cBuffer, 0, 0);

	// Set the texture to be used by the pixel shader
	_deviceContext->PSSetShaderResources(0, 1, _texture.GetAddressOf());
	_deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);
	_deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);
	_deviceContext->IASetInputLayout(_layout.Get());

	// Now render the grid
	UINT stride = sizeof(_VERTEX);
	UINT offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//_deviceContext->RSSetState(_wireframeRasteriserState.Get());
	//_deviceContext->RSSetState(_defaultRasteriserState.Get());
	_deviceContext->DrawIndexed(UINT(_indices.size()), 0, 0);
}

void TerrainNode::Shutdown()
{

}

void TerrainNode::BuildRendererStates()
{
	// Set default and wireframe rasteriser states
	D3D11_RASTERIZER_DESC rasteriserDesc;
	rasteriserDesc.FillMode = D3D11_FILL_SOLID;
	rasteriserDesc.CullMode = D3D11_CULL_BACK;
	rasteriserDesc.FrontCounterClockwise = false;
	rasteriserDesc.DepthBias = 0;
	rasteriserDesc.SlopeScaledDepthBias = 0.0f;
	rasteriserDesc.DepthBiasClamp = 0.0f;
	rasteriserDesc.DepthClipEnable = true;
	rasteriserDesc.ScissorEnable = false;
	rasteriserDesc.MultisampleEnable = false;
	rasteriserDesc.AntialiasedLineEnable = false;
	ThrowIfFailed(_device->CreateRasterizerState(&rasteriserDesc, _defaultRasteriserState.GetAddressOf()));
	rasteriserDesc.FillMode = D3D11_FILL_WIREFRAME;
	ThrowIfFailed(_device->CreateRasterizerState(&rasteriserDesc, _wireframeRasteriserState.GetAddressOf()));
}

XMFLOAT3 TerrainNode::NormalPolygon(XMFLOAT3 vertex0, XMFLOAT3 vertex1, XMFLOAT3 vertex2)
{
	XMFLOAT3 vectorA = XMFLOAT3(vertex0.x - vertex1.x, vertex0.y - vertex1.y, vertex0.z - vertex1.z);
	XMFLOAT3 vectorB = XMFLOAT3(vertex0.x - vertex2.x, vertex0.y - vertex2.y, vertex0.z - vertex2.z);
	XMFLOAT3 crossProduct = CrossProduct(vectorA, vectorB);
	XMFLOAT3 normal = Normal(crossProduct);
	return normal;
}

XMFLOAT3 TerrainNode::CrossProduct(XMFLOAT3 vectorA, XMFLOAT3 vectorB)
{
	float x = (vectorA.y * vectorB.z) - (vectorA.z * vectorB.y);
	float y = (vectorA.z * vectorB.x) - (vectorA.x * vectorB.z);
	float z = (vectorA.x * vectorB.y) - (vectorA.y * vectorB.x);
	return XMFLOAT3(x, y, z);
}

XMFLOAT3 TerrainNode::Normal(XMFLOAT3 vector)
{
	float length = sqrt((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z));

	float x = vector.x / length;
	float y = vector.y / length;
	float z = vector.z / length;
	return XMFLOAT3(x, y, z);
}

void TerrainNode::BuildGeometryBuffers()
{
	xValue = 128.0f;
	zValue = -126.0f;
	yValue = 0.0f;
	yIncrement = 0;
	squareSize = 10.0f;
	UINT index = 0;
	float textureIncrement = 0.0038910506f;
	float xTexture = 0.0f;
	float zTexture = 0.0f;
	for (float z = 128.0f; z > zValue; z--)
	{
		for (float x = -128.0f; x < xValue; x++)
		{
			xTexture = textureIncrement * (x + 128);
			zTexture = textureIncrement * (z + 128);

			yValue = _heightValues[yIncrement] * 256;

			_VERTEX vertex;
			vertex.Position = XMFLOAT3(x * squareSize, float(yValue), z * squareSize);
			vertex.Normal = XMFLOAT3(0, 0, 0);
			vertex.TexCoord = XMFLOAT2(xTexture, zTexture);
			vertex.Colour = XMFLOAT4(0.0, 0.0, 0.0, 1.0);

			_VERTEX vertex2;
			vertex2.Position = XMFLOAT3((x * squareSize) + squareSize, float(_heightValues[yIncrement + 1] * 256), z * squareSize);
			vertex2.Normal = XMFLOAT3(0, 0, 0);
			vertex2.TexCoord = XMFLOAT2(xTexture + textureIncrement, zTexture);
			vertex2.Colour = XMFLOAT4(0.0, 0.0, 0.0, 1.0);

			_VERTEX vertex3;
			vertex3.Position = XMFLOAT3(x * squareSize, float(_heightValues[yIncrement + 256] * 256), (z * squareSize) - squareSize);
			vertex3.Normal = XMFLOAT3(0, 0, 0);
			vertex3.TexCoord = XMFLOAT2(xTexture, zTexture - textureIncrement);
			vertex3.Colour = XMFLOAT4(0.0, 0.0, 0.0, 1.0);

			_VERTEX vertex4;
			vertex4.Position = XMFLOAT3((x * squareSize) + squareSize, float(_heightValues[yIncrement + 257] * 256), (z * squareSize) - squareSize);
			vertex4.Normal = XMFLOAT3(0, 0, 0);
			vertex4.TexCoord = XMFLOAT2(xTexture + textureIncrement, zTexture - textureIncrement);
			vertex4.Colour = XMFLOAT4(0.0, 0.0, 0.0, 1.0);

			XMFLOAT3 polygonNormal1 = NormalPolygon(vertex.Position, vertex2.Position, vertex3.Position);
			XMFLOAT3 polygonNormal2 = NormalPolygon(vertex3.Position, vertex2.Position, vertex4.Position);
			vertex.Normal = polygonNormal1;
			vertex2.Normal = XMFLOAT3(polygonNormal1.x + polygonNormal2.x, polygonNormal1.y + polygonNormal2.y, polygonNormal1.z + polygonNormal2.z);
			vertex2.Normal = XMFLOAT3(vertex2.Normal.x / 2, vertex2.Normal.y / 2, vertex3.Normal.z / 2);
			vertex3.Normal = XMFLOAT3(polygonNormal1.x + polygonNormal2.x, polygonNormal1.y + polygonNormal2.y, polygonNormal1.z + polygonNormal2.z);
			vertex3.Normal = XMFLOAT3(vertex3.Normal.x / 2, vertex3.Normal.y / 2, vertex3.Normal.z / 2);
			vertex4.Normal = polygonNormal2;

			vertex.Normal = Normal(vertex.Normal);
			vertex2.Normal = Normal(vertex2.Normal);
			vertex3.Normal = Normal(vertex3.Normal);
			vertex4.Normal = Normal(vertex4.Normal);

			_grid.push_back(vertex);
			_grid.push_back(vertex2);
			_grid.push_back(vertex3);
			_grid.push_back(vertex4);

			_indices.push_back(index); //v1
			index++;
			_indices.push_back(index); //v2
			index++;
			_indices.push_back(index); //v3
			_indices.push_back(index); //v3
			_indices.push_back(index - 1); //v2
			index++;
			_indices.push_back(index); //v4
			index++;
			yIncrement++;
		}
	}

	D3D11_BUFFER_DESC vertexBufferDescriptor;
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(_VERTEX) * (UINT)_grid.size();
	vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0;
	vertexBufferDescriptor.MiscFlags = 0;
	vertexBufferDescriptor.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexInitialisationData;
	vertexInitialisationData.pSysMem = &_grid[0];
	vertexInitialisationData.SysMemPitch = 0;
	vertexInitialisationData.SysMemSlicePitch = 0;

	ThrowIfFailed(_device->CreateBuffer(&vertexBufferDescriptor, &vertexInitialisationData, _vertexBuffer.GetAddressOf()));

	D3D11_BUFFER_DESC indexBufferDescriptor;
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(UINT) * (UINT)_indices.size();
	indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0;
	indexBufferDescriptor.MiscFlags = 0;
	indexBufferDescriptor.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexInitialisationData;
	indexInitialisationData.pSysMem = &_indices[0];
	indexInitialisationData.SysMemPitch = 0;
	indexInitialisationData.SysMemSlicePitch = 0;

	ThrowIfFailed(_device->CreateBuffer(&indexBufferDescriptor, &indexInitialisationData, _indexBuffer.GetAddressOf()));
}

void TerrainNode::BuildShaders()
{
	DWORD shaderCompileFlags = 0;
#if defined( _DEBUG )
	shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> compilationMessages = nullptr;

	//Compile vertex shader
	HRESULT hr = D3DCompileFromFile(L"TexturedShaders.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VShader", "vs_5_0",
		shaderCompileFlags, 0,
		_vertexShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	// Even if there are no compiler messages, check to make sure there were no other errors.
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreateVertexShader(_vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), NULL, _vertexShader.GetAddressOf()));

	// Compile pixel shader
	hr = D3DCompileFromFile(L"TexturedShaders.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PShader", "ps_5_0",
		shaderCompileFlags, 0,
		_pixelShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreatePixelShader(_pixelShaderByteCode->GetBufferPointer(), _pixelShaderByteCode->GetBufferSize(), NULL, _pixelShader.GetAddressOf()));
}

void TerrainNode::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
	// of each of the vertices we are sending to it.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ThrowIfFailed(_device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
}

void TerrainNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(_device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
}

void TerrainNode::BuildTexture()
{
	// Note that in order to use CreateWICTextureFromFile, we 
	// need to ensure we make a call to CoInitializeEx in our 
	// Initialise method (and make the corresponding call to 
	// CoUninitialize in the Shutdown method).  Otherwise, 
	// the following call will throw an exception
	ThrowIfFailed(CreateWICTextureFromFile(_device.Get(),
		_deviceContext.Get(),
		L"volcano.bmp",
		nullptr,
		_texture.GetAddressOf()
	));
}

bool TerrainNode::LoadHeightMap(wstring heightMapFilename)
{
	int fileSize = (NUMBER_OF_COLUMNS + 1) * (NUMBER_OF_ROWS + 1);
	BYTE * rawFileValues = new BYTE[fileSize];

	std::ifstream inputHeightMap;
	inputHeightMap.open(heightMapFilename.c_str(), std::ios_base::binary);
	if (!inputHeightMap)
	{
		return false;
	}

	inputHeightMap.read((char*)rawFileValues, fileSize);
	inputHeightMap.close();

	// Normalise BYTE values to the range 0.0f - 1.0f;
	for (size_t i = 0; i < fileSize; i++) //unsigned int
	{
		_heightValues.push_back((float)rawFileValues[i] / 255);
	}
	delete[] rawFileValues;
	return true;
}