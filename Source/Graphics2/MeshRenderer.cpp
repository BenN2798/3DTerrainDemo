#include "MeshRenderer.h"
#include "DirectXFramework.h"

struct CBUFFER
{
	XMMATRIX    CompleteTransformation;
	XMMATRIX	WorldTransformation;
	XMFLOAT4	CameraPosition;
	XMVECTOR    LightVector;
	XMFLOAT4    LightColor;
	XMFLOAT4    AmbientColor;
	XMFLOAT4    DiffuseColor;
	XMFLOAT4	SpecularColor;
	float		Shininess;
	float       Padding[3];
};

void MeshRenderer::SetMesh(shared_ptr<Mesh> mesh)
{
	_mesh = mesh;
}

void MeshRenderer::SetWorldTransformation(FXMMATRIX worldTransformation)
{
	XMStoreFloat4x4(&_worldTransformation, worldTransformation);
}

void MeshRenderer::SetAmbientLight(XMFLOAT4 ambientLight)
{
	_ambientLight = ambientLight;
}

void MeshRenderer::SetDirectionalLight(FXMVECTOR lightVector, XMFLOAT4 lightColour)
{
	_directionalLightColour = lightColour;
	XMStoreFloat4(&_directionalLightVector, lightVector);
}

void MeshRenderer::SetCameraPosition(XMFLOAT4 cameraPosition)
{
	_cameraPosition = cameraPosition;
}

bool MeshRenderer::Initialise()
{
	_device = DirectXFramework::GetDXFramework()->GetDevice();
	_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	return true;
}

void MeshRenderer::Render()
{
	XMMATRIX projectionTransformation = DirectXFramework::GetDXFramework()->GetProjectionTransformation();
	XMMATRIX viewTransformation = DirectXFramework::GetDXFramework()->GetCamera()->GetViewMatrix();

	XMMATRIX completeTransformation = XMLoadFloat4x4(&_worldTransformation) * viewTransformation * projectionTransformation;

	// Draw the first cube
	CBUFFER cBuffer;
	cBuffer.CompleteTransformation = completeTransformation;
	cBuffer.WorldTransformation = XMLoadFloat4x4(&_worldTransformation);
	cBuffer.AmbientColor = _ambientLight;
	cBuffer.LightVector = XMVector4Normalize(XMLoadFloat4(&_directionalLightVector)); 
	cBuffer.LightColor = _directionalLightColour;
	cBuffer.CameraPosition = _cameraPosition;

	_deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);
	_deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);
	_deviceContext->IASetInputLayout(_layout.Get());

	unsigned int subMeshCount = static_cast<unsigned int>(_mesh->GetSubMeshCount());
	// Loop through all submeshes in the mesh
	for (unsigned int i = 0; i < subMeshCount; i++)
	{
		shared_ptr<SubMesh> subMesh = _mesh->GetSubMesh(i);

		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		_vertexBuffer = subMesh->GetVertexBuffer();
		_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
		_indexBuffer = subMesh->GetIndexBuffer();
		_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		shared_ptr<Material> material = subMesh->GetMaterial();
		cBuffer.DiffuseColor = material->GetDiffuseColour();
		cBuffer.SpecularColor = material->GetSpecularColour();
		cBuffer.Shininess = material->GetShininess();
		// Update the constant buffer 
		_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
		_deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &cBuffer, 0, 0);
		_texture = material->GetTexture();
		_deviceContext->PSSetShaderResources(0, 1, _texture.GetAddressOf());
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_deviceContext->DrawIndexed(static_cast<UINT>(subMesh->GetIndexCount()), 0, 0);
	}
}

void MeshRenderer::Shutdown(void)
{
}

void MeshRenderer::BuildShaders()
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


void MeshRenderer::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
	// of each of the vertices we are sending to it.

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	ThrowIfFailed(_device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
}

void MeshRenderer::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ThrowIfFailed(_device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
}

