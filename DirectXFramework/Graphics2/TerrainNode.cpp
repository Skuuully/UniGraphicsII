#include "TerrainNode.h"

TerrainNode::TerrainNode(wstring name, wstring heightMapFileName) : SceneNode(name)
{
	_name = name;
	_fileName = heightMapFileName;
}


TerrainNode::~TerrainNode()
{
}

bool TerrainNode::Initialise()
{
	_numberOfXPoints = 1024;
	_numberOfZPoints = 1024;

	SetupFramework();
	LoadHeightMap(_fileName);
	BuildGeometryBuffers();
	LoadTerrainTextures();
	GenerateBlendMap();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	BuildRendererStates();


	return true;
}

void TerrainNode::Render()
{
	XMMATRIX viewTransformation = DirectXFramework::GetDXFramework()->GetCamera()->GetViewMatrix();
	XMStoreFloat4x4(&_viewTransformation, viewTransformation);

	XMMATRIX completeTransformation = XMLoadFloat4x4(&_combinedWorldTransformation) * XMLoadFloat4x4(&_viewTransformation) * XMLoadFloat4x4(&_projectionTransformation);

	// Draw the thing
	cbuffer cbuffer;
	cbuffer.opacity = 0.9f;
	cbuffer.shininess = 0.8f;
	XMStoreFloat4x4(&cbuffer.completeTransformation, completeTransformation);
	cbuffer.worldTransformation = _combinedWorldTransformation;
	cbuffer.ambientColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	cbuffer.lightVector = XMFLOAT4(1.0f, -1.0f, 0.0f, 0.0f);
	cbuffer.lightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cbuffer.diffuseCoefficient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); 
	cbuffer.specularCoefficient = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.1f);
	XMStoreFloat4(&cbuffer.cameraPosition, DirectXFramework::GetDXFramework()->GetCamera()->GetCameraPosition());
	cbuffer.padding = XMFLOAT2();
	
	// Update the constant buffer 
	_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->PSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &cbuffer, 0, 0);
	_deviceContext->PSSetShaderResources(0, 1, _blendMapResourceView.GetAddressOf());
	_deviceContext->PSSetShaderResources(1, 1, _texturesResourceView.GetAddressOf());

	// Set the texture to be used by the pixel shader
	//_deviceContext->PSSetShaderResources(0, 1, _texture.GetAddressOf());

	// Now render the thing
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	_deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);
	_deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);
	_deviceContext->IASetInputLayout(_layout.Get());
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Sets to wireframe mode
	//_deviceContext->RSSetState(_wireframeRasteriserState.Get());

	_deviceContext->DrawIndexed((UINT)_indices.size(), 0, 0);
}

void TerrainNode::Shutdown()
{
}

void TerrainNode::SetupFramework()
{
	_device = DirectXFramework::GetDXFramework()->GetDevice();
	_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();
	XMMATRIX viewTransformation = DirectXFramework::GetDXFramework()->GetCamera()->GetViewMatrix();
	XMStoreFloat4x4(&_viewTransformation, viewTransformation);
	XMMATRIX projection = DirectXFramework::GetDXFramework()->GetProjectionTransformation();
	XMStoreFloat4x4(&_projectionTransformation, projection);
}

void TerrainNode::BuildGeometryBuffers()
{
	//sets up vertices and indices
	InitialiseGrid();

	// Setup the structure that specifies how big the vertex 
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor;
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(Vertex) * (UINT)_vertices.size();
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
	indexBufferDescriptor.ByteWidth = sizeof(UINT) * (UINT)_indices.size();
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
}

void TerrainNode::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
// of each of the vertices we are sending to it.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ThrowIfFailed(_device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
}

void TerrainNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(cbuffer);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(_device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
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

void TerrainNode::LoadTerrainTextures()
{
	// Change the paths below as appropriate for your use
	wstring terrainTextureNames[5] = { L"grass.dds", L"darkdirt.dds", L"stone.dds", L"lightdirt.dds", L"snow.dds" };
	int numberOfTextures = sizeof(terrainTextureNames) / sizeof(*terrainTextureNames);

	// Load the textures from the files
	ComPtr<ID3D11Resource> terrainTextures[5];
	for (int i = 0; i < numberOfTextures; i++)
	{
		ThrowIfFailed(CreateDDSTextureFromFileEx(_device.Get(),
			_deviceContext.Get(),
			terrainTextureNames[i].c_str(),
			0,
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			false,
			terrainTextures[i].GetAddressOf(),
			nullptr
		));
	}
	// Now create the Texture2D arrary.  We assume all textures in the
	// array have the same format and dimensions

	D3D11_TEXTURE2D_DESC textureDescription;
	ComPtr<ID3D11Texture2D> textureInterface;
	terrainTextures[0].As<ID3D11Texture2D>(&textureInterface);
	textureInterface->GetDesc(&textureDescription);

	D3D11_TEXTURE2D_DESC textureArrayDescription;
	textureArrayDescription.Width = textureDescription.Width;
	textureArrayDescription.Height = textureDescription.Height;
	textureArrayDescription.MipLevels = textureDescription.MipLevels;
	textureArrayDescription.ArraySize = numberOfTextures;
	textureArrayDescription.Format = textureDescription.Format;
	textureArrayDescription.SampleDesc.Count = 1;
	textureArrayDescription.SampleDesc.Quality = 0;
	textureArrayDescription.Usage = D3D11_USAGE_DEFAULT;
	textureArrayDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureArrayDescription.CPUAccessFlags = 0;
	textureArrayDescription.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> textureArray = 0;
	ThrowIfFailed(_device->CreateTexture2D(&textureArrayDescription, 0, textureArray.GetAddressOf()));

	// Copy individual texture elements into texture array.
	for (UINT i = 0; i < (unsigned)numberOfTextures; i++)
	{
		// For each mipmap level...
		for (UINT mipLevel = 0; mipLevel < textureDescription.MipLevels; mipLevel++)
		{
			_deviceContext->CopySubresourceRegion(textureArray.Get(),
				D3D11CalcSubresource(mipLevel, i, textureDescription.MipLevels),
				NULL,
				NULL,
				NULL,
				terrainTextures[i].Get(),
				mipLevel,
				nullptr
			);
		}
	}

	// Create a resource view to the texture array.
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDescription;
	viewDescription.Format = textureArrayDescription.Format;
	viewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDescription.Texture2DArray.MostDetailedMip = 0;
	viewDescription.Texture2DArray.MipLevels = textureArrayDescription.MipLevels;
	viewDescription.Texture2DArray.FirstArraySlice = 0;
	viewDescription.Texture2DArray.ArraySize = numberOfTextures;

	ThrowIfFailed(_device->CreateShaderResourceView(textureArray.Get(), &viewDescription, _texturesResourceView.GetAddressOf()));
}

void TerrainNode::GenerateBlendMap()
{
	// Note that _numberOfRows and _numberOfColumns need to be setup
	// to the number of rows and columns in your grid in order for this
	// to work.
	UINT _numberOfRows = _numberOfXPoints;
	UINT _numberOfColumns = _numberOfZPoints;

	DWORD * blendMap = new DWORD[_numberOfRows * _numberOfColumns];
	DWORD * blendMapPtr = blendMap;

	// all 0 = grass
	BYTE darkDirt; // dark dirt
	BYTE stone; // stone
	BYTE lightDirt; // light dirt
	BYTE snow; // snow

	DWORD index = 0;
	for (DWORD i = 0; i < _numberOfColumns; i++)
	{
		for (DWORD j = 0; j < _numberOfRows; j++)
		{

			// Calculate the appropriate blend colour for the 
			// current location in the blend map.  This has been
			// left as an exercise for you.  You need to calculate
			// appropriate values for the r, g, b and a values (each
			// between 0 and 255). The code below combines these
			// into a DWORD (32-bit value) and stores it in the blend map.
			int currentIndex = ((i * _numberOfColumns) + j) * 4;

			Vertex bottomLeft = _vertices[currentIndex];
			Vertex bottomRight = _vertices[(currentIndex) + 1];
			Vertex topLeft = _vertices[(currentIndex) + 2];
			Vertex topRight = _vertices[(currentIndex) + 3];

			float averageHeight = (bottomLeft.Position.y + bottomRight.Position.y + topLeft.Position.y + topRight.Position.y) / 4;
			float averageYNormal = (bottomLeft.Normal.y + bottomRight.Normal.y + topLeft.Normal.y + topRight.Normal.y) / 4;
			snow = 0;
			lightDirt = 0;
			stone = 0;
			darkDirt = 0;

			
			if (averageHeight < 200.0f)
			{
				lightDirt = 255;
			}
			else if (averageHeight < 400.0f)
			{
				// avgheight % range gets number between 0 and max range, divide by range to get between 0 and 1 then multiply by 255
				lightDirt = (BYTE)(255 - (((int)averageHeight % 200) / 200.0f) * 255);
			}
			else if (averageHeight < 600.0f)
			{

			}
			else if (averageHeight < 700.0f)
			{
				darkDirt = (BYTE)((((int)averageHeight % 100) / 100.0f) * 255);
			}
			else if (averageHeight < 750.0f)
			{
				darkDirt = 255;
			}
			else if (averageHeight < 800.0f)
			{
				stone = (BYTE)((((int)averageHeight % 50) / 50.0f) * 255);
				darkDirt = 255 - stone;
			}
			else
			{
				stone = 255;
			}

			DWORD mapValue = (snow << 24) + (lightDirt << 16) + (stone << 8) + darkDirt;
			*blendMapPtr++ = mapValue;
		}
	}
	D3D11_TEXTURE2D_DESC blendMapDescription;
	blendMapDescription.Width = _numberOfRows;
	blendMapDescription.Height = _numberOfColumns;
	blendMapDescription.MipLevels = 1;
	blendMapDescription.ArraySize = 1;
	blendMapDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	blendMapDescription.SampleDesc.Count = 1;
	blendMapDescription.SampleDesc.Quality = 0;
	blendMapDescription.Usage = D3D11_USAGE_DEFAULT;
	blendMapDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	blendMapDescription.CPUAccessFlags = 0;
	blendMapDescription.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA blendMapInitialisationData;
	blendMapInitialisationData.pSysMem = blendMap;
	blendMapInitialisationData.SysMemPitch = 4 * _numberOfColumns;

	ComPtr<ID3D11Texture2D> blendMapTexture;
	ThrowIfFailed(_device->CreateTexture2D(&blendMapDescription, &blendMapInitialisationData, blendMapTexture.GetAddressOf()));

	// Create a resource view to the texture array.
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDescription;
	viewDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	viewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDescription.Texture2D.MostDetailedMip = 0;
	viewDescription.Texture2D.MipLevels = 1;

	ThrowIfFailed(_device->CreateShaderResourceView(blendMapTexture.Get(), &viewDescription, _blendMapResourceView.GetAddressOf()));
	delete[] blendMap;
}

bool TerrainNode::LoadHeightMap(wstring heightMapFilename)
{
	unsigned int mapSize = _numberOfXPoints * _numberOfZPoints;
	USHORT * rawFileValues = new USHORT[mapSize];

	std::ifstream inputHeightMap;
	inputHeightMap.open(heightMapFilename.c_str(), std::ios_base::binary);
	if (!inputHeightMap)
	{
		return false;
	}

	inputHeightMap.read((char*)rawFileValues, mapSize * 2);
	inputHeightMap.close();

	// Normalise BYTE values to the range 0.0f - 1.0f;
	for (unsigned int i = 0; i < mapSize; i++)
	{
		_heightValues.push_back((float)rawFileValues[i] / 65536);
	}
	delete[] rawFileValues;
	return true;
}

void TerrainNode::InitialiseGrid()
{
	srand((unsigned int)time(NULL));
	_gridWidth = 10;
	_worldHeight = 1024;
	int yPos = 0;
	Vertex bottomLeft = Vertex();
	Vertex bottomRight = Vertex();
	Vertex topLeft = Vertex();
	Vertex topRight = Vertex();
	int indexCounter = 0;
	int heightIndex = 0;

	int xStartPoint = (_numberOfXPoints / 2) * -1;
	int zStartPoint = (_numberOfZPoints / 2) * -1;

	for (int xPos = xStartPoint; xPos < (int)(_numberOfXPoints / 2); xPos++)
	{
		for (int zPos = zStartPoint; zPos < (int)(_numberOfZPoints / 2); zPos++)
		{
			int botLeftYPos = (int)(_heightValues[heightIndex] * _worldHeight);
			int botRightYPos = (int)(_heightValues[heightIndex] * _worldHeight);
			int topLeftY = (int)(_heightValues[heightIndex] * _worldHeight);
			int topRightY = (int)(_heightValues[heightIndex] * _worldHeight);

			if (heightIndex + 1 < _heightValues.size())
			{
				botRightYPos = (int)(_heightValues[heightIndex + 1] * _worldHeight);
			}
			if (heightIndex + _numberOfZPoints < _heightValues.size())
			{
				topLeftY = (int)(_heightValues[heightIndex + _numberOfZPoints] * _worldHeight);
			}
			if (heightIndex + _numberOfZPoints + 1 < _heightValues.size())
			{
				topRightY = (int)(_heightValues[heightIndex + _numberOfZPoints + 1] * _worldHeight);
			}

			// Increment the height Index
			heightIndex++;
			// Set postion
			int x0 = xPos + 512;
			int z0 = zPos + 512;

			bottomLeft.Position = XMFLOAT3((float)(xPos * _gridWidth), (float)botLeftYPos, (float)(zPos * _gridWidth));
			bottomLeft.TextureCoordinate = XMFLOAT2(0.0f, 1.0f);
			bottomLeft.BlendMapTexCoord = XMFLOAT2((1.0f / (float)_numberOfZPoints) * z0, (1.0f / (float)_numberOfXPoints) * x0);

			bottomRight.Position = XMFLOAT3((float)(xPos * _gridWidth), (float)botRightYPos, (float)((zPos + 1) * _gridWidth));
			bottomRight.TextureCoordinate = XMFLOAT2(1.0f, 1.0f);
			bottomRight.BlendMapTexCoord = XMFLOAT2((1.0f / (float)_numberOfZPoints) * (z0 + 1), (1.0f / (float)_numberOfXPoints) * x0);

			topLeft.Position = XMFLOAT3((float)((xPos + 1) * _gridWidth), (float)topLeftY, (float)(zPos * _gridWidth));
			topLeft.TextureCoordinate = XMFLOAT2(0.0f, 0.0f);
			topLeft.BlendMapTexCoord = XMFLOAT2((1.0f / (float)_numberOfZPoints) * z0, (1.0f / (float)_numberOfXPoints) * (x0 + 1));

			topRight.Position = XMFLOAT3((float)((xPos + 1) * _gridWidth), (float)topRightY, (float)((zPos + 1) * _gridWidth));
			topRight.TextureCoordinate = XMFLOAT2(1.0f , 0.0f);
			topRight.BlendMapTexCoord = XMFLOAT2((1.0f / (float)_numberOfZPoints) * (z0 + 1), (1.0f / (float)_numberOfXPoints) * (x0 + 1));

			// add vertices to vector
			_vertices.push_back(bottomLeft);
			_vertices.push_back(bottomRight);
			_vertices.push_back(topLeft);
			_vertices.push_back(topRight);

			//top left triangle
			_indices.push_back(indexCounter);
			_indices.push_back(indexCounter + 1);
			_indices.push_back(indexCounter + 2);

			//bottom right triangle
			_indices.push_back(indexCounter + 2);
			_indices.push_back(indexCounter + 1);
			_indices.push_back(indexCounter + 3);

			// increase to next square counter
			indexCounter += 4;
		}
	}

	for (int xPos = 0; (unsigned)xPos < _numberOfXPoints; xPos++)
	{
		for (int zPos = 0; (unsigned)zPos < _numberOfZPoints; zPos++)
		{
			if (!(xPos == 0 || (unsigned)xPos == _numberOfXPoints - 1 || zPos == 0 || (unsigned)zPos == _numberOfZPoints - 1))
			{
				int vertexIndex = ((xPos * _numberOfZPoints) + zPos) * 4;
				Vertex currentSet[16];
				currentSet[0] = _vertices[vertexIndex]; // currentBottomLeft
				currentSet[1] = _vertices[vertexIndex + 1]; // currentBottomRight
				currentSet[2] = _vertices[vertexIndex + 2]; // currentTopLeft
				currentSet[3] = _vertices[vertexIndex + 3]; // currentTopRight
				currentSet[4] = _vertices[((((xPos + 1) * _numberOfZPoints) + zPos - 1) * 4) + 1]; // topLeftLeft
				currentSet[5] = _vertices[((((xPos + 1) * _numberOfZPoints) + zPos) * 4)]; // topMidLeft
				currentSet[6] = _vertices[((((xPos + 1) * _numberOfZPoints) + zPos) * 4) + 1]; // topMidRight
				currentSet[7] = _vertices[((((xPos + 1) * _numberOfZPoints) + zPos + 1) * 4)]; // topRightRight
				currentSet[8] = _vertices[(((xPos * _numberOfZPoints) + zPos + 1) * 4) + 2]; // midRightTop
				currentSet[9] = _vertices[(((xPos * _numberOfZPoints) + zPos + 1) * 4)]; // midRightBottom
				currentSet[10] = _vertices[((((xPos - 1) * _numberOfZPoints) + zPos + 1) * 4) + 2]; // bottomRightRight
				currentSet[11] = _vertices[((((xPos - 1) * _numberOfZPoints) + zPos) * 4) + 3]; // bottomMidRight
				currentSet[12] = _vertices[((((xPos - 1) * _numberOfZPoints) + zPos) * 4) + 2]; // bottomMidLeft
				currentSet[13] = _vertices[((((xPos - 1) * _numberOfZPoints) + zPos - 1) * 4) + 3]; // bottomLeftLeft
				currentSet[14] = _vertices[(((xPos * _numberOfZPoints) + zPos - 1) * 4) + 1]; // midLeftBottom
				currentSet[15] = _vertices[(((xPos * _numberOfZPoints) + zPos - 1) * 4) + 3]; // midLeftTop

				XMVECTOR vector1 = { (currentSet[0].Position.x - currentSet[1].Position.x), (currentSet[0].Position.y - currentSet[1].Position.y), (currentSet[0].Position.z - currentSet[1].Position.z) };
				XMVECTOR vector2 = { (currentSet[0].Position.x - currentSet[3].Position.x), (currentSet[0].Position.y - currentSet[3].Position.y), (currentSet[0].Position.z - currentSet[3].Position.z) };
				XMVECTOR cross = XMVector3Cross(vector1, vector2);
				cross = XMVector3Normalize(cross);

				for (int i = 0; i < 16; i++)
				{
					XMStoreFloat3(&currentSet[i].Normal, XMLoadFloat3(&currentSet[i].Normal) + cross);
				}

				_vertices[vertexIndex] = currentSet[0];
				_vertices[vertexIndex + 1] = currentSet[1];
				_vertices[vertexIndex + 2] = currentSet[2];
				_vertices[vertexIndex + 3] = currentSet[3];
				_vertices[((((xPos + 1) * _numberOfZPoints) + zPos - 1) * 4) + 1] = currentSet[4];
				_vertices[((((xPos + 1) * _numberOfZPoints) + zPos) * 4)] = currentSet[5];
				_vertices[((((xPos + 1) * _numberOfZPoints) + zPos) * 4) + 1] = currentSet[6];
				_vertices[((((xPos + 1) * _numberOfZPoints) + zPos + 1) * 4)] = currentSet[7];
				_vertices[(((xPos * _numberOfZPoints) + zPos + 1) * 4) + 2] = currentSet[8];
				_vertices[(((xPos * _numberOfZPoints) + zPos + 1) * 4)] = currentSet[9];
				_vertices[((((xPos - 1) * _numberOfZPoints) + zPos + 1) * 4) + 2] = currentSet[10];
				_vertices[((((xPos - 1) * _numberOfZPoints) + zPos) * 4) + 3] = currentSet[11];
				_vertices[((((xPos - 1) * _numberOfZPoints) + zPos) * 4) + 2] = currentSet[12];
				_vertices[((((xPos - 1) * _numberOfZPoints) + zPos - 1) * 4) + 3] = currentSet[13];
				_vertices[(((xPos * _numberOfZPoints) + zPos - 1) * 4) + 1] = currentSet[14];
				_vertices[(((xPos * _numberOfZPoints) + zPos - 1) * 4) + 3] = currentSet[15];
			}
		}
	}

	//for (int i = 0; i < _vertices.size(); i++)
	//{
	//	XMStoreFloat3(&_vertices[i].Normal, XMVector3Normalize(XMLoadFloat3(&_vertices[i].Normal)));
	//}
}

float TerrainNode::GetHeightAtPoint(float x, float z)
{
	// check if the x and z positions are even on the grid
	if (x <= _vertices[_vertices.size() - 1].Position.x && z < _vertices[_vertices.size() - 1].Position.z)
	{
		float terrainStartX = _vertices[0].Position.x;
		float terrainStartZ = _vertices[0].Position.z;

		// get which sqaure of grid we are in
		int squareX = (int)((x - terrainStartX) / _gridWidth);
		int squareZ = (int)((z - terrainStartZ) / _gridWidth);

		int index = ((squareX * (int)_numberOfZPoints) + squareZ) * 4;
		Vertex topLeft = _vertices[index + 2];
		Vertex topRight = _vertices[index + 3];
		Vertex bottomRight = _vertices[index + 1];
		Vertex bottomLeft = _vertices[index];

		float differenceInX = x - topLeft.Position.x;
		float differenceInZ = z - topLeft.Position.z;

		XMVECTOR topRightNormal = XMLoadFloat3(&topRight.Normal);
		XMVECTOR bottomLeftNormal = XMLoadFloat3(&bottomLeft.Normal);

		if (differenceInX <= differenceInZ)
		{
			XMVECTOR topLeftNormal = XMLoadFloat3(&topLeft.Normal);

			XMVECTOR vector1 = XMVectorSubtract(topLeftNormal, topRightNormal);
			XMVECTOR vector2 = XMVectorSubtract(topLeftNormal, bottomLeftNormal);

			XMVECTOR cross = XMVector3Cross(vector1, vector2);
			cross = XMVector3Normalize(cross);
			XMFLOAT3 normal;
			XMStoreFloat3(&normal, cross);

			float normalX = normal.x;
			float normalY = normal.y;
			float normalZ = normal.z;
			float bottomLeftY = bottomLeft.Position.y;

			float position = bottomLeftY + ((normalX * differenceInX + normalZ * differenceInZ) / (-normalY));
			return position;
		}
		else
		{
			XMVECTOR bottomRightNormal = XMLoadFloat3(&bottomRight.Normal);;

			XMVECTOR vector1 = XMVectorSubtract(bottomRightNormal, topRightNormal);
			XMVECTOR vector2 = XMVectorSubtract(bottomRightNormal, bottomLeftNormal);

			XMVECTOR cross = XMVector3Cross(vector1, vector2);
			cross = XMVector3Normalize(cross);
			XMFLOAT3 normal;
			XMStoreFloat3(&normal, cross);

			float normalX = normal.x;
			float normalY = normal.y;
			float normalZ = normal.z;
			float bottomLeftY = bottomLeft.Position.y;

			float position = bottomLeftY + ((normalX * differenceInX + normalZ * differenceInZ) / (-normalY));
			return position;
		}
	}
}
