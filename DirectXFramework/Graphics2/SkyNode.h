#pragma once

#ifndef SKYNODE_H
#define SKYNODE_H

#include "SceneNode.h"
#include <vector>
#include "DirectXFramework.h"
#include "DDSTextureLoader.h"

struct SkyVertex { XMFLOAT3 Position; };
struct CBUFFER { XMMATRIX completeTransformation; };
#endif

class SkyNode :
	public SceneNode
{
public:
	SkyNode(wstring name, wstring textureFileName, float radius);
	~SkyNode();

	bool Initialise() override;
	void Render() override;
	void Shutdown() override;

	void CreateSphere(float radius, size_t tesselation);
	void BuildRendererStates();
	void BuildDepthStencilState();
	void SetupFramework();
	void BuildGeometryBuffers();
	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void LoadSkyTexture();

private:
	wstring _name;
	wstring _textureFileName;
	float _radius;
	vector<UINT> _indices;
	vector<SkyVertex> _vertices;
	UINT _numberOfVertices;
	UINT _numberOfIndices;

	ComPtr<ID3D11RasterizerState>   _defaultRasteriserState;
	ComPtr<ID3D11RasterizerState>   _wireframeRasteriserState;
	ComPtr<ID3D11RasterizerState>   _noCullRasteriserState;

	ComPtr<ID3D11DepthStencilState>	 _stencilState;
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

	D3D11_VIEWPORT					_screenViewport;

	XMFLOAT4X4						_viewTransformation;
	XMFLOAT4X4						_projectionTransformation;

	ComPtr<ID3D11ShaderResourceView> _textureResourceView;

};

