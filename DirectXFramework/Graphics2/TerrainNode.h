#pragma once

#ifndef TERRAIN_NODE
#define TERRAIN_NODE

#include "SceneNode.h"
#include "DirectXFramework.h"
#include <vector>
#include"WICTextureLoader.h"
#include <fstream>
#include "Camera.h"
#include "DDSTextureLoader.h"
#include <stdlib.h>
#include <ctime>

struct Vertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 TextureCoordinate;
	XMFLOAT2 BlendMapTexCoord;
};

struct cbuffer
{
	XMFLOAT4X4 completeTransformation;
	XMFLOAT4X4 worldTransformation;
	XMFLOAT4 cameraPosition;
	XMFLOAT4 lightVector;			// the light's vector
	XMFLOAT4 lightColor;			// the light's color
	XMFLOAT4 ambientColor;		// the ambient light's color
	XMFLOAT4 diffuseCoefficient;	// The diffuse reflection cooefficient
	XMFLOAT4 specularCoefficient;	// The specular reflection cooefficient
	float  shininess;			// The shininess factor
	float  opacity;				// The opacity (transparency) of the material. 0 = fully transparent, 1 = fully opaque
	XMFLOAT2 padding;
};

#endif  TERRAIN_NODE

class TerrainNode :
	public SceneNode
{
public:
	TerrainNode(wstring name, wstring heightMapFileName);
	~TerrainNode();

	bool Initialise() override;
	void Render() override;
	void Shutdown() override;

	//Initialise methods
	void SetupFramework();
	void BuildGeometryBuffers();
	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void BuildRendererStates();
	void LoadTerrainTextures();
	void GenerateBlendMap();
	bool LoadHeightMap(wstring heightMapFilename);
	void InitialiseGrid();
	float GetHeightAtPoint(float x, float z);

private:
	// stuff for a node
	ComPtr<ID3D11Device>			_device;
	ComPtr<ID3D11DeviceContext>		_deviceContext;

	ComPtr<ID3D11Buffer>			_vertexBuffer;
	ComPtr<ID3D11Buffer>			_indexBuffer;

	ComPtr<ID3DBlob>				_vertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>				_pixelShaderByteCode = nullptr;
	ComPtr<ID3D11VertexShader>		_vertexShader;
	ComPtr<ID3D11PixelShader>		_pixelShader;
	ComPtr<ID3D11InputLayout>		_layout;
	ComPtr<ID3D11Buffer>			_constantBuffer;

	XMFLOAT4X4						_viewTransformation;
	XMFLOAT4X4						_projectionTransformation;

	//texturey stuff
	ComPtr<ID3D11ShaderResourceView> _texturesResourceView;
	ComPtr<ID3D11ShaderResourceView> _blendMapResourceView;

	// specific to a terrainnode
	wstring _name; // name of the node
	wstring _fileName; // name of the height map file

	vector<Vertex>  _vertices; // collection of vertices
	unsigned int	_numberOfXPoints;
	unsigned int	_numberOfZPoints;
	int				_gridWidth; // distance between pixels
	int				_worldHeight;
	vector<UINT>	_indices;
	vector<float>	_heightValues; // loaded by loadheightmap

	ComPtr<ID3D11RasterizerState> _defaultRasteriserState;
	ComPtr<ID3D11RasterizerState> _wireframeRasteriserState;

};

