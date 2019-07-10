#pragma once
#include "Mesh.h"
#include "Renderer.h"
#include <map>

struct VERTEX
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
	XMFLOAT4 Colour;
};

struct MeshResourceStruct
{
	unsigned int			ReferenceCount;
	shared_ptr<Mesh>		MeshPointer;
};

typedef map<wstring, MeshResourceStruct>		MeshResourceMap;

struct MaterialResourceStruct
{
	unsigned int			ReferenceCount;
	shared_ptr<Material>	MaterialPointer;
};

typedef map<wstring, MaterialResourceStruct>	MaterialResourceMap;

typedef map<wstring, shared_ptr<Renderer>>		RendererResourceMap;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
				
	shared_ptr<Renderer>						GetRenderer(wstring rendererName);

	shared_ptr<Mesh>							GetMesh(wstring modelName);
	void										ReleaseMesh(wstring modelName);

	void										CreateMaterialFromTexture(wstring textureName);
    void										CreateMaterialWithNoTexture(wstring materialName, XMFLOAT4 diffuseColour, XMFLOAT4 specularColour, float shininess);
    void										CreateMaterial(wstring materialName, XMFLOAT4 diffuseColour, XMFLOAT4 specularColour, float shininess, wstring textureName);
	shared_ptr<Material>						GetMaterial(wstring materialName);
	void										ReleaseMaterial(wstring materialName);

private:
	MeshResourceMap								_meshResources;
	MaterialResourceMap							_materialResources;
	RendererResourceMap							_rendererResources;

	ComPtr<ID3D11Device>						_device;
	ComPtr<ID3D11DeviceContext>					_deviceContext;

	ComPtr<ID3D11ShaderResourceView>			_defaultTexture;
    
	shared_ptr<Mesh>			LoadModelFromFile(wstring modelName);
    void	                    InitialiseMaterial(wstring materialName, XMFLOAT4 diffuseColour, XMFLOAT4 specularColour, float shininess, wstring textureName);
};

