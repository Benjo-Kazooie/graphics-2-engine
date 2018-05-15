#include "TerrainNode.h"

#define NUMBER_OF_ROWS			256
#define NUMBER_OF_COLUMNS	    256

TerrainNode::TerrainNode(wstring name, wstring heightMap) : SceneNode(name)
{
	_filename = heightMap;
}

TerrainNode::~TerrainNode()
{
}

bool TerrainNode::Initialise()
{
	_device = DirectXFramework::GetDXFramework()->GetDevice();
	_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();

	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	LoadHeightMap(_filename);
	BuildTexture();
	BuildRendererStates();
	GenerateVerticesAndIndices();
	GenerateNormals();
	BuildBuffers();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	
	return true;
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

void TerrainNode::GenerateVerticesAndIndices()
{
	TerrainVertex temp;
	temp.Normal = XMFLOAT3(0, 0, 0);
	temp.TexCoord0 = XMFLOAT2(0, 0);
	temp.TexCoord1 = XMFLOAT2(0, 0);

	//Generates grid of vertices
	int iterator = 0;
	for (int z = 128; z > -128; z--)
	{	
		for (int x = -128; x < 128; x++)
		{
			//v1
			temp.Position = XMFLOAT3(float(x * 10), _heightValues[iterator] * 256, float(z * 10));
			temp.TexCoord0 = XMFLOAT2((x + 128) * (1.0f / NUMBER_OF_ROWS), (z + 128) * (1.0f / NUMBER_OF_COLUMNS));
			_vertices.push_back(temp);
			//v2
			temp.Position = XMFLOAT3(float((x + 1) * 10), _heightValues[iterator + 1] * 256, float(z * 10));
			temp.TexCoord0 = XMFLOAT2(((x + 128) + 1) * (1.0f / NUMBER_OF_ROWS), (z + 128) * (1.0f / NUMBER_OF_COLUMNS));
			_vertices.push_back(temp);
			//v3
			temp.Position = XMFLOAT3(float(x * 10), _heightValues[iterator + NUMBER_OF_ROWS] * 256, float((z - 1) * 10));
			temp.TexCoord0 = XMFLOAT2((x + 128) * (1.0f / NUMBER_OF_ROWS), ((z + 128) - 1) * (1.0f / NUMBER_OF_COLUMNS));
			_vertices.push_back(temp);
			//v4
			temp.Position = XMFLOAT3(float((x + 1) * 10), _heightValues[iterator + NUMBER_OF_ROWS + 1] * 256, float((z - 1) * 10));
			temp.TexCoord0 = XMFLOAT2(((x + 128) + 1) * (1.0f / NUMBER_OF_ROWS), ((z + 128) - 1) * (1.0f / NUMBER_OF_COLUMNS));
			_vertices.push_back(temp);
			_amountOfSquares++;
			iterator++;
		}
	}

	//Generates the list of indices
	for (size_t i = 0; i < _vertices.size(); i += 4)
	{
		UINT tempI = UINT(i);
		//v1
		_indices.push_back(tempI);
		//v2
		_indices.push_back(tempI + 1);
		//v3
		_indices.push_back(tempI + 2);

		//v3
		_indices.push_back(tempI + 2);
		//v2
		_indices.push_back(tempI + 1);
		//v4
		_indices.push_back(tempI + 3);
	}
}

void TerrainNode::BuildBuffers()
{
	// Setup the structure that specifies how big the vertex 
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor;
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(TerrainVertex) * UINT(_vertices.size());
	vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0;
	vertexBufferDescriptor.MiscFlags = 0;
	vertexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the vertices from
	D3D11_SUBRESOURCE_DATA vertexInitialisationData;
	vertexInitialisationData.pSysMem = &_vertices[0];

	// and create the vertex buffer
	ThrowIfFailed(_device->CreateBuffer(&vertexBufferDescriptor, &vertexInitialisationData, _vertexBuffer.GetAddressOf()));

	// Setup the structure that specifies how big the index 
	// buffer should be
	D3D11_BUFFER_DESC indexBufferDescriptor;
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(UINT) * UINT(_indices.size());
	indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0;
	indexBufferDescriptor.MiscFlags = 0;
	indexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the indices from
	D3D11_SUBRESOURCE_DATA indexInitialisationData;
	indexInitialisationData.pSysMem = &_indices[0];

	// and create the index buffer
	ThrowIfFailed(_device->CreateBuffer(&indexBufferDescriptor, &indexInitialisationData, _indexBuffer.GetAddressOf()));
}

void TerrainNode::Render()
{
	// Calculate the world x view x projection transformation
	//XMMATRIX viewTransform = DirectXFramework::GetDXFramework()->GetViewTransformation();
	XMMATRIX viewTransform = DirectXFramework::GetDXFramework()->GetCamera()->GetViewMatrix();
	XMMATRIX projectionTransform = DirectXFramework::GetDXFramework()->GetProjectionTransformation();
	XMMATRIX completeTransformation = XMLoadFloat4x4(&_combinedWorldTransformation) * viewTransform * projectionTransform;

	CBUFFER cBuffer;
	cBuffer.CompleteTransformation = completeTransformation;
	cBuffer.WorldTransformation = XMLoadFloat4x4(&_combinedWorldTransformation);
	cBuffer.AmbientColour = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	cBuffer.LightVector = XMVector4Normalize(XMVectorSet(0.0f, 01.0f, 1.0f, 0.0f));
	cBuffer.LightColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cBuffer.DiffuseColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cBuffer.SpecularColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cBuffer.shininess = 10.0f;

	// Update the constant buffer 
	_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &cBuffer, 0, 0);

	// Set the texture to be used by the pixel shader
	_deviceContext->PSSetShaderResources(0, 1, _texture.GetAddressOf());
	//_deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);
	//_deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);

	// Now render the terrain
	UINT stride = sizeof(TerrainVertex);
	UINT offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//_deviceContext->RSSetState(_wireframeRasteriserState.Get());
	_deviceContext->DrawIndexed(UINT(_indices.size()), 0, 0);
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
		L"rollinghills.bmp",
		nullptr,
		_texture.GetAddressOf()
	));
}

