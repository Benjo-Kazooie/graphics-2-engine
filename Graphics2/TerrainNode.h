#pragma once
#include "SceneNode.h"
#include "DirectXFramework.h"
#include "WICTextureLoader.h"
#include <vector>
#include <fstream>

struct TerrainVertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord0;
	XMFLOAT2 TexCoord1;
	int normalCount;
};

struct CBUFFER
{
	XMMATRIX    CompleteTransformation;
	XMMATRIX	WorldTransformation;
	XMVECTOR    LightVector;
	XMFLOAT4    LightColour;
	XMFLOAT4    AmbientColour;
	XMFLOAT4	DiffuseColour;
	XMFLOAT4	SpecularColour;
	FLOAT		shininess;
};

class TerrainNode : public SceneNode
{
public:
	TerrainNode(wstring name, wstring heightMap);
	~TerrainNode();


	bool Initialise(void);
private:
	ComPtr<ID3D11Device>			_device;
	ComPtr<ID3D11DeviceContext>		_deviceContext;

	ComPtr<ID3D11Buffer>			_vertexBuffer;
	ComPtr<ID3D11Buffer>			_indexBuffer;

	ComPtr<ID3D11RasterizerState>	_defaultRasteriserState;
	ComPtr<ID3D11RasterizerState>	_wireframeRasteriserState;

	ComPtr<ID3DBlob>				_vertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>				_pixelShaderByteCode = nullptr;
	ComPtr<ID3D11VertexShader>		_vertexShader;
	ComPtr<ID3D11PixelShader>		_pixelShader;
	ComPtr<ID3D11InputLayout>		_layout;
	ComPtr<ID3D11Buffer>			_constantBuffer;

	ComPtr<ID3D11ShaderResourceView> _texture;

	void BuildRendererStates();
	void GenerateVerticesAndIndices();
	void BuildBuffers();
	void Render();
	void BuildTexture();
	void Shutdown(void);
	void BuildShaders();
	void BuildConstantBuffer();
	void BuildVertexLayout();
	bool LoadHeightMap(wstring heightMapFilename);
	void GenerateNormals();
	XMFLOAT3 CrossProduct(XMFLOAT3 u, XMFLOAT3 v);

	vector<TerrainVertex> _vertices;
	int _amountOfSquares = 0;
	vector<UINT> _indices;
	vector<float> _heightValues;
	wstring _filename;
};