void TerrainNode::Shutdown()
{
}

void TerrainNode::BuildShaders()
{
	DWORD shaderCompileFlags = 0;
#if defined( _DEBUG )
	shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> compilationMessages = nullptr;

	//Compile vertex shader
	HRESULT hr = D3DCompileFromFile(L"TerrainShader.hlsl",
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
	_deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);

	// Compile pixel shader
	hr = D3DCompileFromFile(L"TerrainShader.hlsl",
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
	_deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);
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

void TerrainNode::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
	// of each of the vertices we are sending to it.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	ThrowIfFailed(_device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
	_deviceContext->IASetInputLayout(_layout.Get());
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
	for (int i = 0; i < fileSize; i++)
	{
		_heightValues.push_back((float)rawFileValues[i] / 255);
	}
	delete[] rawFileValues;
	return true;
}

void TerrainNode::GenerateNormals()
{
	int verticesSize = int(_vertices.size());
	for (int i = 0; i < verticesSize; i++)
	{
		_vertices[i].normalCount = 0;
	}

	int iterator;
	for (int i = 0; i < _amountOfSquares; i++)
	{
		iterator = i * 4;
		XMFLOAT3 v1 = _vertices[iterator].Position;
		XMFLOAT3 v2 = _vertices[iterator + 1].Position;
		XMFLOAT3 v3 = _vertices[iterator + 2].Position;
		XMFLOAT3 v4 = _vertices[iterator + 3].Position;

		//Vectors for triangle 1
		XMFLOAT3 vector1A;
		vector1A.x = v2.x - v1.x;
		vector1A.y = v2.y - v1.y;
		vector1A.z = v2.z - v1.z;
		XMFLOAT3 vector1B;
		vector1B.x = v3.x - v1.x;
		vector1B.y = v3.y - v1.y;
		vector1B.z = v3.z - v1.z;

		XMFLOAT3 normal1 = CrossProduct(vector1B, vector1A);
		_vertices[iterator].Normal.x = (_vertices[iterator].Normal.x + normal1.x);
		_vertices[iterator].Normal.y = (_vertices[iterator].Normal.y + normal1.y);
		_vertices[iterator].Normal.z = (_vertices[iterator].Normal.z + normal1.z);
		_vertices[iterator].normalCount++;

		_vertices[iterator + 1].Normal.x = (_vertices[iterator].Normal.x + normal1.x);
		_vertices[iterator + 1].Normal.y = (_vertices[iterator].Normal.y + normal1.y);
		_vertices[iterator + 1].Normal.z = (_vertices[iterator].Normal.z + normal1.z);
		_vertices[iterator + 1].normalCount++;

		_vertices[iterator + 2].Normal.x = (_vertices[iterator].Normal.x + normal1.x);
		_vertices[iterator + 2].Normal.y = (_vertices[iterator].Normal.y + normal1.y);
		_vertices[iterator + 2].Normal.z = (_vertices[iterator].Normal.z + normal1.z);
		_vertices[iterator + 2].normalCount++;

		//Vectors for triangle 2
		XMFLOAT3 vector2A;
		vector2A.x = v2.x - v3.x;
		vector2A.y = v2.y - v3.y;
		vector2A.z = v2.z - v3.z;
		XMFLOAT3 vector2B;
		vector2B.x = v4.x - v3.x;
		vector2B.y = v4.y - v3.y;
		vector2B.z = v4.z - v3.z;

		XMFLOAT3 normal2 = CrossProduct(vector2B, vector2A);
		_vertices[iterator + 2].Normal.x = (_vertices[iterator].Normal.x + normal1.x);
		_vertices[iterator + 2].Normal.y = (_vertices[iterator].Normal.y + normal1.y);
		_vertices[iterator + 2].Normal.z = (_vertices[iterator].Normal.z + normal1.z);
		_vertices[iterator + 2].normalCount++;

		_vertices[iterator + 1].Normal.x = (_vertices[iterator].Normal.x + normal1.x);
		_vertices[iterator + 1].Normal.y = (_vertices[iterator].Normal.y + normal1.y);
		_vertices[iterator + 1].Normal.z = (_vertices[iterator].Normal.z + normal1.z);
		_vertices[iterator + 1].normalCount++;

		_vertices[iterator + 3].Normal.x = (_vertices[iterator].Normal.x + normal1.x);
		_vertices[iterator + 3].Normal.y = (_vertices[iterator].Normal.y + normal1.y);
		_vertices[iterator + 3].Normal.z = (_vertices[iterator].Normal.z + normal1.z);
		_vertices[iterator + 3].normalCount++;
	}

	for (int i = 0; i < verticesSize; i++)
	{
		_vertices[i].Normal.x = (_vertices[i].Normal.x / float(_vertices[i].normalCount));
		_vertices[i].Normal.y = (_vertices[i].Normal.y / float(_vertices[i].normalCount));
		_vertices[i].Normal.z = (_vertices[i].Normal.z / float(_vertices[i].normalCount));

		float length = (_vertices[i].Normal.x * _vertices[i].Normal.x
					  + _vertices[i].Normal.y * _vertices[i].Normal.y
					  + _vertices[i].Normal.z * _vertices[i].Normal.z);
		_vertices[i].Normal.x = _vertices[i].Normal.x / length;
		_vertices[i].Normal.y = _vertices[i].Normal.y / length;
		_vertices[i].Normal.z = _vertices[i].Normal.z / length;
	}
}

XMFLOAT3 TerrainNode::CrossProduct(XMFLOAT3 u, XMFLOAT3 v)
{
	XMFLOAT3 temp;
	temp.x = (u.y * v.z - u.z * v.y);
	temp.y = (u.z * v.x - u.x * v.z);
	temp.z = (u.x * v.y - u.y * v.x);
	return temp;
}